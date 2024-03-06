#include <systemc-ams.h>
#include "config.h"
#include "lut.h"
#define SIZE_CONV_DCDC_${name}
static const double ${input_variable}_CONV_${name}[SIZE_CONV_${name}] = {${str(lut["input_values"])[1:len(str(lut["input_values"]))-1]}};
static const double ETA_CONV_${name}[SIZE_CONV_${name}] = {${str(lut["efficiency_values"])[1:len(str(lut["efficiency_values"]))-1]}};

SCA_TDF_MODULE(${name}_converter)
{
    sca_tdf::sca_in<double> i_in; // Current from PV panel
    sca_tdf::sca_in<double> v_in; // Voltage from PV panel
    sca_tdf::sca_out<double> i_out; // Current generated delivered to the BUS

    SCA_CTOR(${name}_converter): i_in("i_in"),
                            v_in("v_in"),
                            i_out("i_out") {};

    void set_attributes();
    void initialize();
    void processing();

    ${name}_converter(){}

    private:
        LUT lut_eta = LUT(${input_variable}_CONV_${name}, ETA_CONV_${name}, SIZE_CONV_DCDC_${name});
};
