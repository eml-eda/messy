#include <systemc-ams.h>

SCA_TDF_MODULE(battery_voc)
{
    sca_tdf::sca_in<double> i; // Battery current
    sca_tdf::sca_out<double> v_oc; // Voltage open-circuit
    sca_tdf::sca_out<double> r_s; // Series resistance
    sca_tdf::sca_out<double> soc; // State of Charge

    SCA_CTOR(battery_voc): v_oc("v_oc"),
                           r_s("r_s"),
                           soc("soc"),
                           tmpsoc(1.0),
                           prev_i_batt(0) {}

    void set_attributes();
    void initialize();
    void processing();
    
    private:
        double tmpsoc;
        double prev_i_batt;
};