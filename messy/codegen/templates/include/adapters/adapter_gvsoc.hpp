#ifndef __ADAPTER_GVSOC_H
#define __ADAPTER_GVSOC_H

#include <config.hpp>
#include <gv/gvsoc.hpp>
// vtable issues? how to build abstract class?
//#include <adapters/iss_adapter.hpp>
#include <cstring>
#include <string.h>
#include <vp/launcher.hpp>
#include <vp/itf/io.hpp>
#include <messy_request.hpp>

class AdapterGvsoc : public gv::Io_user{
    public:
    int64_t exec_events_at(int64_t timestamp);
    double get_power_at(int64_t timestamp);
    void close();
    void startup();
    void custom_reply(MessyRequest* req);
    void access(gv::Io_request* req);
    void grant(gv::Io_request *req);
    void reply(gv::Io_request *req);
    MessyRequest* get_messy_request_from_gvsoc(gv::Io_request* req);
    AdapterGvsoc();
    //~AdapterGvsoc() {};
    bool finished=false;
    private:
    int closed=0;
    gv::Io_binding *axi;
    gv::GvsocLauncher *gvsoc;
};

#endif