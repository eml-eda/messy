#include <systemc.h>
#include "core.h"
#include "sys_functional_bus.h"
#include "temperature_sensor_functional.h"
#include "air_quality_sensor_functional.h"
#include "air_quality_sensor_power.h"


int sc_main(int argc, char* argv[])
{
    Core Master("Master");
    functional_bus Func("Func");
    temperature_sensor_functional Temp("T_Sensor");
    air_quality_sensor_functional Air("Air_Sensor");
    air_quality_sensor_power Air_P("Air_Power");

    sc_signal <bool> Enable_Temp;
    //Data from Master to Bus
    sc_signal <int>  A_M_to_B;
    sc_signal <int>  D_M_to_B;
    sc_signal <bool> F_M_to_B;
    //Data from Bus to Master
    sc_signal <int>  D_B_to_M;
    //Data from Bus to Slave
    sc_signal <int>  A_B_to_S[NS];
    sc_signal <int>  D_B_to_S[NS];
    sc_signal <bool>  F_B_to_S[NS];
    //Data from Slave to Bus
    sc_signal <int> Fake;
    Func.data_in_S[0](Fake);

    //Air Quality Data
    sc_signal <int> Air_Data;
    //Air Quality Power
    sc_signal <int> Air_F_to_P;
    //Temperature Data
    sc_signal <int>  Temp_Data;

    //Binding Master's signals
    Master.A_Out(A_M_to_B);
    Master.D_Out(D_M_to_B);
    Master.F_Out(F_M_to_B);
    Master.Data_in(D_B_to_M);
    //Binding Bus's Input Address 
    Func.address_in_M(A_M_to_B);
    Func.data_in_M(D_M_to_B);
    Func.flag_in_M(F_M_to_B);
    //Binding Slave Input to Bus
    //Air Quality Sensor
    Func.data_in_S[1](Air_Data);
    //Temperature Sensor
    Func.data_in_S[2](Temp_Data);

    //Binding Bus's Output Address
    Func.data_out_M(D_B_to_M);
    for (size_t i = 0; i < NS; i++) {
        Func.address_out_S[i](A_B_to_S[i]);
        Func.data_out_S[i](D_B_to_S[i]);
        Func.flag_out_S[i](F_B_to_S[i]);
    }
    //Binding Air Quality Sensor's signals
    Air.enable(Enable_Temp);
    Air.address(A_B_to_S[1]);
    Air.data_in(D_B_to_S[1]);
    Air.flag_wr(F_B_to_S[1]);
    Air.data_out(Air_Data);
    Air.power_signal(Air_F_to_P);
    //Binding Air Quality Power signals
    Air_P.func_signal(Air_F_to_P);
    //Binding Temperature Sensor's signals
    Temp.enable(Enable_Temp);
    Temp.address(A_B_to_S[2]);
    Temp.data_in(D_B_to_S[2]);
    Temp.flag_wr(F_B_to_S[2]);
    Temp.data_out(Temp_Data);

    Enable_Temp.write(true);
    std::cout <<"The simulation starts!"<<endl;

    sc_start(10, sc_core::SC_SEC); // Set the simulation length

    std::cout <<"The simulation ends @ "<<sc_time_stamp()<<endl;

    return 0;
}