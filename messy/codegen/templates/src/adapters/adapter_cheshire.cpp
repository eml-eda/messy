#include <adapters/adapter_cheshire.hpp>

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
#ifdef DEBUG_GDB
            printf("[GDB]: %s", this->gdb_out_buf);
#endif
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
#ifdef DEBUG_GDB
    printf("GDB command sent: %s\n", cmd.c_str());
#endif
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
    // Regex patterns
    std::regex func_regex("func=\"([^\"]+)\"");
    std::regex address_regex("name=\"sensor_address\",value=\"([^\"]+)\"");
    std::regex data_regex("name=\"sensor_data\",value=\"([^\"]+)\"");

    std::smatch match;
    uint64_t addr;
    bool is_read;
    unsigned int size = 8; // 64bit TODO: Adjust size based on the actual data type

    // Prepare the request data buffer
    this->req_data_buf = (uint8_t *)malloc(size);
    if (!this->req_data_buf) {
        throw std::runtime_error("Failed to allocate memory for request data buffer.");
    }
    memset(this->req_data_buf, 0, size); // Clear the buffer

#ifdef DEBUG_GDB
    printf("Parsing GDB response: %s\n", response.c_str());
#endif
    // Find function call name
    if (std::regex_search(response, match, func_regex)) {
        if (match[1] == "write_sensor") {
            is_read = false; // This is a write operation
        } else if (match[1] == "read_sensor") {
            is_read = true; // This is a read operation
        } else {
            throw std::runtime_error("Unknown function name in \"*stopped\" line.");
        }
    } else {
        throw std::runtime_error("Function name not found in \"*stopped\" line.");
    }

    // Find address and convert
    if (std::regex_search(response, match, address_regex)) {
        // Convert address from string (hex) to uint64_t
        addr = (uint64_t)std::stoull(match[1], nullptr, 16);
    } else {
        throw std::runtime_error("Address not found in \"*stopped\" line.");
    }

    // Find data only if it is a write operation
    if (!is_read) {
        if (std::regex_search(response, match, data_regex)) {
            // Convert data from string (base 10) to uint64_t
            uint64_t data_value = std::stoull(match[1], nullptr, 10);
            // Store data in the buffer
            memcpy(this->req_data_buf, &data_value, sizeof(data_value));
        } else {
            throw std::runtime_error("Data not found in \"*stopped\" line for write operation.");
        }
    }

    return new MessyRequest((long long)addr, (unsigned int *)this->req_data_buf, is_read, nullptr, size);
}

void AdapterCheshire::startup(void)
{
    // Start Cheshire
    this->gdb_server.setup();
    printf("GDB started. PID: %d\n", this->gdb_server.get_pid());
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
    printf("Cheshire adapter closed.\n");
    return;
}

uint64_t AdapterCheshire::exec()
{
    std::string request_unparsed;
    MessyRequest *request;

    // Read start time from the machine timer
    uint64_t start_timestamp_us = this->gdb_server.read_mtime();

    // Resume execution of the program
    this->gdb_server.send_command("-exec-continue");

    // Wait for a breakpoint (or an error)
    request_unparsed = this->gdb_server.wait_for_line("*stopped,reason=\"signal-received\"");

    // Read the machine timer again to get the timestamp of the request
    // TODO: This currently includes the overhead of the GDB communication.
    //  Although the overhead **should** be negligible, this could create skewed results in long simulations.
    uint64_t end_timestamp_us = this->gdb_server.read_mtime();

    // Calculate the delay of the computation in picoseconds
    uint64_t delay_ps = (end_timestamp_us - start_timestamp_us) * 1'000'000;

    // Parse the GDB response to get the MessyRequest
    request = this->get_messy_request_from_gdb(request_unparsed);
    add_request(request);

#ifdef DEBUG
    if (request->read_req) {
        printf("Parsed READ request: <-(0x%llx), %u Bytes\n", request->addr, request->size);
    } else {
        printf("Parsed WRITE request: %u ->(0x%llx), %u Bytes\n", *request->data, request->addr, request->size);
    }
#endif

    // Jump over the ebreak instruction
    this->gdb_server.send_command("set $pc=$pc+2");

    // Flush GDB output
    this->gdb_server.flush_output();

    // Return the computation delay in picoseconds
    return delay_ps;
}

/**
 * @brief Custom reply handler for MessyRequest.
 *
 * This function handles custom replies for MessyRequest.
 * It sends the response back to GDB if it was a read request.
 *
 * @param req The MessyRequest to handle.
 */
void AdapterCheshire::custom_reply(MessyRequest *req)
{
    // If it was a read request, we need to send the value back to GDB
    if (req->read_req) {
#ifdef DEBUG
        printf("Responding to READ request: %u <-(0x%llx), %u Bytes\n", *req->data, req->addr, req->size);
#endif
        this->gdb_server.send_command("set sensor_data=" + std::to_string(*req->data));
    }
}