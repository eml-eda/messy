#ifndef __ADAPTER_GVSOC_H
#define __ADAPTER_GVSOC_H

#include <config.hpp>
#include <gv/gvsoc.hpp>
// vtable issues? how to build abstract class?
// #include <adapters/iss_adapter.hpp>
#include <cstring>
#include <messy_request.hpp>
#include <string.h>
#include <vp/itf/io.hpp>
#include <vp/launcher.hpp>
#define LOG_REQUESTS 1
#ifdef LOG_REQUESTS
#include <fstream>
#endif

class AdapterGvsocGap9 : public gv::Io_user, public vp::Notifier {
  public:
    int64_t exec_events_at(int64_t timestamp);
    double get_power_at(int64_t timestamp);
    void close();
    void startup();
    void custom_reply(MessyRequest *req);
    void access(gv::Io_request *req);
    void grant(gv::Io_request *req);
    void reply(gv::Io_request *req);
    void notify_stop(int64_t time);
    void notify_run(int64_t time);
    MessyRequest *get_messy_request_from_gvsoc(gv::Io_request *req);
    AdapterGvsocGap9();
    //~AdapterGvsocGap9() {};
    bool finished = false;

  private:
    int closed = 0;
    gv::Io_binding *axi;
    Gvsoc_launcher *gvsoc;
#ifdef LOG_REQUESTS
    std::ofstream request_addresses;
    std::ofstream request_sizes;
#endif
};

#endif