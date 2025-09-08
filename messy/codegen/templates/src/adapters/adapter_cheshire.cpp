#include <adapters/adapter_cheshire.hpp>
#include <stdint.h>

/* -------------------------------------------------------------------------- */
/*                              GdbServer methods                             */
/* -------------------------------------------------------------------------- */

// GdbServer implementation
GdbServer::GdbServer()
{
    closed = true;
    gdb_pid = -1;
    gdb_in = nullptr;
    gdb_out = nullptr;
    dump_file = nullptr;
    memset(gdb_out_buf, 0, sizeof(gdb_out_buf));
}

GdbServer::~GdbServer()
{
    close();
}

/**
 * @brief Sets up the GDB connection to Cheshire.
 *
 * This function sets up the GDB connection to Cheshire.
 * It forks a new process and runs GDB, redirecting GDB I/O to pipes.
 * GDB will be used to start, continue and stop bare metal programs running on Cheshire.
 */
void GdbServer::setup()
{
    // Open dump file for memory dumps
    this->dump_file = fopen(CHESSY_TMP_FILE, "w+b");
    if (!this->dump_file) {
        perror("Failed to open dump file");
        exit(1);
    }

    // Create pipes for GDB input and output redirection
    if (pipe(this->gdb_in_fd) == -1 || pipe(this->gdb_out_fd) == -1) {
        perror("Pipe creation failed");
        exit(1);
    }

    // Fork a new process and run GDB targeting OpenOCD server
    pid_t pid = fork();
    if (pid < 0) {
        perror("GDB fork failed");
        exit(1);
    } else if (pid == 0) {

        /* Child process */

        // Redirect standard input and output to the pipes
        dup2(this->gdb_in_fd[0], STDIN_FILENO);
        dup2(this->gdb_out_fd[1], STDOUT_FILENO);

        // Close unused pipe ends
        ::close(this->gdb_in_fd[1]);
        ::close(this->gdb_out_fd[0]);

        // Launch gdb in MI mode
        execlp("riscv64-unknown-elf-gdb", "riscv64-unknown-elf-gdb", "--interpreter=mi2", "--quiet", "--nx", NULL);
        perror("GDB execlp failed"); // TODO: it should be checked by the parent process
        exit(1);
    } else {

        /* Parent process */

        this->gdb_pid = pid; // Store the child process ID

        // Close unused pipe ends
        ::close(gdb_in_fd[0]);
        ::close(gdb_out_fd[1]);

        // Store the pipe file descriptors for parent to use
        this->gdb_in  = fdopen(this->gdb_in_fd[1], "w");
        this->gdb_out = fdopen(this->gdb_out_fd[0], "r");
        if (!this->gdb_in || !this->gdb_out) {
            perror("fdopen failed");
            exit(1);
        }
        fflush(this->gdb_out);

        // Set up GDB connection
        this->send_command("-gdb-set pagination off");
        this->send_command("-gdb-set confirm off");
        this->send_command("-gdb-set verbose off");
        this->send_command("-target-select extended-remote localhost:" GDB_PORT);
        this->send_command("-file-exec-and-symbols " CHESSY_TEST_BIN);
        this->send_command("-target-download");
        this->wait_for_line("^done,address="); // Wait for GDB to finish downloading the program
        this->flush_output();

        closed = false;
    }
}

void GdbServer::close()
{
    if (!closed) {
        // Exit GDB processes
        this->send_command("-gdb-exit");

        // Close pipes
        if (gdb_in) {
            fflush(gdb_in);
            fclose(gdb_in);
            gdb_in = nullptr;
        }
        if (gdb_out) {
            fclose(gdb_out);
            gdb_out = nullptr;
        }

        closed = true;
    }
}

std::string GdbServer::wait_for_line(const std::string &keyword)
{
    std::string line;

    while (true) {
        if (fgets(this->gdb_out_buf, sizeof(this->gdb_out_buf), this->gdb_out)) {
            DEBUG_PRINT_GDB("[GDB]: %s", this->gdb_out_buf);
            line = std::string(this->gdb_out_buf);
            if (line.rfind(keyword, 0) == 0) {
                // Found the line starting with the keyword
                return line;
            } else if (line.rfind("^error", 0) == 0) {
                // An error occurred
                throw std::runtime_error("GDB returned an error: " + line);
            }
        } else {
            throw std::runtime_error("Failed to read from GDB output.");
        }
    }
}

void GdbServer::send_command(const std::string &cmd)
{
    // Send command to GDB
    fprintf(this->gdb_in, "%s\n", cmd.c_str());
        DEBUG_PRINT_GDB("GDB command sent: %s\n", cmd.c_str());
    fflush(this->gdb_in);
}

/**
 * @brief Reads the machine timer register from Cheshire through GDB.
 *
 * This function reads the machine timer register from Cheshire through GDB.
 * It sends a command to read the machine timer and parses the response.
 *
 * @return The value of the machine timer.
 */
uint64_t GdbServer::read_mtime()
{
    // TODO: make this generic for any register
    std::string line;
    int ret;

    // Regex to match the memory read response
    std::regex memory_regex("data=\\[\"([^\"]+)\"\\]");
    std::smatch match;

    // Flush the GDB output buffer
    this->flush_output();
    this->send_command("-data-read-memory 0x" CHS_MTIME_REG " u 8 1 1");

    line = this->wait_for_line("^done,addr=\"0x00000000" CHS_MTIME_REG);

    if (std::regex_search(line, match, memory_regex)) {
        return std::stoull(match[1], nullptr, 10);
    } else {
        throw std::runtime_error("Failed to parse machine timer response: " + line);
    }
}

void GdbServer::set_mtime(const std::string &value) 
{
    // Set the machine timer register in Cheshire through GDB
    // FIXME: it needs a hex and will set bytes in reverse order
    this->send_command("-data-write-memory-bytes 0x" CHS_MTIME_REG + value);
    std::string response = this->wait_for_line("^done");
    
    if (response.find("^error") != std::string::npos) {
        throw std::runtime_error("Failed to set machine timer: " + response);
    }
}

uint64_t GdbServer::read_var(const std::string &var_name, int base)
{
    // Read the value of a variable from GDB
    std::string line;
    std::regex value_regex("value=\"([^\"]+)\"");
    std::smatch match;

    this->send_command("-data-evaluate-expression \"" + var_name + "\"");
    line = this->wait_for_line("^done,value=");

    if (std::regex_search(line, match, value_regex)) {
        return std::stoull(match[1], nullptr, base);
    } else {
        throw std::runtime_error("Failed to parse variable response: " + line);
    }
}

void GdbServer::write_var(const std::string &var_name, uint64_t value)
{
    // Set the value of a variable in GDB
    this->send_command("set " + var_name + "=" + std::to_string(value));

    std::string response = this->wait_for_line("^done");
    if (response.find("^error") != std::string::npos) {
        throw std::runtime_error("Failed to write variable: " + response);
    }
}

void GdbServer::dump_memory(uint64_t address, uint8_t* data_ptr, size_t size_bytes)
{
    // Dump memory from the target address into the provided buffer
    std::string line;
    std::regex memory_regex("data=\\[\"([^\"]+)\"\\]");
    std::smatch match;
    size_t nread;

    // Dump to the temporary file
    this->flush_output();
    this->send_command("dump memory " CHESSY_TMP_FILE " " + std::to_string(address) + " " + std::to_string(address + size_bytes));
    line = this->wait_for_line("^done");
    DEBUG_PRINT_GDB("Dumped memory from %p to %p (%zu bytes)\n", address, address + size_bytes, size_bytes);
    
    // Read from the temporary file into the buffer
    this->dump_file = freopen(CHESSY_TMP_FILE, "rb", this->dump_file); ///< Refresh the file so we see new data written by GDB
    fseek(this->dump_file, 0, SEEK_SET);
    nread = fread(data_ptr, 1, size_bytes, this->dump_file);
    DEBUG_PRINT_GDB("Read %zu bytes from dump file into buffer\n", nread);

    DEBUG_PRINT_GDB("Data: ");
    for (size_t i = 0; i < nread; i++) {
        DEBUG_PRINT_GDB("%02x ", data_ptr[i]);
    }
    DEBUG_PRINT_GDB("\n");
}

void GdbServer::restore_memory(uint64_t address, uint8_t* data_ptr, size_t size_bytes)
{
    // Write memory to the target address from the provided data buffer
    std::string line;

    // Write to the temporary file
    freopen(CHESSY_TMP_FILE, "w+b", this->dump_file); // clears file each time
    fwrite(data_ptr, 1, size_bytes, this->dump_file);
    fflush(this->dump_file);
    DEBUG_PRINT_GDB("Wrote %zu bytes to dump file\n", size_bytes);

    // Load from the temporary file to the target memory
    this->flush_output();
    this->send_command("restore " CHESSY_TMP_FILE " binary " + std::to_string(address));
    line = this->wait_for_line("^done");
    DEBUG_PRINT_GDB("Restored memory to %p (%zu bytes)\n", address, size_bytes);
}

void GdbServer::flush_output()
{
    // Flush the GDB output stream
    fflush(this->gdb_out);
    // Clear the output buffer
    memset(this->gdb_out_buf, 0, sizeof(this->gdb_out_buf));
}

/* -------------------------------------------------------------------------- */
/*                           AdapterCheshire methods                          */
/* -------------------------------------------------------------------------- */

// AdapterCheshire implementation
AdapterCheshire::AdapterCheshire()
{
    closed   = 0;
    finished = false;
}

/**
 * @brief Gets the power at a given timestamp.
 *
 * This function gets the power at a given timestamp.
 *
 * @param timestamp The timestamp at which to get the power.
 * @return The power at the given timestamp.
 */
double AdapterCheshire::get_power_at(int64_t timestamp)
{
    // Not implemented yet
    return 0;
}

MessyRequest *AdapterCheshire::get_messy_request_from_gdb(const std::string &response)
{
    uint64_t req_addr;
    std::smatch match;
    uint64_t data_addr;
    bool is_read = false;
    unsigned int req_size;

    DEBUG_PRINT_GDB("Parsing GDB response: %s\n", response.c_str());
    // Check that it is the expected function
    if (std::regex_search(response, match, this->func_regex)) {
        if (match[1] != "__chessy_access") {
            if (match[1] == "chessy_close") {
                DEBUG_PRINT_CHESHIRE("Detected chessy_close function, marking adapter as finished.\n");
                this->finished = true;
                return nullptr;
            }
            throw std::runtime_error("Unknown function name in \"*stopped\" line.");
        }
    } else {
        throw std::runtime_error("Function name not found in \"*stopped\" line.");
    }

    // Check if it is a read or write operation
    if (std::regex_search(response, match, this->rw_regex)) {
        // Convert value from string (hex) to bool
        is_read = (bool)std::stoull(match[1], nullptr, 16);
    } else {
        throw std::runtime_error("Read/Write flag not found in \"*stopped\" line.");
    }

    // Find address and convert
    if (std::regex_search(response, match, this->addr_regex)) {
        // Convert address from string (hex) to uint64_t
        req_addr = std::stoull(match[1], nullptr, 16);
    } else {
        throw std::runtime_error("Address not found in \"*stopped\" line.");
    }

    // Find size and convert
    if (std::regex_search(response, match, this->size_regex)) {
        // Convert size from string (decimal) to unsigned int
        req_size = (unsigned int)std::stoull(match[1], nullptr, 10);
    } else {
        throw std::runtime_error("Size not found in \"*stopped\" line.");
    }

    // Prepare the request data buffer
    free(this->req_data_buf); // Free previous buffer if any
    this->req_data_buf = (uint8_t *)malloc(req_size);
    if (!this->req_data_buf) {
        throw std::runtime_error("Failed to allocate memory for request data buffer.");
    }
    memset(this->req_data_buf, 0, req_size); // Clear the buffer

    // Find data pointer and convert
    if (std::regex_search(response, match, this->data_regex)) {
        // Convert data from string (hex) to uint64_t
        data_addr = std::stoull(match[1], nullptr, 16);
    } else {
        throw std::runtime_error("Data pointer not found in \"*stopped\" line.");
    }

    // If it is a write operation, read the data from the target memory
    if (!is_read) {
        // Read the data from the target memory
        this->gdb_server.dump_memory(data_addr, this->req_data_buf, req_size);
    }
    // Reads will be processed later in custom_reply()

    // FIXME: Store the data address in the handler for custom_reply to work, this is a temporary solution
    return new MessyRequest((long long)req_addr, (unsigned int *)this->req_data_buf, is_read, (unsigned int *)data_addr, req_size);
}

void AdapterCheshire::startup(void)
{
    // Start Cheshire
    this->gdb_server.setup();
    DEBUG_PRINT_CHESHIRE("GDB started. PID: %d\n", this->gdb_server.get_pid());
}

void AdapterCheshire::close()
{
    if (!closed) {
        // Close GDB server
        this->gdb_server.close();

        // Free request data buffer
        if (req_data_buf) {
            free(req_data_buf);
            req_data_buf = nullptr;
        }

        closed = 1;
    }
    DEBUG_PRINT_CHESHIRE("Cheshire adapter closed.\n");
    return;
}

uint64_t AdapterCheshire::exec()
{
    std::string request_unparsed;
    MessyRequest *request;

    // Resume execution of the program
    this->gdb_server.send_command("-exec-continue");

    // Wait for a breakpoint (or an error)
    request_unparsed = this->gdb_server.wait_for_line("*stopped,reason=\"signal-received\"");

    // Parse the GDB response to get the MessyRequest
    request = this->get_messy_request_from_gdb(request_unparsed);
    if (this->finished) {
        return 0; // Detect chessy_close and exit
    }
    add_request(request);

    // Read the machine timer again to get the timestamp of the request
    uint64_t req_timestamp_ps = this->gdb_server.read_var("req_timestamp", 10) * 1'000'000; // Convert from us to ps

    if (request->read_req) {
        DEBUG_PRINT_CHESHIRE(
            "Parsed READ request: <-(0x%llx), %u Bytes @ %llu ms\n",
            request->addr,
            request->size,
            req_timestamp_ps / 1'000'000'000
        );
    } else {
        DEBUG_PRINT_CHESHIRE(
            "Parsed WRITE request: %llx ->(0x%llx), %u Bytes @ %llu ms\n",
            *request->data,
            request->addr,
            request->size,
            req_timestamp_ps / 1'000'000'000
        );
    }

    // Jump over the ebreak instruction
    this->gdb_server.send_command("set $pc=$pc+2");

    // Flush GDB output
    this->gdb_server.flush_output();

    // Return the timestamp of the request
    return req_timestamp_ps;
}

/**
 * @brief Custom reply handler for MessyRequest.
 *
 * This function handles custom replies for MessyRequest.
 * It sends the response back to GDB if it was a read request.
 *
 * @param req The MessyRequest to handle.
 */
void AdapterCheshire::custom_reply(MessyRequest *req, uint64_t timestamp_us)
{
    // If it was a read request, we need to send the value back to GDB
    if (req->read_req) {
        DEBUG_PRINT_CHESHIRE("Responding to READ request: <-(0x%llx), %u Bytes\n", req->addr, req->size);
        DEBUG_PRINT_GDB("Data to send back: ");
        for (unsigned int i = 0; i < req->size; i++) {
            DEBUG_PRINT_GDB("%02x ", ((uint8_t*)req->data)[i]);
        }
        DEBUG_PRINT_GDB("\n");
        // Restore the data to the target memory
        this->gdb_server.restore_memory((uint64_t)req->handle_c, this->req_data_buf, req->size);
    }

    // Send back the new timestamp to GDB
    #ifndef PROFILE_CHESSY
    DEBUG_PRINT_CHESHIRE("Sending timestamp back to GDB: %llu ms\n", timestamp_us / 1'000);
    this->gdb_server.write_var("req_timestamp", timestamp_us);
    #else
    // In profiling mode, we do not update the timestamp to let Cheshire use mtime
    #warning "Profiling mode enabled: not updating req_timestamp!"
    #endif
}