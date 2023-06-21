#include <systemc.h>
#define NS 3

SC_MODULE(functional_bus){

    //Input Port
    sc_core::sc_in <int>  address_in_M;
    sc_core::sc_in <int>  data_in_M;
    sc_core::sc_in <bool> flag_in_M;
    sc_core::sc_in <int>  data_in_S[NS];
    //Output Port
    sc_core::sc_out <int>  data_out_M;
    sc_core::sc_out <int>  address_out_S[NS];
    sc_core::sc_out <int>  data_out_S[NS];
    sc_core::sc_out <bool> flag_out_S[NS];

    SC_CTOR(functional_bus):
    address_in_M("Address_from_Master_to_Bus"),
    data_in_M("Data_from_Master_to_Bus"),
    flag_in_M("Flag_from_Master_to_Bus"),
    data_out_M("Data_from_Bus_to_Master")
    {
        SC_THREAD(processing_data);
        sensitive << address_in_M
                  << data_in_M
                  << flag_in_M;

        SC_METHOD(response);
        for (int i = 0; i < NS; i++){
            sensitive << data_in_S[i];
        }    
        
    }

    void processing_data();
    void response();
    
    private:
        int selected_slave = 0;

functional_bus(){}
};