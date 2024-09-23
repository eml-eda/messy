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
 * This function waits until the next simulation timestamp is reached and then
 * updates the current simulation timestamp.
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
    // wait until next resolutional value(next ms)
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
 * @brief Continues the simulation process for the Core.
 *
 * This function advances the simulation by executing events, updating power consumption,
 * and handling the request queue if specified. It also increments the simulation iteration count.
 *
 * @param handle_req_queue A boolean flag indicating whether to handle the request queue.
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
 * @brief Handles a request by connecting signals with the SystemC simulator signals.
 *
 * This function processes a request by either reading from or writing to the functional bus.
 * It sets the appropriate signals, waits for the operation to complete, and then handles the 
 * data transfer between the request and the internal memory.
 *
 * @param req Pointer to a MessyRequest object containing the request details.
 *
 * The function performs the following steps:
 * - If the request is a read request:
 *   - Sets the functional bus flag and request ready signals.
 *   - Writes the request address and size to the corresponding signals.
 *   - Waits for the operation to complete.
 *   - Checks for segmentation fault by reading the idx_sensor signal.
 *   - If idx_sensor is negative, closes the core.
 *   - Reads the data from the request value signal into the request data.
 * - If the request is a write request:
 *   - Writes the request data to the request data signal.
 *   - Clears the functional bus flag and sets the request ready signal.
 *   - Writes the request address and size to the corresponding signals.
 *   - Waits for the operation to complete.
 *   - Checks for segmentation fault by reading the idx_sensor signal.
 *   - If idx_sensor is negative, closes the core.
 * - Calls the custom_reply method of the iss_adapter to send a reply for the request.
 */
void Core::handle_req(MessyRequest *req)
{
   
    if (req->read_req)
    {
        // For the read request the functional bus flag is set to true
        functional_bus_flag.write(true);

        // Set the request ready signal
        request_ready.write(true);
        
        // Write the address and size to the corresponding signals
        request_address.write(req->addr);
        request_size.write(req->size);
        wait();

        // Check for segmentation fault
        if(idx_sensor.read() < 0) 
            this->close();

        request_ready.write(false);
        wait();

        // Write the read data into the iss memory
        uint8_t* sensor_memory = (uint8_t*)request_value.read();
        for(unsigned int i = 0; i < req->size; i++)    
            ((uint8_t*)req->data)[i] = sensor_memory[i];
    }
    else
    {
        request_data.write(((uint8_t*)req->data));
        
        // For the write request the functional bus flag is set to false
        functional_bus_flag.write(false);
        request_ready.write(true);

        request_address.write(req->addr);
        request_size.write(req->size);
        wait();
        if(idx_sensor.read() < 0) 
            this->close();
        request_ready.write(false);
        wait();
        
    }
    iss_adapter->custom_reply(req);   
}


void Core::request_delay(double start_time,int time_to_skip,int resolution){
    double time = (time_to_skip*get_resolution_val(${resolution}))+start_time;
    while(next_timestamp<time && !this->iss_adapter->finished)
        this->continue_messy(false);
}

void Core::grant_req(MessyRequest *req)
{

}

void Core::reply_to_req(MessyRequest *req)
{

}