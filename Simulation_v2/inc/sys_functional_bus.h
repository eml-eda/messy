#include <systemc.h>
#define NS 6

SC_MODULE(functional_bus){

    //Input Port
    sc_core::sc_in <int>  address_in_M;
    sc_core::sc_in <int>  data_in_M;
    sc_core::sc_in <bool> flag_in_M;
    sc_core::sc_in <bool> Ready_M;
    sc_core::sc_in <int>  data_in_S[NS];
    sc_core::sc_in <bool> Go_S[NS];
    //Output Port
    sc_core::sc_out <int>  data_out_M;
    sc_core::sc_out <bool> Go_M;
    sc_core::sc_out <int>  address_out_S[NS];
    sc_core::sc_out <int>  data_out_S[NS];
    sc_core::sc_out <bool> flag_out_S[NS];
    sc_core::sc_out <bool> Ready_S[NS];

    SC_CTOR(functional_bus):
    address_in_M("Address_from_Master_to_Bus"),
    data_in_M("Data_from_Master_to_Bus"),
    flag_in_M("Flag_from_Master_to_Bus"),
    Ready_M("Ready_from_Master_to_Bus"),
    data_out_M("Data_from_Bus_to_Master"),
    Go_M("Go_from_Bus_to_Master")
    {
        SC_THREAD(processing_data);
        sensitive << Ready_M;
        for (int i = 0; i < NS; i++){
            sensitive << Go_S[i];
        }      
        
    }

    void processing_data();
    void response();
    void Set_Go(bool flag);
    void Set_Slave(int address, bool flag);
    
    private:
        int selected_slave = 0;

functional_bus(){}
};