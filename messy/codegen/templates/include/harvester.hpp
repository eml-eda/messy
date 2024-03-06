#include <systemc-ams.h>

% if iref and type(iref)==dict:
#define SIZE_IREF_${harvester_name}
static const double INPUT_VALUES_IREF_${harvester_name}[SIZE_IREF_${harvester_name}] = {${str(iref["lut"]["input_values"])[1:len(str(iref["lut"]["input_values"]))-1]}};
static const double IREF_${harvester_name}_VALUES[SIZE_IREF_${harvester_name}] = {${str(iref["lut"]["current_values"])[1:len(str(iref["lut"]["current_values"]))-1]}};
% endif

% if vref and type(vref)==dict:
#define SIZE_VREF_${harvester_name}
static const double INPUT_VALUES_VREF_${harvester_name}[SIZE_VREF_${harvester_name}] = {${str(vref["lut"]["input_values"])[1:len(str(vref["lut"]["input_values"]))-1]}};
static const double VREF_${harvester_name}_VALUES[SIZE_VREF_${harvester_name}] = {${str(vref["lut"]["voltage_values"])[1:len(str(vref["lut"]["voltage_values"]))-1]}};
% endif

% if harvester_type!="battery":
SCA_TDF_MODULE(Harvester_${harvester_name})
{
    sca_tdf::sca_out<double> i; // Output current at mpp
    sca_tdf::sca_out<double> v; // Output voltage at mpp

    SCA_CTOR(Harvester_${harvester_name}): i("i"), v("v") {}

    void set_attributes();
    void initialize();
    void processing();

    Harvester_${harvester_name}(){}
    
    private:
        % if iref and type(iref)==dict:
        ifstream iref_input_file;
        LUT lut_i = LUT(INPUT_VALUES_IREF_${harvester_name}, IREF_${harvester_name}_VALUES, SIZE_IREF_${harvester_name});
        % endif
        % if vref and type(vref)==dict:
        ifstream vref_input_file;
        LUT lut_v = LUT(INPUT_VALUES_VREF_${harvester_name}, VREF_${harvester_name}_VALUES, SIZE_VREF_${harvester_name});
        % endif
};
% else:
SC_MODULE(Harvester_${harvester_name})
{
    // Interface and internal components declaration
    sca_tdf::sca_in<double> i; // Battery current
    sca_tdf::sca_out<double> v; // Battery voltage
    sca_tdf::sca_out<double> soc; // Battery SOC

    // Connecting signals
    sca_tdf::sca_signal<double> v_oc, r_s;

    // Instantiation of battery componenets
    Harvester_${harvester_name}_battery_voc* voc_module;
    Harvester_${harvester_name}_battery_char* char_module;

    SC_CTOR(Harvester_${harvester_name}): i("i"),
                      v("v"),
                      soc("soc")
    {
        voc_module = new Harvester_${harvester_name}_battery_voc("voc");
        char_module = new Harvester_${harvester_name}_battery_char("batt");

        voc_module->i(i);
        voc_module->v_oc(v_oc);
        voc_module->r_s(r_s);
        voc_module->soc(soc);

        char_module->r_s(r_s);
        char_module->i(i);
        char_module->v_oc(v_oc);
        char_module->v(v);
    }
};


SCA_TDF_MODULE(Harvester_${harvester_name}_battery_voc)
{
    sca_tdf::sca_in<double> i; // Battery current
    sca_tdf::sca_out<double> v_oc; // Voltage open-circuit
    sca_tdf::sca_out<double> r_s; // Series resistance
    sca_tdf::sca_out<double> soc ; // State of Charge

    SCA_CTOR(Harvester_${harvester_name}_battery_voc): v_oc("v_oc"),
                           r_s("r_s"),
                           soc("soc"),
                           tmpsoc(({{starting_voc}}/100)),
                           prev_i_batt(0) {}

    void set_attributes();
    void initialize();
    void processing();
    
    private:
        // battery is 32mAh
	    int c_nom = 32;
        double tmpsoc={{starting_voc/100}}f;
        double prev_i_batt;
};



SC_MODULE(Harvester_${harvester_name}_battery_char)
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

    SC_CTOR(Harvester_${harvester_name}_battery_char)
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
% endif