#ifndef __ADAPTER_CHESHIRE_H
#define __ADAPTER_CHESHIRE_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <config.hpp>
#include <string.h>
#include <messy_request.hpp>
// GDB-related includes
#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <vector>
#include <regex>

//
// Cheshire-related definitions
//
#define GDB_BUFFER 2048 ///< Size of the buffer to read GDB output
#define GDB_PORT "3334" ///< Port where GDB server will listen on the board
#define CHESSY_TEST_BIN "/messy/messy/bin/chessy_app" ///< Application binaries copied when running make application
#define CHS_MTIME_REG "0204bff8" ///< Machine timer register, contains the current time in microseconds. Avoid the 0x prefix!
#define CHESSY_TMP_FILE "/messy/messy/chessy_req_data.bin" ///< Temporary file to store request data

//
// Enable debug prints ?
//
#define DEBUG_CHESHIRE
//#define DEBUG_GDB

#ifdef DEBUG_CHESHIRE
  #define DEBUG_PRINT_CHESHIRE(...) printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT_CHESHIRE(...)
#endif

#ifdef DEBUG_GDB
  #define DEBUG_PRINT_GDB(...) printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT_GDB(...)
#endif

class GdbServer
{
  public:
    GdbServer();
    ~GdbServer();
    
    void setup();
    void close();
    void send_command(const std::string &cmd);
    std::string wait_for_line(const std::string &keyword);
    void flush_output();
    uint64_t read_mtime();
    void set_mtime(const std::string &value);
    uint64_t read_var(const std::string &var_name, int base = 10);
    void write_var(const std::string &var_name, uint64_t value);
    void dump_memory(uint64_t address, uint8_t* data_ptr, size_t size_bytes);
    void restore_memory(uint64_t address, uint8_t* data_ptr, size_t size_bytes);

    bool is_closed() const { return closed; }
    pid_t get_pid() const { return gdb_pid; }
    
  private:
    bool closed;
    pid_t gdb_pid;
    FILE *gdb_in, *gdb_out;
    FILE *dump_file;
    int gdb_out_fd[2];
    int gdb_in_fd[2];
    char gdb_out_buf[GDB_BUFFER];
};

class AdapterCheshire
{
  public:
    AdapterCheshire();
    ~AdapterCheshire()
    {
        close();
    }
    void close();
    void startup(); 
    uint64_t exec();
    double get_power_at(int64_t timestamp);
    void custom_reply(MessyRequest *req, uint64_t timestamp_us);
    
    MessyRequest *get_messy_request_from_gdb(const std::string &response);

    bool finished;

  private:
    int closed;
    GdbServer gdb_server;
    uint8_t *req_data_buf; // Buffer to store request data

    // Regex patterns
    std::regex func_regex = std::regex("func=\"([^\"]+)\"");
    std::regex addr_regex = std::regex("name=\"req_addr\",value=\"([^\"]+)\"");
    std::regex data_regex = std::regex("name=\"req_data\",value=\"([^\"]+)\"");
    std::regex size_regex = std::regex("name=\"req_size\",value=\"([^\"]+)\"");
    std::regex rw_regex = std::regex("name=\"req_is_read\",value=\"([^\"]+)\"");
};

#endif