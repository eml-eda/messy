#ifndef __ADAPTER_CHESHIRE_H
#define __ADAPTER_CHESHIRE_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>

#include <config.hpp>
#include <string.h>
#include <messy_request.hpp>

// GDB
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
#define CHESSY_TEST_BIN "/tests/fake_sensor.spm.elf"

#define CORE_FREQ 50000000      // 50 MHz
#define CHS_CC_REG "0x0204bff8" // Cycle counter register

#define DEBUG
// #define DEBUG_GDB

class AdapterCheshire
{
  public:
    void close();
    void startup();
    void exec();
    double get_power_at(int64_t timestamp);
    AdapterCheshire();
    ~AdapterCheshire()
    {
        close();
    }
    void custom_reply(MessyRequest *req);

    void setup_gdb();
    void send_gdb_command(const std::string cmd);
    MessyRequest *get_messy_request_from_gdb(const std::string &response);
    std::string wait_for_gdb_line(const std::string &keyword = "*stopped,reason=\"signal-received\"");
    uint64_t read_cycle_counter();
    void flush_gdb_output();

    bool finished;

  private:
    // TODO: have a GdbServer class that handles all GDB interactions
    int closed;
    pid_t gdb_pid;
    FILE *gdb_in, *gdb_out;
    int gdb_out_fd[2];
    int gdb_in_fd[2];
    char gdb_out_buf[GDB_BUFFER];

    uint8_t *req_data_buf; // Buffer to store request data
};

#endif