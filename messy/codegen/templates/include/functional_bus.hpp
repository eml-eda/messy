#include <systemc.h>
#include <config.hpp>

SC_MODULE(Functional_bus){

    //Input Port
    sc_core::sc_in <unsigned int>  request_address;
    sc_core::sc_in <uint8_t*>  request_data;
    sc_core::sc_in <bool> flag_from_core;
    sc_core::sc_in <bool> request_ready;
    sc_core::sc_in <unsigned int> request_size;
    sc_core::sc_in <uint8_t*>  data_input_sensor[NUM_SENSORS];
    sc_core::sc_in <bool> go_sensors[NUM_SENSORS];
    //Output Port
    sc_core::sc_out <uint8_t*>  request_value;
    sc_core::sc_out <bool> request_go;
    sc_core::sc_out <int> idx_sensor;
    sc_core::sc_out <unsigned int>  address_out_sensor[NUM_SENSORS];
    sc_core::sc_out <uint8_t*>  data_out_sensor[NUM_SENSORS];
    sc_core::sc_out <unsigned int>  size_out_sensor[NUM_SENSORS];
    sc_core::sc_out <bool> flag_out_sensor[NUM_SENSORS];
    sc_core::sc_out <bool> ready_sensor[NUM_SENSORS];

    SC_CTOR(Functional_bus):
    request_address("Address_from_Master_to_Bus"),
    request_data("Data_from_Master_to_Bus"),
    flag_from_core("Flag_from_Master_to_Bus"),
    request_ready("Ready_from_Master_to_Bus"),
    request_value("Data_from_Bus_to_Master"),
    request_go("Go_from_Bus_to_Master"),
    idx_sensor("selected_sensor_from_request")
    {
        SC_THREAD(processing_data);
        sensitive << request_ready;
        for (int i = 0; i < NUM_SENSORS; i++){
            sensitive << go_sensors[i];
        }      
        
    }

    void processing_data();
    void response();
    
    private:
        int selected_sensor = 0;

Functional_bus(){}
};