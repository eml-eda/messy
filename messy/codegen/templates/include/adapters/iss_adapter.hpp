#ifndef __ISS_ADAPTER_H
#define __ISS_ADAPTER_H

#include <messy_request.hpp>
#include <stdint.h>
class ISS_Adapter {
    public:
    virtual int64_t exec_event_at(int64_t timestamp);
    virtual void close();
    virtual ~ISS_Adapter() {};
    virtual void startup();
    virtual void custom_reply(MessyRequest* req);
    bool finished=false;
};

#endif