#include <messy_request.hpp>

void add_request(MessyRequest* req) { core_requests.emplace_back(req); }

MessyRequest* get_request_at(int i) { return core_requests[i]; }

void delete_n_requests(int size) { core_requests.erase(core_requests.begin(),core_requests.begin()+size); }

int request_queue_size() { return core_requests.size(); }