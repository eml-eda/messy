#include <systemc.h>
#include <systemc-ams.h>
#include "battery.h"
#include "battery_converter.h"
#include "core.h"
#include "core_power.h"
#include "sys_functional_bus.h"
#include "sys_power_bus.h"
#include "load_converter.h"
#include "pv_panel.h"
#include "converter_pv.h"
#include "air_quality_sensor_functional.h"
#include "air_quality_sensor_power.h"
#include "temperature_sensor_functional.h"
#include "temperature_sensor_power.h"
#include "methane_sensor_functional.h"
#include "methane_sensor_power.h"


int sc_main(int argc, char* argv[])
{
    Core                          Master("Master");
    Core_power                    Master_Power("Master_Power");
    functional_bus                Func("Func");
    power_bus                     Pwr("Power");
    battery                       Bat("Battery");
    battery_converter             Batt_conv("Battery_Converter");
    pv_panel                      Pv("Solar_Panel");
    converter_pv                  Pv_conv("Solar_Converter");

    air_quality_sensor_functional AIR_QUALITY_SENSOR("air_quality_sensor");
    air_quality_sensor_power AIR_QUALITY_SENSOR_P("air_quality_sensor_Power");
    load_converter AIR_QUALITY_SENSOR_CONV("air_quality_sensor_Converter");  
    temperature_sensor_functional TEMPERATURE_SENSOR("temperature_sensor");
    temperature_sensor_power TEMPERATURE_SENSOR_P("temperature_sensor_Power");
    load_converter TEMPERATURE_SENSOR_CONV("temperature_sensor_Converter");  
    methane_sensor_functional METHANE_SENSOR("methane_sensor");
    methane_sensor_power METHANE_SENSOR_P("methane_sensor_Power");
    load_converter METHANE_SENSOR_CONV("methane_sensor_Converter");  
    
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

    //Battery Signals
    sca_tdf::sca_signal <double> SoC_Batt;
    sca_tdf::sca_signal <double> V_Batt;
    sca_tdf::sca_signal <double> I_Batt;
    //Battery Converter Signals
    sca_tdf::sca_signal <double> I_Bus_to_C_Batt;

    //Solar Panel Signals
    sca_tdf::sca_signal <double> PV_I;
    sca_tdf::sca_signal <double> PV_V;
    //Solar Converter Signal
    sca_tdf::sca_signal <double> PV_I_C_to_Bus;

    //AIR_QUALITY_SENSOR Data
    sc_signal <int> AIR_QUALITY_SENSOR_Data;
    sc_signal <bool> AIR_QUALITY_SENSOR_Go;  
    //AIR_QUALITY_SENSOR Power
    sc_signal <int> AIR_QUALITY_SENSOR_F_to_P;
    sca_tdf::sca_signal <double> AIR_QUALITY_SENSOR_V_State;
    sca_tdf::sca_signal <double> AIR_QUALITY_SENSOR_I_State;
    //AIR_QUALITY_SENSOR Converter
    sca_tdf::sca_signal <double> AIR_QUALITY_SENSOR_I_S_to_C;  
    //TEMPERATURE_SENSOR Data
    sc_signal <int> TEMPERATURE_SENSOR_Data;
    sc_signal <bool> TEMPERATURE_SENSOR_Go;  
    //TEMPERATURE_SENSOR Power
    sc_signal <int> TEMPERATURE_SENSOR_F_to_P;
    sca_tdf::sca_signal <double> TEMPERATURE_SENSOR_V_State;
    sca_tdf::sca_signal <double> TEMPERATURE_SENSOR_I_State;
    //TEMPERATURE_SENSOR Converter
    sca_tdf::sca_signal <double> TEMPERATURE_SENSOR_I_S_to_C;  
    //METHANE_SENSOR Data
    sc_signal <int> METHANE_SENSOR_Data;
    sc_signal <bool> METHANE_SENSOR_Go;  
    //METHANE_SENSOR Power
    sc_signal <int> METHANE_SENSOR_F_to_P;
    sca_tdf::sca_signal <double> METHANE_SENSOR_V_State;
    sca_tdf::sca_signal <double> METHANE_SENSOR_I_State;
    //METHANE_SENSOR Converter
    sca_tdf::sca_signal <double> METHANE_SENSOR_I_S_to_C;  
    
    //Binding Functional Master's signals
    Master.A_Out(A_M_to_B);
    Master.D_Out(D_M_to_B);
    Master.F_Out(F_M_to_B);
    Master.Ready(Ready_M_to_B);
    Master.Go(Go_B_to_M);
    Master.Data_in(D_B_to_M);
    Master.power_signal(Master_F_to_P);
    //Binding Power Master's signals
    Master_Power.func_signal(Master_F_to_P);
    Master_Power.voltage_state(V_M_to_B);
    Master_Power.current_state(I_M_to_B);
    //Binding Functional Bus Master's Input 
    Func.address_in_M(A_M_to_B);
    Func.data_in_M(D_M_to_B);
    Func.flag_in_M(F_M_to_B);
    Func.Ready_M(Ready_M_to_B);
    //Binding Slave Input to Functional Bus
    //AIR_QUALITY_SENSOR
    Func.data_in_S[1](AIR_QUALITY_SENSOR_Data);
    Func.Go_S[1](AIR_QUALITY_SENSOR_Go);
    //TEMPERATURE_SENSOR
    Func.data_in_S[2](TEMPERATURE_SENSOR_Data);
    Func.Go_S[2](TEMPERATURE_SENSOR_Go);
    //METHANE_SENSOR
    Func.data_in_S[3](METHANE_SENSOR_Data);
    Func.Go_S[3](METHANE_SENSOR_Go);
    

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

    //Binding AIR_QUALITY_SENSOR's signals
    AIR_QUALITY_SENSOR.enable(Enable_Temp);
    AIR_QUALITY_SENSOR.address(A_B_to_S[1]);
    AIR_QUALITY_SENSOR.data_in(D_B_to_S[1]);
    AIR_QUALITY_SENSOR.flag_wr(F_B_to_S[1]);
    AIR_QUALITY_SENSOR.ready(Ready_B_to_S[1]);
    AIR_QUALITY_SENSOR.data_out(AIR_QUALITY_SENSOR_Data);
    AIR_QUALITY_SENSOR.go(AIR_QUALITY_SENSOR_Go);
    AIR_QUALITY_SENSOR.power_signal(AIR_QUALITY_SENSOR_F_to_P);
    //Binding AIR_QUALITY_SENSOR Power signals
    AIR_QUALITY_SENSOR_P.func_signal(AIR_QUALITY_SENSOR_F_to_P);
    AIR_QUALITY_SENSOR_P.voltage_state(V_S_to_B[0]);
    AIR_QUALITY_SENSOR_P.current_state(AIR_QUALITY_SENSOR_I_S_to_C);
    //Binding AIR_QUALITY_SENSOR Converter signals
    AIR_QUALITY_SENSOR_CONV.current_in(AIR_QUALITY_SENSOR_I_S_to_C);
    AIR_QUALITY_SENSOR_CONV.voltage_in(V_S_to_B[0]);
    AIR_QUALITY_SENSOR_CONV.current_out(I_S_to_B[0]);
    //Set-Up AIR_QUALITY_SENSOR Converter Efficency
    AIR_QUALITY_SENSOR_CONV.set_efficency(1.0);

    //Binding TEMPERATURE_SENSOR's signals
    TEMPERATURE_SENSOR.enable(Enable_Temp);
    TEMPERATURE_SENSOR.address(A_B_to_S[2]);
    TEMPERATURE_SENSOR.data_in(D_B_to_S[2]);
    TEMPERATURE_SENSOR.flag_wr(F_B_to_S[2]);
    TEMPERATURE_SENSOR.ready(Ready_B_to_S[2]);
    TEMPERATURE_SENSOR.data_out(TEMPERATURE_SENSOR_Data);
    TEMPERATURE_SENSOR.go(TEMPERATURE_SENSOR_Go);
    TEMPERATURE_SENSOR.power_signal(TEMPERATURE_SENSOR_F_to_P);
    //Binding TEMPERATURE_SENSOR Power signals
    TEMPERATURE_SENSOR_P.func_signal(TEMPERATURE_SENSOR_F_to_P);
    TEMPERATURE_SENSOR_P.voltage_state(V_S_to_B[1]);
    TEMPERATURE_SENSOR_P.current_state(TEMPERATURE_SENSOR_I_S_to_C);
    //Binding TEMPERATURE_SENSOR Converter signals
    TEMPERATURE_SENSOR_CONV.current_in(TEMPERATURE_SENSOR_I_S_to_C);
    TEMPERATURE_SENSOR_CONV.voltage_in(V_S_to_B[1]);
    TEMPERATURE_SENSOR_CONV.current_out(I_S_to_B[1]);
    //Set-Up TEMPERATURE_SENSOR Converter Efficency
    TEMPERATURE_SENSOR_CONV.set_efficency(1.0);

    //Binding METHANE_SENSOR's signals
    METHANE_SENSOR.enable(Enable_Temp);
    METHANE_SENSOR.address(A_B_to_S[3]);
    METHANE_SENSOR.data_in(D_B_to_S[3]);
    METHANE_SENSOR.flag_wr(F_B_to_S[3]);
    METHANE_SENSOR.ready(Ready_B_to_S[3]);
    METHANE_SENSOR.data_out(METHANE_SENSOR_Data);
    METHANE_SENSOR.go(METHANE_SENSOR_Go);
    METHANE_SENSOR.power_signal(METHANE_SENSOR_F_to_P);
    //Binding METHANE_SENSOR Power signals
    METHANE_SENSOR_P.func_signal(METHANE_SENSOR_F_to_P);
    METHANE_SENSOR_P.voltage_state(V_S_to_B[2]);
    METHANE_SENSOR_P.current_state(METHANE_SENSOR_I_S_to_C);
    //Binding METHANE_SENSOR Converter signals
    METHANE_SENSOR_CONV.current_in(METHANE_SENSOR_I_S_to_C);
    METHANE_SENSOR_CONV.voltage_in(V_S_to_B[2]);
    METHANE_SENSOR_CONV.current_out(I_S_to_B[2]);
    //Set-Up METHANE_SENSOR Converter Efficency
    METHANE_SENSOR_CONV.set_efficency(1.0);

    //Binding Battery Converter signals
    Batt_conv.current_in(I_Bus_to_C_Batt);
    Batt_conv.voltage_in(V_Batt);
    Batt_conv.current_out(I_Batt);

    //Binding Battery signals
    Bat.i_batt(I_Batt);
    Bat.v_batt(V_Batt);
    Bat.soc(SoC_Batt);
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
    sca_util::sca_trace(ptf, I_Bus_to_C_Batt, "I_Tot_Bus");
    sca_util::sca_trace(ptf, I_Batt, "I_to_Batt");
    sca_util::sca_trace(ptf, I_M_to_B, "CPU");
    sca_util::sca_trace(ptf, I_S_to_B[0], "air_quality_sensor");
    sca_util::sca_trace(ptf, I_S_to_B[1], "temperature_sensor");
    sca_util::sca_trace(ptf, I_S_to_B[2], "methane_sensor");
    
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