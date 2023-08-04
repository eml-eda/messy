#include <systemc.h>
#include <systemc-ams.h>
#include "core.h"
#include "core_power.h"
#include "sys_functional_bus.h"
#include "sys_power_bus.h"
#include "load_converter.h"
#include "air_quality_sensor_functional.h"
#include "air_quality_sensor_power.h"
#include "temperature_sensor_functional.h"
#include "temperature_sensor_power.h"
#include "methane_sensor_functional.h"
#include "methane_sensor_power.h"
#include "mic_click_sensor_functional.h"
#include "mic_click_sensor_power.h"
#include "rf_functional.h"
#include "rf_power.h"

#include "battery.h"
#include "battery_converter.h"
#include "battery_peukert.h"

#include "pv_panel.h"
#include "converter_pv.h"

int sc_main(int argc, char* argv[])
{
    Core                          Master("Master");
    Core_power                    Master_Power("Master_Power");
    functional_bus                Func("Func");
    power_bus                     Pwr("Power");
    air_quality_sensor_functional Air("Air_Sensor");
    air_quality_sensor_power      Air_P("Air_Power");
    load_converter                Air_conv("Air_Converter");
    temperature_sensor_functional Temp("T_Sensor");
    temperature_sensor_power      Temp_P("T_Power");
    load_converter                Temp_conv("Temp_Converter");
    methane_sensor_functional     Meth("Meth_Sensor");
    methane_sensor_power          Meth_P("Meth_Power");
    load_converter                Meth_conv("Meth_Converter");
    mic_click_sensor_functional   Mic("Mic_Sensor");
    mic_click_sensor_power        Mic_P("Mic_Power");
    load_converter                Mic_conv("Mic_Converter");
    rf_functional                 RF("Radio_Sensor");
    rf_power                      RF_P("Radio_Power");
    load_converter                RF_conv("Radio_Converter");
    battery                       Bat("Battery");
    battery_converter             Batt_conv("Battery_Converter");
    //battery_peukert               P_Batt("Peukert_Battery");
    pv_panel                      Pv("Solar_Panel");
    converter_pv                  Pv_conv("Solar_Converter");
    
    //Master Functional Signal
    sc_signal <int> Master_F_to_P;
    //Master Poweer Signal
    sca_tdf::sca_signal <double> V_M_to_B;
    sca_tdf::sca_signal <double> I_M_to_B;
    //Functional Bus
    sc_signal <bool> Enable_Temp;
    //Data from Master to Functional Bus
    sc_signal <int>  A_M_to_B;
    sc_signal <int>  D_M_to_B;
    sc_signal <bool> F_M_to_B;
    sc_signal <bool> Ready_M_to_B;
    //Data from Functional Bus to Master
    sc_signal <int>  D_B_to_M;
    sc_signal <bool> Go_B_to_M;
    //Data from Functional Bus to Slave
    sc_signal <int>  A_B_to_S[NS];
    sc_signal <int>  D_B_to_S[NS];
    sc_signal <bool> F_B_to_S[NS];
    sc_signal <bool> Ready_B_to_S[NS];
    //Data from Slave to Functional Bus
    sc_signal <int> Fake_D;
    sc_signal <bool> Fake_GO;
    Func.data_in_S[0](Fake_D);
    Func.Go_S[0](Fake_GO);

    //Power Bus
    //Signals from Slave to Bus
    sca_tdf::sca_signal <double> V_S_to_B[NP];
    sca_tdf::sca_signal <double> I_S_to_B[NP];
    //Output Traces from Power Bus
    sca_tdf::sca_signal <double> Tot_Voltage;

    //Air Quality Data
    sc_signal <int> Air_Data;
    sc_signal <bool> Air_Go;
    //Air Quality Power
    sc_signal <int> Air_F_to_P;
    sca_tdf::sca_signal <double> Air_V_State;
    sca_tdf::sca_signal <double> Air_I_State;
    //Air Converter
    sca_tdf::sca_signal <double> Air_I_S_to_C;
    
    //Temperature Data
    sc_signal <int>  Temp_Data;
    sc_signal <bool> Temp_Go;
    //Temperature Power
    sc_signal <int> Tmp_F_to_P;
    sca_tdf::sca_signal <double> Tmp_V_State;
    sca_tdf::sca_signal <double> Tmp_I_State;
    //Temperature Converter
    sca_tdf::sca_signal <double> Tmp_I_S_to_C;

    //Methane Data
    sc_signal <int>  Meth_Data;
    sc_signal <bool> Meth_Go;
    //Methane Power
    sc_signal <int> Meth_F_to_P;
    sca_tdf::sca_signal <double> Meth_V_State;
    sca_tdf::sca_signal <double> Meth_I_State;
    //Methane Converter
    sca_tdf::sca_signal <double> Meth_I_S_to_C;

    //Microphone Data
    sc_signal <int>  Mic_Data;
    sc_signal <bool> Mic_Go;
    //Microphone Power
    sc_signal <int> Mic_F_to_P;
    sca_tdf::sca_signal <double> Mic_V_State;
    sca_tdf::sca_signal <double> Mic_I_State;
    //Microphone Converter
    sca_tdf::sca_signal <double> Mic_I_S_to_C;

    //Radio Frequency Data
    sc_signal <int>  Rf_Data;
    sc_signal <bool> Rf_Go;
    //Radio Frequency Power
    sc_signal <int> Rf_F_to_P;
    sca_tdf::sca_signal <double> Rf_V_State;
    sca_tdf::sca_signal <double> Rf_I_State;
    //Radio Frequency Converter
    sca_tdf::sca_signal <double> Rf_I_S_to_C;

    //Battery Signals
    sca_tdf::sca_signal <double> SoC_Batt;
    sca_tdf::sca_signal <double> V_Batt;
    sca_tdf::sca_signal <double> I_Batt;
    //Peukert Battery Signals
    sca_tdf::sca_signal <double> Lifetime;
    //Battery Converter Signals
    sca_tdf::sca_signal <double> I_Bus_to_C_Batt;

    //Solar Panel Signals
    sca_tdf::sca_signal <double> PV_I;
    sca_tdf::sca_signal <double> PV_V;
    //Solar Converter Signal
    sca_tdf::sca_signal <double> PV_I_C_to_Bus;

    //Binding Functional Master's signals
    Master.A_Out(A_M_to_B);
    Master.D_Out(D_M_to_B);
    Master.F_Out(F_M_to_B);
    Master.Ready(Ready_M_to_B);
    Master.Go(Go_B_to_M);
    Master.Data_in(D_B_to_M);
    Master.power_signal(Master_F_to_P);
    //Binding Power MAster's signals
    Master_Power.func_signal(Master_F_to_P);
    Master_Power.voltage_state(V_M_to_B);
    Master_Power.current_state(I_M_to_B);
    //Binding Functional Bus Master's Input 
    Func.address_in_M(A_M_to_B);
    Func.data_in_M(D_M_to_B);
    Func.flag_in_M(F_M_to_B);
    Func.Ready_M(Ready_M_to_B);
    //Binding Slave Input to Functional Bus
    //Air Quality Sensor
    Func.data_in_S[1](Air_Data);
    Func.Go_S[1](Air_Go);
    //Temperature Sensor
    Func.data_in_S[2](Temp_Data);
    Func.Go_S[2](Temp_Go);
    //Methane Sensor
    Func.data_in_S[3](Meth_Data);
    Func.Go_S[3](Meth_Go);
    //Microphone Sensor
    Func.data_in_S[4](Mic_Data);
    Func.Go_S[4](Mic_Go);
    //Radio Frequency Sensor
    Func.data_in_S[5](Rf_Data);
    Func.Go_S[5](Rf_Go);

    //Binding Functional Bus's Output Address
    Func.data_out_M(D_B_to_M);
    Func.Go_M(Go_B_to_M);
    for (size_t i = 0; i < NS; i++) {
        Func.address_out_S[i](A_B_to_S[i]);
        Func.data_out_S[i](D_B_to_S[i]);
        Func.flag_out_S[i](F_B_to_S[i]);
        Func.Ready_S[i](Ready_B_to_S[i]);
    }
    
    //Binding Power Bus's Input Signal
    Pwr.CPU_V(V_M_to_B);
    Pwr.CPU_I(I_M_to_B);
    Pwr.PV_I(PV_I_C_to_Bus);

    for (size_t i = 0; i < NP; i++) {
        Pwr.voltage_in_S[i](V_S_to_B[i]);
        Pwr.current_in_S[i](I_S_to_B[i]);
    }
    //Binding Power Bus's Output Signal
    Pwr.battery_out_current(I_Bus_to_C_Batt);

    //Binding Air Quality Sensor's signals
    Air.enable(Enable_Temp);
    Air.address(A_B_to_S[1]);
    Air.data_in(D_B_to_S[1]);
    Air.flag_wr(F_B_to_S[1]);
    Air.ready(Ready_B_to_S[1]);
    Air.data_out(Air_Data);
    Air.go(Air_Go);
    Air.power_signal(Air_F_to_P);
    //Binding Air Quality Power signals
    Air_P.func_signal(Air_F_to_P);
    Air_P.voltage_state(V_S_to_B[0]);
    Air_P.current_state(Air_I_S_to_C);
    //Binding Air Converter signals
    Air_conv.current_in(Air_I_S_to_C);
    Air_conv.voltage_in(V_S_to_B[0]);
    Air_conv.current_out(I_S_to_B[0]);
    //Set-Up Air Converter Efficency
    Air_conv.set_efficency(1);

    //Binding Temperature Sensor's signals
    Temp.enable(Enable_Temp);
    Temp.address(A_B_to_S[2]);
    Temp.data_in(D_B_to_S[2]);
    Temp.flag_wr(F_B_to_S[2]);
    Temp.ready(Ready_B_to_S[2]);
    Temp.data_out(Temp_Data);
    Temp.go(Temp_Go);
    Temp.power_signal(Tmp_F_to_P);
    //Binding Temperature Power signals
    Temp_P.func_signal(Tmp_F_to_P);
    Temp_P.voltage_state(V_S_to_B[1]);
    Temp_P.current_state(Tmp_I_S_to_C);
    //Binding Temperature Converter signals
    Temp_conv.current_in(Tmp_I_S_to_C);
    Temp_conv.voltage_in(V_S_to_B[1]);
    Temp_conv.current_out(I_S_to_B[1]);
    //Set-Up Temperature Converter Efficency
    Temp_conv.set_efficency(1.0);

    //Binding Methane Sensor's signals
    Meth.enable(Enable_Temp);
    Meth.address(A_B_to_S[3]);
    Meth.data_in(D_B_to_S[3]);
    Meth.flag_wr(F_B_to_S[3]);
    Meth.ready(Ready_B_to_S[3]);
    Meth.data_out(Meth_Data);
    Meth.go(Meth_Go);
    Meth.power_signal(Meth_F_to_P);
    //Binding Methane Power signals
    Meth_P.func_signal(Meth_F_to_P);
    Meth_P.voltage_state(V_S_to_B[2]);
    Meth_P.current_state(Meth_I_S_to_C);
    //Binding Methane Converter signals
    Meth_conv.current_in(Meth_I_S_to_C);
    Meth_conv.voltage_in(V_S_to_B[2]);
    Meth_conv.current_out(I_S_to_B[2]);
    //Set-Up Methane Converter Efficency
    Meth_conv.set_efficency(1.0);

    //Binding Microphone Sensor's signals
    Mic.enable(Enable_Temp);
    Mic.address(A_B_to_S[4]);
    Mic.data_in(D_B_to_S[4]);
    Mic.flag_wr(F_B_to_S[4]);
    Mic.ready(Ready_B_to_S[4]);
    Mic.data_out(Mic_Data);
    Mic.go(Mic_Go);
    Mic.power_signal(Mic_F_to_P);
    //Binding Microphone Power signals
    Mic_P.func_signal(Mic_F_to_P);
    Mic_P.voltage_state(V_S_to_B[3]);
    Mic_P.current_state(Mic_I_S_to_C);
    //Binding Microphone Converter signals
    Mic_conv.current_in(Mic_I_S_to_C);
    Mic_conv.voltage_in(V_S_to_B[3]);
    Mic_conv.current_out(I_S_to_B[3]);
    //Set-Up Microphone Converter Efficency
    Mic_conv.set_efficency(1.0);

    //Binding Radio Frequency Sensor's signals
    RF.enable(Enable_Temp);
    RF.address(A_B_to_S[5]);
    RF.data_in(D_B_to_S[5]);
    RF.flag_wr(F_B_to_S[5]);
    RF.ready(Ready_B_to_S[5]);
    RF.data_out(Rf_Data);
    RF.go(Rf_Go);
    RF.power_signal(Rf_F_to_P);
    //Binding Radio Frequency Power signals
    RF_P.func_signal(Rf_F_to_P);
    RF_P.voltage_state(V_S_to_B[4]);
    RF_P.current_state(Rf_I_S_to_C);
    //Binding Radio Frequency Converter signals
    RF_conv.current_in(Rf_I_S_to_C);
    RF_conv.voltage_in(V_S_to_B[4]);
    RF_conv.current_out(I_S_to_B[4]);
    //Set-Up Radio Frequency Converter Efficency
    RF_conv.set_efficency(1.0);

    //Binding Battery Converter signals
    Batt_conv.current_in(I_Bus_to_C_Batt);
    Batt_conv.voltage_in(V_Batt);
    Batt_conv.current_out(I_Batt);

    //Binding Battery signals
    Bat.i_batt(I_Batt);
    Bat.v_batt(V_Batt);
    Bat.soc(SoC_Batt);
    //Binding Peukert Battery signals
//    P_Batt.in(I_Batt);
//    P_Batt.Vbatt(V_Batt);
//    P_Batt.Soc(SoC_Batt);
//    P_Batt.Lt(Lifetime);
    
    //Binding Solar Panel signals
    Pv.i(PV_I);
    Pv.v(PV_V);
    //Binding Solar Converter signals
    Pv_conv.i_in(PV_I);
    Pv_conv.v_in(PV_V);
    Pv_conv.i_out(PV_I_C_to_Bus);
    
    //Define Power Simulation File
    sca_util::sca_trace_file* ptf = sca_util::sca_create_tabular_trace_file("power_trace.txt");
    //Power Traces
    sca_util::sca_trace(ptf, SoC_Batt, "SoC_Batt");
    //sca_util::sca_trace(ptf, V_Batt, "Batt_Voltage");
    //sca_util::sca_trace(ptf, Tot_Voltage, "Total_Voltage_Post_Conv");
    //sca_util::sca_trace(ptf, Air_V_S_to_C, "Air_Voltage_Pre_Conv");
    //sca_util::sca_trace(ptf, Air_P.current_state, "Air_Current_Pre_Conv");
    sca_util::sca_trace(ptf, I_Bus_to_C_Batt, "I_Tot_Bus");
    sca_util::sca_trace(ptf, I_Batt, "I_to_Batt");
    sca_util::sca_trace(ptf, I_S_to_B[0], "Air");
    //sca_util::sca_trace(ptf, Temp_P.voltage_state, "Temp_Voltage");
    //sca_util::sca_trace(ptf, Tmp_I_S_to_C, "Temp_Current_Pre_Conv");
    sca_util::sca_trace(ptf, I_S_to_B[1], "Temp");
    sca_util::sca_trace(ptf, I_S_to_B[2], "Meth");
    sca_util::sca_trace(ptf, I_S_to_B[3], "Mic");
    sca_util::sca_trace(ptf, I_M_to_B, "CPU");
    sca_util::sca_trace(ptf, I_S_to_B[4], "RF");

    //Define Functional Simulation File
    sca_util::sca_trace_file* ftf = sca_util::sca_create_tabular_trace_file("funtional_trace.txt");
    //Simulation Traces
    sca_util::sca_trace(ftf, A_M_to_B, "Master_Adderess");
    sca_util::sca_trace(ftf, D_M_to_B, "Master_Data");
    sca_util::sca_trace(ftf, D_B_to_M, "Bus_Data");

    Enable_Temp.write(true);
    std::cout <<"The simulation starts!"<<endl;

    sc_start(SIM_LEN , sc_core::SC_SEC); // Set the simulation length

    std::cout <<"The simulation ends @ "<<sc_time_stamp()<<endl;

    //Close Trace Files
    sca_util::sca_close_tabular_trace_file(ptf);
    sca_util::sca_close_tabular_trace_file(ftf);

    return 0;
}