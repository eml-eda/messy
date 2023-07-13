#include <systemc.h>
#include <systemc-ams.h>
#define NP 5

SCA_TDF_MODULE(power_bus){
    //Rate 1ms
    //Input Port
    sca_tdf::sca_in <double>  CPU_I;
    sca_tdf::sca_in <double>  CPU_V;
    sca_tdf::sca_in <double>  voltage_in_S[NP];
    sca_tdf::sca_in <double>  current_in_S[NP];

    //Output Port
    sca_tdf::sca_out <double>  battery_out_current;
    sca_tdf::sca_out <double>  voltage_out_sum;

    SCA_CTOR(power_bus):
    CPU_I("Current_of_CPU"),
    CPU_V("Voltage_of_CPU"),
    battery_out_current("Current_to_Battery"),
    voltage_out_sum("Sum_of_Voltages")
    {}

    void set_attributes();
    void initialize();
    void processing();

    private:
    double total_voltage = 0;
    double total_current = 0;

    power_bus(){}
};