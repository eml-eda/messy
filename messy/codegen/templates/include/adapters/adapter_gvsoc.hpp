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
#define LOG_REQUESTS 1
#ifdef LOG_REQUESTS
#include <fstream>
#endif

class AdapterGvsoc : public gv::Io_user, public gv::Gvsoc_user{
    public:
    int64_t exec_events_at(int64_t timestamp);
    double get_power_at(int64_t timestamp);
    void close();
    void startup();
    void custom_reply(MessyRequest* req);
    void access(gv::Io_request* req);
    void grant(gv::Io_request *req);
    void reply(gv::Io_request *req);
    /**
     * Called by GVSOC to notify the simulation has ended.
     *
     * This means the simulated software is over and probably exited and GVSOC cannnot further
     * simulate it.
     */
    void has_ended() override;

    /**
     * Called by GVSOC to notify the simulation has stopped.
     *
     * This means the an event occurs which stopped the simulation. Simulation can be still be
     * resumed.
     */
    void has_stopped() override;

    /**
     * Called by GVSOC to notify the simulation engine was updated.
     *
     * This means a new event was posted to the engine and modified the timestamp of the next
     * event to be executed.
     */
    void was_updated() override;
    MessyRequest* get_messy_request_from_gvsoc(gv::Io_request* req);
    AdapterGvsoc();
    //~AdapterGvsoc() {};
    bool finished=false;
    private:
    int closed=0;
    gv::Io_binding *axi;
    gv::GvsocLauncher *gvsoc;
    gv::PowerReport* power_report;
    #ifdef LOG_REQUESTS
    std::ofstream request_addresses;
    std::ofstream request_sizes;
    #endif
};

#endif