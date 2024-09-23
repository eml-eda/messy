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
    sc_core::sc_out<unsigned int> request_address;
    sc_core::sc_out<uint8_t *> request_data;
    sc_core::sc_out<unsigned int> request_size;
    sc_core::sc_out<bool> functional_bus_flag;
    sc_core::sc_out<bool> request_ready;
    sc_core::sc_in<bool> request_go;
    sc_core::sc_in<uint8_t *> request_value;
    sc_core::sc_in<int> idx_sensor;
    // Power Port
    sc_core::sc_out<double> power_signal;

    int simulation_iters = 0;  /**< Number of simulation iterations. It gets incremented every time the continue_messy method is executed */
    double tot_power = 0.0; /**< Total power consumed. It gets accumulated every time the continue_messy is executed, by summing to the total power the instant power returned by gvsoc */

    /**
     * @brief Runs the core functionality.
     */
    void run();

    /**
     * @brief Closes the core functionality and the SystemC simulation.
     */
    void close();

    /**
     * @brief Runs the next SystemC process.
     */
    void run_next_sc();

    /**
     * @brief Continues the messy process.
     * 
     * @param handle_req_queue Flag indicating whether to handle the request queue.
     */
    void continue_messy(bool handle_req_queue);

    /**
     * @brief Handles the request queue.
     */
    void handle_req_queue();

    /**
     * @brief Requests a delay.
     * 
     * @param start_time The start time of the delay.
     * @param time_to_skip The amount of time to skip.
     * @param resolution The resolution of the delay.
     */
    void request_delay(double start_time, int time_to_skip, int resolution);
    
    /**
     * @brief Handles a request.
     * 
     * @param req Pointer to the MessyRequest to be handled.
     */
    void handle_req(MessyRequest *req);

    /**
     * @brief Handles an access request from the gvsoc side. It gets called when an access from the gvsoc side is reaching us.
     * 
     * @param req Pointer to the MessyRequest to be accessed.
     */
    void access_request(MessyRequest *req);

    /**
     * @brief Handles a granted request. It gets called when one of our access gets granted.
     * 
     * @param req Pointer to the MessyRequest that was granted.
     */
    void grant_req(MessyRequest *req);
    

    /**
     * @brief Handles a response to a request. It gets called when one of our access gets its response.
     * 
     * @param req Pointer to the MessyRequest that received a response.
     */
    void reply_to_req(MessyRequest *req);

    /**
     * @brief Constructor for Core.
     * 
     * Initializes the input and output ports with their respective names.
     */
    SC_CTOR(Core) : request_address("Address_From_Core_to_Func_Bus"),
                    request_data("Data_From_Core_to_Func_Bus"),
                    functional_bus_flag("Flag_From_Core_to_Func_Bus"),
                    request_ready("Master_Ready_to_Func_Bus"),
                    request_go("Master_GO_to_Func_Bus"),
                    request_value("Data_form_Bus_to_Master"),
                    power_signal("Func_to_Power_signal"),
                    idx_sensor("selected_sensor_of_request") {
        iss_adapter = (${adapter_class}*) new ${adapter_class}();
        SC_THREAD(run);
        sensitive << request_go;
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