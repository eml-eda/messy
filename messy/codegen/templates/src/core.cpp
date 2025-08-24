#include <core.hpp>

/**
 * @brief Get the resolution value for a given SystemC time unit.
 *
 * This function returns the resolution value in picoseconds for the specified
 * SystemC time unit. The resolution values are as follows:
 * - SC_PS: 1 picosecond
 * - SC_NS: 1000 picoseconds (1 nanosecond)
 * - SC_US: 1000000 picoseconds (1 microsecond)
 * - SC_MS: 1000000000 picoseconds (1 millisecond)
 * - SC_SEC: 1000000000000 picoseconds (1 second)
 *
 * @param time_unit The SystemC time unit (sc_core::sc_time_unit).
 * @return The resolution value in picoseconds for the given time unit.
 */
int64_t get_resolution_val(sc_core::sc_time_unit time_unit){
    switch (time_unit)
    {
    case sc_core::SC_PS:
        return 1;
    case sc_core::SC_NS:
        return 1000;
    case sc_core::SC_US:
        return 1000000;
    case sc_core::SC_MS:
        return 1000000000;
    case sc_core::SC_SEC:
        return 1000000000000;
    default:
        return 1;
    }
}

/**
 * @brief Executes the core process.
 *
 * This function starts up the ISS adapter and enters a loop that continues
 * processing until the ISS adapter signals it has finished. Once the loop
 * exits, the core process is closed.
 * 
 * @see continue_messy
 * @see close
 */
void Core::run()
{
    iss_adapter->startup();
    
    while(!this->iss_adapter->finished)
        continue_messy(true);

    this->close();
}

/**
 * @brief Executes the next simulation cycle.
 *
 * This function suspends execution until the next simulation timestamp is reached,
 * then updates the current simulation timestamp to match the SystemC time.
 * 
 * @note This function uses SystemC's wait() mechanism to synchronize with the
 * simulation clock.
 */
void Core::run_next_sc(){
    wait(next_timestamp-sc_time_stamp().to_double(),sc_core::SC_PS);
    sc_timestamp=double(sc_time_stamp().to_double());
}

/**
 * @brief Closes the core process and the SystemC simulation.
 *
 * This function waits until the next resolutional value (e.g. next millisecond) and
 * then deletes all remaining requests in the request queue before closing the
 * ISS adapter and stopping the SystemC simulation.
 */
void Core::close(){
    // wait until next resolutional value (next ms)
    wait(get_resolution_val(${resolution})-(next_timestamp%get_resolution_val(${resolution})),sc_core::SC_PS);
    delete_n_requests(request_queue_size());
    this->iss_adapter->close();
    sc_stop();
}

/**
 * @brief Handles the request queue by processing each request.
 * 
 * This function iterates through the request queue, processes each request
 * using the handle_req function, and then deletes the processed requests.
 * 
 * @note The size of the request queue is determined at the start of the function,
 *       and all requests present at that time are processed and deleted.
 */
void Core::handle_req_queue(){
    int core_requests_size = request_queue_size();
    for(int i = 0; i < core_requests_size; i++)   
        handle_req(get_request_at(i));
    delete_n_requests(core_requests_size);
}


/**
 * @brief Advances the simulation by one step.
 *
 * This function performs the following operations:
 * 1. Executes events scheduled by the ISS until the current SystemC timestamp
 * 2. Updates the next timestamp to the next scheduled event
 * 3. Accumulates power consumption statistics
 * 4. Increments the simulation iteration counter
 * 5. Optionally processes the request queue
 * 6. Advances to the next SystemC timestamp
 *
 * @param handle_req_queue If true, processes pending requests in the queue; if false, 
 *                         skips request processing for this simulation step
 * @see handle_req_queue
 * @see run_next_sc
 */
void Core::continue_messy(bool handle_req_queue){

    // Execute events at the current SystemC timestamp and update the next timestamp
    next_timestamp = this->iss_adapter->exec_events_at(sc_timestamp);

    // Accumulate the total power consumed up to the current timestamp
    this->tot_power += this->iss_adapter->get_power_at(sc_timestamp);
    
    // Increment the simulation iteration count
    simulation_iters++;
    
    // Handle the request queue if the flag is set
    if(handle_req_queue)    
        this->handle_req_queue();

    // Run the next simulation cycle
    this->run_next_sc();

}

/**
 * @brief Processes a single request by interacting with the functional bus.
 *
 * This function handles both read and write requests:
 * 
 * For read requests:
 * - Sets the functional bus flag to true to indicate read operation
 * - Signals that a request is ready to be processed
 * - Transfers the request address and size to the bus
 * - Waits for operation completion
 * - Checks for segmentation faults
 * - Copies data from the bus to the request's data buffer
 * 
 * For write requests:
 * - Transfers data from the request to the bus
 * - Sets the functional bus flag to false to indicate write operation
 * - Signals that a request is ready to be processed
 * - Transfers the request address and size to the bus
 * - Waits for operation completion
 * - Checks for segmentation faults
 * 
 * In both cases, the function notifies the ISS adapter after request completion.
 *
 * @param req Pointer to a MessyRequest object containing request parameters and data buffers
 * @throws None, but will terminate simulation if a segmentation fault is detected
 * 
 * @see MessyRequest
 * @see close
 */
void Core::handle_req(MessyRequest *req)
{
   
    if (req->read_req)
    {
        // For the read request the functional bus flag is set to true
        o_is_read.write(true);

        // Set the request ready signal
        o_activate_functional_bus.write(true);
        
        // Write the address and size to the corresponding signals
        o_address.write(req->addr);
        o_size.write(req->size);
        wait();

        // Check for segmentation fault
        if(i_idx_sensor.read() < 0) 
            this->close();

        o_activate_functional_bus.write(false);
        wait();

        // Write the read data into the iss memory
        uint8_t* sensor_memory = (uint8_t*)i_data_ptr.read();
        for(unsigned int i = 0; i < req->size; i++)    
            ((uint8_t*)req->data)[i] = sensor_memory[i];
    }
    else
    {
        o_data_ptr.write(((uint8_t*)req->data));
        
        // For the write request the functional bus flag is set to false
        o_is_read.write(false);
        o_activate_functional_bus.write(true);

        o_address.write(req->addr);
        o_size.write(req->size);
        wait();
        if(i_idx_sensor.read() < 0) 
            this->close();
        o_activate_functional_bus.write(false);
        wait();
        
    }
    iss_adapter->custom_reply(req);   
}


/**
 * @brief Introduces a time delay in the simulation execution.
 *
 * This function calculates a target timestamp by adding a specified time delay 
 * to the given start time, then continuously advances the simulation until either:
 * 1. The target timestamp is reached, or
 * 2. The ISS adapter signals completion
 *
 * @param start_time The reference timestamp in picoseconds from which to calculate the delay
 * @param time_to_skip The number of time units to skip
 * @param resolution The resolution unit for the time_to_skip parameter
 * 
 * @note The actual delay is calculated as time_to_skip * resolution value in picoseconds
 * @see continue_messy
 * @see get_resolution_val
 */
void Core::request_delay(double start_time,int time_to_skip,sc_core::sc_time_unit resolution){
    double time = (time_to_skip*get_resolution_val(resolution))+start_time;
    while(next_timestamp<time && !this->iss_adapter->finished)
        this->continue_messy(false);
}

void Core::grant_req(MessyRequest *req)
{

}

/**
 * @brief Handles a response to a request. It gets called when one of our access gets its response.
 * 
 * @param req Pointer to the MessyRequest that received a response.
 */
void Core::reply_to_req(MessyRequest *req)
{

}