#include <messy_request.hpp>

/**
 * @brief Adds a request to the core requests.
 * 
 * This function takes a pointer to a MessyRequest object and adds it to the 
 * core_requests vector. The core_requests vector is used to store all the 
 * requests that need to be processed.
 * 
 * @param req Pointer to the MessyRequest to be added.
 */
void add_request(MessyRequest *req) { core_requests.emplace_back(req); }

/**
 * @brief Gets the request at a specific index.
 * 
 * This function returns a pointer to the MessyRequest object at the specified 
 * index in the core_requests vector. It allows access to individual requests 
 * based on their position in the vector.
 * 
 * @param i Index of the request.
 * @return Pointer to the MessyRequest at the specified index.
 */
MessyRequest *get_request_at(int i) { return core_requests[i]; }

/**
 * @brief Deletes a number of requests from the core requests.
 * 
 * This function removes a specified number of requests from the beginning of 
 * the core_requests vector. It is useful for clearing out processed requests 
 * from the queue.
 * 
 * @param size Number of requests to delete.
 */
void delete_n_requests(int size) { core_requests.erase(core_requests.begin(), core_requests.begin() + size); }

/**
 * @brief Gets the size of the request queue.
 * 
 * This function returns the number of requests currently stored in the 
 * core_requests vector. It provides a way to check how many requests are 
 * pending processing.
 * 
 * @return Size of the request queue.
 */
int request_queue_size() { return core_requests.size(); }