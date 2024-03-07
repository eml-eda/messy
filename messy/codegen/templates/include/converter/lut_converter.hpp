#include <systemc-ams.h>
#include <config.hpp>
#include <lut.hpp>
#define SIZE_CONV_DCDC_${name} ${len(lut["input_values"])}
static const double ${input_variable}_CONV_${name}[SIZE_CONV_DCDC_${name}] = {${str(lut["input_values"])[1:len(str(lut["input_values"]))-1]}};
static const double ETA_CONV_${name}[SIZE_CONV_DCDC_${name}] = {${str(lut["efficiency_values"])[1:len(str(lut["efficiency_values"]))-1]}};

SCA_TDF_MODULE(${name}_converter)
{
    sca_tdf::sca_in<double> current_in; // Current from PV panel
    sca_tdf::sca_in<double> voltage_in; // Voltage from PV panel
    sca_tdf::sca_out<double> current_out; // Current generated delivered to the BUS

    SCA_CTOR(${name}_converter): current_in("current_in"),
                            voltage_in("voltage_in"),
                            current_out("current_out") {};

    void set_attributes();
    void initialize();
    void processing();

    ${name}_converter(){}

    private:
        LUT lut_eta = LUT(${input_variable}_CONV_${name}, ETA_CONV_${name}, SIZE_CONV_DCDC_${name});
};
