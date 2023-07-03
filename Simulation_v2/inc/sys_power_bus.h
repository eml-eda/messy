#include <systemc.h>
#include <systemc-ams.h>
#define NP 2

SCA_TDF_MODULE(power_bus){
    //Rate 1ms
    //Input Port
    sca_tdf::sca_in <double>  voltage_in_S[NP];
    sca_tdf::sca_in <double>  current_in_S[NP];
    //sca_tdf::sca_in <double>  battery_in_voltage;
    //sca_tdf::sca_in <double>  battery_in_SoC;

    //Output Port
    //sca_tdf::sca_out <double>  battery_out_current;
    sca_tdf::sca_out <double>  voltage_out_sum;
    sca_tdf::sca_out <double>  current_out_sum;

    SCA_CTOR(power_bus):
    //battery_in_voltage("Voltage_from_Battery"),
    //battery_in_SoC("SoC_from_Battery"),
    //battery_out_current("Current_to_Battery"),
    voltage_out_sum("Sum_of_Voltages"),
    current_out_sum("Sum_of_Currents")
    {}

    void set_attributes();
    void initialize();
    void processing();

    private:
    double total_voltage = 0;
    double total_current = 0;

    power_bus(){}
};