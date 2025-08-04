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

#define GDB_BUFFER 1024
#define GDB_PORT "3333"
#define GDB_DEF_LONG_TIMEOUT_MS 2000
#define GDB_DEF_SHORT_TIMEOUT_MS 100
#define CHESSY_TEST_BIN "/tests/fake_sensor_multiple_reads.spm.elf" // TODO: make this configurable

// Machine timer register, contains the current time in microseconds. 
// Avoid the 0x prefix!
#define CHS_MTIME_REG "0204bff8"

#define DEBUG
// #define DEBUG_GDB

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
    
    bool is_closed() const { return closed; }
    pid_t get_pid() const { return gdb_pid; }
    
  private:
    bool closed;
    pid_t gdb_pid;
    FILE *gdb_in, *gdb_out;
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
    void custom_reply(MessyRequest *req);
    
    MessyRequest *get_messy_request_from_gdb(const std::string &response);

    bool finished;

  private:
    int closed;
    GdbServer gdb_server;
    uint8_t *req_data_buf; // Buffer to store request data
};

#endif