#ifndef __MESSY_REQUEST__H__
#define __MESSY_REQUEST__H__

#include <vector>

class MessyRequest{
    public:
    long long int addr;
    unsigned int* data;
    bool read_req;
    unsigned int* handle_c;
    MessyRequest(long long int address_t,unsigned int* data_t,bool read_req_t,unsigned int* handle_c_t){
        addr=address_t;
        data=data_t;
        read_req=read_req_t;
        handle_c=handle_c_t;
    };
};

static std::vector<MessyRequest*> core_requests;

void add_request(MessyRequest* req);

MessyRequest* get_request_at(int i);

void delete_n_requests(int size);

int request_queue_size();

#endif