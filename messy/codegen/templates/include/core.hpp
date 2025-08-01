#ifndef __MESSY_CORE_H__
#define __MESSY_CORE_H__
#include <adapters/${adapter_filenames}.hpp>
#include <config.hpp>
#include <cstring>
#include <messy_request.hpp>
#include <string.h>
#include <systemc.h>

class Core : public sc_module {
  public:
    // Input Ports
    sc_core::sc_in<bool> i_done_functional_bus; /**< Flag indicating if the request can proceed */
    sc_core::sc_in<uint8_t *> i_data_ptr; /**< Value of the request */
    sc_core::sc_in<int> i_idx_sensor; /**< Index of the sensor */

    // Output Ports
    sc_core::sc_out<unsigned int> o_address; /**< Address of the request */
    sc_core::sc_out<uint8_t *> o_data_ptr; /**< Data of the request */
    sc_core::sc_out<unsigned int> o_size; /**< Size of the request */
    sc_core::sc_out<bool> o_is_read; /**< Flag for the functional bus */
    sc_core::sc_out<bool> o_activate_functional_bus; /**< Flag indicating if the request is ready */


    // Power Port
    sc_core::sc_out<double> o_power_state; /**< Power signal output */

    // Variables
    int simulation_iters = 0;  /**< Number of simulation iterations. It gets incremented every time the continue_messy method is executed */
    double tot_power = 0.0; /**< Total power consumed. It gets accumulated every time the continue_messy is executed, by summing to the total power the instant power returned by gvsoc */

    /**
     * @brief Executes the core functionality.
     *
     * This function starts up the ISS adapter and enters a loop that continues
     * processing until the ISS adapter signals it has finished. Once the loop
     * exits, the core process is closed.
     *
     * @see continue_messy
     * @see close
     */
    void run();

    /**
     * @brief Closes the core functionality and the SystemC simulation.
     *
     * This function waits until the next resolutional value (e.g. next millisecond) and
     * then deletes all remaining requests in the request queue before closing the
     * ISS adapter and stopping the SystemC simulation.
     */
    void close();

    /**
     * @brief Runs the next SystemC process.
     *
     * This function suspends execution until the next simulation timestamp is reached,
     * then updates the current simulation timestamp to match the SystemC time.
     *
     * @note This function uses SystemC's wait() mechanism to synchronize with the
     * simulation clock.
     */
    void run_next_sc();

    /**
     * @brief Continues the messy process.
     *
     * This function advances the simulation by one step. It executes events scheduled by the ISS,
     * updates the current timestamp, accumulates power consumption, increments the simulation
     * iteration counter, and optionally processes the request queue.
     *
     * @param handle_req_queue If true, processes pending requests in the queue; if false,
     *                         skips request processing for this simulation step.
     * @see handle_req_queue
     * @see run_next_sc
     */
    void continue_messy(bool handle_req_queue);

    /**
     * @brief Handles the request queue.
     *
     * This function iterates through the request queue, processes each request
     * using the handle_req function, and then deletes the processed requests.
     *
     * @note The size of the request queue is determined at the start of the function,
     *       and all requests present at that time are processed and deleted.
     */
    void handle_req_queue();

    /**
     * @brief Requests a delay.
     *
     * This function calculates a target timestamp by adding a specified time delay
     * to the given start time, then continuously advances the simulation until either:
     * 1. The target timestamp is reached, or
     * 2. The ISS adapter signals completion.
     *
     * @param start_time The reference timestamp in picoseconds from which to calculate the delay.
     * @param time_to_skip The number of time units to skip.
     * @param resolution The resolution unit for the time_to_skip parameter.
     *
     * @note The actual delay is calculated as time_to_skip * resolution value in picoseconds.
     * @see continue_messy
     * @see get_resolution_val
     */
    void request_delay(double start_time, int time_to_skip, sc_core::sc_time_unit resolution);
    
    /**
     * @brief Handles a request.
     *
     * This function processes a single request by interacting with the functional bus.
     * It handles both read and write operations, transfers data, and checks for segmentation faults.
     *
     * @param req Pointer to a MessyRequest object containing request parameters and data buffers.
     * @throws None, but will terminate simulation if a segmentation fault is detected.
     *
     * @see MessyRequest
     * @see close
     */
    void handle_req(MessyRequest *req);

    /**
     * @brief Handles an access request from the gvsoc side.
     *
     * This function is called when an access from the gvsoc side is reaching the adapter.
     *
     * @param req Pointer to the MessyRequest to be accessed.
     */
    void access_request(MessyRequest *req);

    /**
     * @brief Handles a granted request.
     *
     * This function is called when one of our access requests gets granted.
     *
     * @param req Pointer to the MessyRequest that was granted.
     */
    void grant_req(MessyRequest *req);
    

    /**
     * @brief Handles a response to a request.
     *
     * This function is called when one of our access requests gets its response.
     *
     * @param req Pointer to the MessyRequest that received a response.
     */
    void reply_to_req(MessyRequest *req);

    /**
     * @brief Constructor for Core.
     * 
     * Initializes the input and output ports with their respective names.
     */
    SC_CTOR(Core) {
        iss_adapter = (${adapter_class}*) new ${adapter_class}();
        SC_THREAD(run);
        sensitive << i_done_functional_bus;
    }

    ~Core() {
        delete iss_adapter;
    }

  private:
    int64_t next_timestamp = 0; /**< The next timestamp for scheduling events in the simulation */
    int64_t sc_timestamp = 0; /**< The current SystemC timestamp */
    ${adapter_class} *iss_adapter; /**< Pointer to the ISS (Instruction Set Simulator) adapter */
};
#endif