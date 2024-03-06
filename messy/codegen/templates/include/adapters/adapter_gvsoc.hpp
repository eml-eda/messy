#include "config.h"
#include <gv/gvsoc.hpp>
#include <adapters/iss_adapter.hpp>
#include <cstring>
#include <string.h>
#include <vp/launcher.hpp>
#include <vp/itf/io.hpp>

class AdapterGvsoc : public adapters::ISS_Adapter,public gv::Io_user, public vp::Notifier{
    public:
    virtual int64_t exec_event_at(int64_t timestamp);
    virtual void close();
    void access(gv::Io_request* req);
    void custom_reply_axi(gv::Io_request *req);
    void grant(gv::Io_request *req);
    void reply(gv::Io_request *req);
    void notify_stop(int64_t time);
    void notify_run(int64_t time);
    virtual AdapterGvsoc();
    virtual ~AdapterGvsoc() {};
    private:
    int closed=0;
    gv::Io_binding *axi;
    Gvsoc_launcher *gvsoc;
}