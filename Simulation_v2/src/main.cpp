#include <systemc.h>
#include <systemc-ams.h>
#include "core.h"
#include "sys_functional_bus.h"
#include "sys_power_bus.h"
#include "temperature_sensor_functional.h"
#include "temperature_sensor_power.h"
#include "air_quality_sensor_functional.h"
#include "air_quality_sensor_power.h"


int sc_main(int argc, char* argv[])
{
    Core Master("Master");
    functional_bus Func("Func");
    power_bus Pwr("Power");
    temperature_sensor_functional Temp("T_Sensor");
    temperature_sensor_power      Temp_P("T_Power");
    air_quality_sensor_functional Air("Air_Sensor");
    air_quality_sensor_power      Air_P("Air_Power");
    
    //Functional Bus
    sc_signal <bool> Enable_Temp;
    //Data from Master to Functional Bus
    sc_signal <int>  A_M_to_B;
    sc_signal <int>  D_M_to_B;
    sc_signal <bool> F_M_to_B;
    //Data from Functional Bus to Master
    sc_signal <int>  D_B_to_M;
    //Data from Functional Bus to Slave
    sc_signal <int>  A_B_to_S[NS];
    sc_signal <int>  D_B_to_S[NS];
    sc_signal <bool>  F_B_to_S[NS];
    //Data from Slave to Functional Bus
    sc_signal <int> Fake;
    Func.data_in_S[0](Fake);

    //Power Bus
    //Signals from Slave to Bus
    sca_tdf::sca_signal <double> V_S_to_B[NP];
    sca_tdf::sca_signal <double> I_S_to_B[NP];
    //Signals from Battery to Bus
    sca_tdf::sca_signal <double> V_Batt_to_B;
    sca_tdf::sca_signal <double> SoC_Batt_to_B;
    //Signals from Bus to Battery
    sca_tdf::sca_signal <double> I_B_to_Batt;
    //Output Traces from Power Bus
    sca_tdf::sca_signal <double> Tot_Voltage;
    sca_tdf::sca_signal <double> Tot_Current;

    //Air Quality Data
    sc_signal <int> Air_Data;
    //Air Quality Power
    sc_signal <int> Air_F_to_P;
    sca_tdf::sca_signal <double> Air_V_State;
    sca_tdf::sca_signal <double> Air_I_State;
    
    //Temperature Data
    sc_signal <int>  Temp_Data;
    //Temperature Power
    sc_signal <int> Tmp_F_to_P;
    sca_tdf::sca_signal <double> Tmp_V_State;
    sca_tdf::sca_signal <double> Tmp_I_State;

    //Binding Master's signals
    Master.A_Out(A_M_to_B);
    Master.D_Out(D_M_to_B);
    Master.F_Out(F_M_to_B);
    Master.Data_in(D_B_to_M);
    //Binding Functional Bus's Input Address 
    Func.address_in_M(A_M_to_B);
    Func.data_in_M(D_M_to_B);
    Func.flag_in_M(F_M_to_B);
    //Binding Slave Input to Functional Bus
    //Air Quality Sensor
    Func.data_in_S[1](Air_Data);
    //Temperature Sensor
    Func.data_in_S[2](Temp_Data);

    //Binding Functional Bus's Output Address
    Func.data_out_M(D_B_to_M);
    for (size_t i = 0; i < NS; i++) {
        Func.address_out_S[i](A_B_to_S[i]);
        Func.data_out_S[i](D_B_to_S[i]);
        Func.flag_out_S[i](F_B_to_S[i]);
    }
    
    //Binding Power Bus's Input Signal
    for (size_t i = 0; i < NP; i++) {
        Pwr.voltage_in_S[i](V_S_to_B[i]);
        Pwr.current_in_S[i](I_S_to_B[i]);
    }
    //Pwr.battery_in_voltage(V_Batt_to_B);
    //Pwr.battery_in_SoC(SoC_Batt_to_B);
    //Binding Power Bus's Output Signal
    //Pwr.battery_out_current(I_B_to_Batt);
    Pwr.voltage_out_sum(Tot_Voltage);
    Pwr.current_out_sum(Tot_Current);

    //Binding Air Quality Sensor's signals
    Air.enable(Enable_Temp);
    Air.address(A_B_to_S[1]);
    Air.data_in(D_B_to_S[1]);
    Air.flag_wr(F_B_to_S[1]);
    Air.data_out(Air_Data);
    Air.power_signal(Air_F_to_P);
    //Binding Air Quality Power signals
    Air_P.func_signal(Air_F_to_P);
    Air_P.voltage_state(V_S_to_B[0]);
    Air_P.current_state(I_S_to_B[0]);

    //Binding Temperature Sensor's signals
    Temp.enable(Enable_Temp);
    Temp.address(A_B_to_S[2]);
    Temp.data_in(D_B_to_S[2]);
    Temp.flag_wr(F_B_to_S[2]);
    Temp.data_out(Temp_Data);
    Temp.power_signal(Tmp_F_to_P);
    //Binding Temperature Power signals
    Temp_P.func_signal(Tmp_F_to_P);
    Temp_P.voltage_state(V_S_to_B[1]);
    Temp_P.current_state(I_S_to_B[1]);
    
    //Define Power Simulation File
    sca_util::sca_trace_file* ptf = sca_util::sca_create_tabular_trace_file("power_trace.txt");
    //Power Traces
    sca_util::sca_trace(ptf, Tot_Voltage, "Total_Voltage");
    sca_util::sca_trace(ptf, Tot_Current, "Total_Current");
    sca_util::sca_trace(ptf, V_S_to_B[0], "Air_Voltage");
    sca_util::sca_trace(ptf, Air_P.current_state, "Air_Current");
    sca_util::sca_trace(ptf, Temp_P.voltage_state, "Temp_Voltage");
    sca_util::sca_trace(ptf, I_S_to_B[1], "Temp_Current");

    //Define Functional Simulation File
    sca_util::sca_trace_file* ftf = sca_util::sca_create_tabular_trace_file("funtional_trace.txt");
    //Simulation Traces
    sca_util::sca_trace(ftf, A_M_to_B, "Master_Adderess");
    sca_util::sca_trace(ftf, D_M_to_B, "Master_Data");
    sca_util::sca_trace(ftf, D_B_to_M, "Bus_Data");

    Enable_Temp.write(true);
    std::cout <<"The simulation starts!"<<endl;

    sc_start(10, sc_core::SC_SEC); // Set the simulation length

    std::cout <<"The simulation ends @ "<<sc_time_stamp()<<endl;

    //Close Trace Files
    sca_util::sca_close_tabular_trace_file(ptf);
    sca_util::sca_close_tabular_trace_file(ftf);

    return 0;
}