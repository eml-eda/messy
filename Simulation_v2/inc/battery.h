#include <systemc-ams.h>

#include "battery_char.h"
#include "battery_voc.h"


SC_MODULE(battery)
{
    // Interface and internal components declaration
    sca_tdf::sca_in<double> i_batt; // Battery current
    sca_tdf::sca_out<double> v_batt; // Battery voltage
    sca_tdf::sca_out<double> soc; // Battery SOC

    // Connecting signals
    sca_tdf::sca_signal<double> v_oc, r_s;

    // Instantiation of battery componenets
    battery_voc* voc_module;
    battery_char* char_module;

    SC_CTOR(battery): i_batt("i_batt"),
                      v_batt("v_batt"),
                      soc("soc")
    {
        voc_module = new battery_voc("voc");
        char_module = new battery_char("batt");

        voc_module->i(i_batt);
        voc_module->v_oc(v_oc);
        voc_module->r_s(r_s);
        voc_module->soc(soc);

        char_module->r_s(r_s);
        char_module->i(i_batt);
        char_module->v_oc(v_oc);
        char_module->v(v_batt);
    }
};
