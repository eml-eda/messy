#include <systemc-ams.h>

SCA_TDF_MODULE(air_quality_therm)
{
    sca_tdf::sc_in <double> input;
    sca_tdf::sc_out <double> therm_out;

    SCA_CTOR(air_quality_therm):
        input("Enable_of_Therm_Model"),
        therm_out("Air_qualty_Therm_Out")
    {}

    void set_attributes();
    void initialize();
    void processing();

    air_quality_therm(){}
};