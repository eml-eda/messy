#include <systemc-ams.h>
#include "config.h"
#include "config_converter_battery.h"
#include "lut.h"

SCA_TDF_MODULE(battery_converter)
{
    //Input Port
    sca_tdf::sca_in <double> current_in; //From Bus
    sca_tdf::sca_in <double> voltage_in; //From Battery
    //Output Port
    sca_tdf::sca_out <double> current_out; //To Battery

    SCA_CTOR(battery_converter):
        current_in("Current_from_power_bus"),
        voltage_in("Voltage_from_battery"),
        current_out("Current_to_battery")
    {}

    void set_attributes();
    void initialize();
    void processing();

    void set_efficency(double k);

    battery_converter(){}
    
    private:
        double efficency_factor = 1.0;
        LUT lut_eta = LUT(I_CONV_BATT, ETA_CONV_BATT, SIZE_CONV_BATT);
};