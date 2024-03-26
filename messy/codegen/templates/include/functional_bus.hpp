#include <systemc.h>
#include <config.hpp>

SC_MODULE(Functional_bus){

    //Input Port
    sc_core::sc_in <int>  request_address;
    sc_core::sc_in <int>  request_data;
    sc_core::sc_in <bool> flag_from_core;
    sc_core::sc_in <bool> request_ready;
    sc_core::sc_in <int>  data_input_sensor[NUM_SENSORS];
    sc_core::sc_in <bool> go_sensors[NUM_SENSORS];
    //Output Port
    sc_core::sc_out <int>  request_value;
    sc_core::sc_out <bool> request_go;
    sc_core::sc_out <int>  address_out_sensor[NUM_SENSORS];
    sc_core::sc_out <int>  data_out_sensor[NUM_SENSORS];
    sc_core::sc_out <bool> flag_out_sensor[NUM_SENSORS];
    sc_core::sc_out <bool> ready_sensor[NUM_SENSORS];

    SC_CTOR(Functional_bus):
    request_address("Address_from_Master_to_Bus"),
    request_data("Data_from_Master_to_Bus"),
    flag_from_core("Flag_from_Master_to_Bus"),
    request_ready("Ready_from_Master_to_Bus"),
    request_value("Data_from_Bus_to_Master"),
    request_go("Go_from_Bus_to_Master")
    {
        SC_THREAD(processing_data);
        sensitive << request_ready;
        for (int i = 0; i < NUM_SENSORS; i++){
            sensitive << go_sensors[i];
        }      
        
    }

    void processing_data();
    void response();
    void Set_Go(bool flag);
    void Set_Slave(int address, bool flag);
    
    private:
        int selected_slave = 0;

Functional_bus(){}
};