#include <systemc-ams.h>


SC_MODULE(battery_char)
{
    // Interface and internal components declaration
    sca_tdf::sca_in<double> r_s, // Internal resistance
                            i, // Battery current
                            v_oc; // Battery V_oc
    sca_tdf::sca_out<double> v; // V_batt

    sca_eln::sca_tdf::sca_isource* I_batt;
    sca_eln::sca_tdf::sca_vsink* V_batt;

    sca_eln::sca_tdf_vsource* V_oc;
    sca_eln::sca_node n1, n2;
    sca_eln::sca_node_ref gnd;
    sca_eln::sca_tdf::sca_r* R_s;

    SC_CTOR(battery_char)
    {
        // V_oc voltage instantiation
        V_oc = new sca_eln::sca_tdf_vsource("V_oc");
        V_oc->inp(v_oc);
        V_oc->p(n1);
        V_oc->n(gnd);
        // Internal resistance instantiation
        R_s = new sca_eln::sca_tdf::sca_r("R_s");
        R_s->p(n1);
        R_s->n(n2);
        R_s->scale=1.0;
        R_s->inp(r_s);
        //Load current instantiation
        I_batt = new sca_eln::sca_tdf::sca_isource("I_batt");
        I_batt->inp(i);
        I_batt->p(n2);
        I_batt->n(gnd);
        //Output voltage of the battery
        V_batt = new sca_eln::sca_tdf::sca_vsink("V_batt");
        V_batt->p(n2);
        V_batt->n(gnd);
        V_batt->outp(v);
    }
};
