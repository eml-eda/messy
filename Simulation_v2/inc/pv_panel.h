#include <fstream>
#include <iostream>
#include <string>
#include <systemc-ams.h>

#include "config.h"
#include "config_pv.h"
#include "lut.h"


SCA_TDF_MODULE(pv_panel)
{
    sca_tdf::sca_out<double> i; // Output current at mpp
    sca_tdf::sca_out<double> v; // Output voltage at mpp

    SCA_CTOR(pv_panel): i("i"), v("v") {}

    void set_attributes();
    void initialize();
    void processing();

    pv_panel(){}
    
    private:
        int cnt; // when cnt % TRACE_PERIOD == 0 a new measure of irradiance 
                 // is read from the file
        ifstream top; // file from which irradiance values are retrieved
        double g_top;
        LUT lut_i = LUT(G, I_MPP, SIZE_PV);
        LUT lut_v = LUT(G, V_MPP, SIZE_PV);
};