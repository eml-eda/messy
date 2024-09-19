#ifndef __MESSY_REQUEST__H__
#define __MESSY_REQUEST__H__

#include <vector>

/**
 * @class MessyRequest
 * @brief Represents a request with address, data, read/write flag, handle, and size.
 * 
 * The MessyRequest class encapsulates the details of a request that includes
 * an address, a pointer to the data, a flag indicating whether it is a read 
 * request, a pointer to a handle, and the size of the request. This class is 
 * used to manage and process requests in a system that handles multiple 
 * requests, such as a memory management system or a network request handler.
 */
class MessyRequest {
  public:
    long long int addr; /**< Address of the request */
    unsigned int *data;  /**< Pointer to the data */
    bool read_req;  /**< Flag indicating if it's a read request */
    unsigned int *handle_c; /**< Pointer to the handle */
    unsigned int size; /**< Size of the request */

    /**
     * @brief Constructor for MessyRequest.
     * 
     * This constructor initializes a MessyRequest object with the given 
     * parameters. It sets the address, data pointer, read request flag, 
     * handle pointer, and size of the request.
     * 
     * @param address_t Address of the request.
     * @param data_t Pointer to the data.
     * @param read_req_t Flag indicating if it's a read request.
     * @param handle_c_t Pointer to the handle.
     * @param size_t Size of the request.
     */
    MessyRequest(long long int address_t, unsigned int *data_t, bool read_req_t, unsigned int *handle_c_t, unsigned int size_t) {
        addr = address_t;
        data = data_t;
        read_req = read_req_t;
        handle_c = handle_c_t;
        size = size_t;
    };
};

/** 
 * @brief Vector to store core requests.
 * 
 * This static vector is used to store pointers to MessyRequest objects. It 
 * acts as a queue for managing the requests that need to be processed.
 */
static std::vector<MessyRequest *> core_requests;

/**
 * @brief Adds a request to the core requests.
 * 
 * This function takes a pointer to a MessyRequest object and adds it to the 
 * core_requests vector. The core_requests vector is used to store all the 
 * requests that need to be processed.
 * 
 * @param req Pointer to the MessyRequest to be added.
 */
void add_request(MessyRequest *req);

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
MessyRequest *get_request_at(int i);

/**
 * @brief Deletes a number of requests from the core requests.
 * 
 * This function removes a specified number of requests from the beginning of 
 * the core_requests vector. It is useful for clearing out processed requests 
 * from the queue.
 * 
 * @param size Number of requests to delete.
 */
void delete_n_requests(int size);

/**
 * @brief Gets the size of the request queue.
 * 
 * This function returns the number of requests currently stored in the 
 * core_requests vector. It provides a way to check how many requests are 
 * pending processing.
 * 
 * @return Size of the request queue.
 */
int request_queue_size();

#endif