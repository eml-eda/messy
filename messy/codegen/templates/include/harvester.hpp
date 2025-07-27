#include <systemc-ams.h>
#include <config.hpp>

% if (iref and type(iref)==dict) or (vref and type(vref)==dict):
#include <lut.hpp>
#include <fstream>
#include <iostream>
% endif

% if iref and type(iref)==dict:
#define SIZE_IREF_${harvester_name} ${len(iref["lut"]["input_values"])}
static const double INPUT_VALUES_IREF_${harvester_name}[SIZE_IREF_${harvester_name}] = {${str(iref["lut"]["input_values"])[1:len(str(iref["lut"]["input_values"]))-1]}};
static const double IREF_${harvester_name}_VALUES[SIZE_IREF_${harvester_name}] = {${str(iref["lut"]["current_values"])[1:len(str(iref["lut"]["current_values"]))-1]}};
% endif

% if vref and type(vref)==dict:
#define SIZE_VREF_${harvester_name} ${len(vref["lut"]["input_values"])}
static const double INPUT_VALUES_VREF_${harvester_name}[SIZE_VREF_${harvester_name}] = {${str(vref["lut"]["input_values"])[1:len(str(vref["lut"]["input_values"]))-1]}};
static const double VREF_${harvester_name}_VALUES[SIZE_VREF_${harvester_name}] = {${str(vref["lut"]["voltage_values"])[1:len(str(vref["lut"]["voltage_values"]))-1]}};
% endif

% if harvester_type!="battery":
SCA_TDF_MODULE(Harvester_${harvester_name})
{
    sca_tdf::sca_out<double> i; // Output current at mpp
    sca_tdf::sca_out<double> v; // Output voltage at mpp

    SCA_CTOR(Harvester_${harvester_name}): i("i"), v("v") {}

    /**
     * @brief Sets the attributes of the Harvester module.
     * 
     * This function sets the timestep for the output current (i) and voltage (v) signals.
     */
    void set_attributes();
    /**
     * @brief Initializes the Harvester module.
     * 
     * This function opens the input files for iref and vref if they are defined.
     */
    void initialize();
    /**
     * @brief Processes the Harvester module's functionality.
     * 
     * This function reads the input values for iref and vref from their respective files
     * and writes the corresponding current and voltage values to the output ports.
     */
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
/**
 * @class Harvester_${harvester_name}_battery_voc
 * @brief Class modeling the open-circuit voltage and series resistance of a battery-based harvester.
 */
SCA_TDF_MODULE(Harvester_${harvester_name}_battery_voc)
{
    sca_tdf::sca_in<double> i; ///< Battery current
    sca_tdf::sca_out<double> v_oc; ///< Voltage open-circuit
    sca_tdf::sca_out<double> r_s; ///< Series resistance
    sca_tdf::sca_out<double> soc ; ///< State of Charge

    /**
     * @brief Constructor for Harvester_${harvester_name}_battery_voc.
     * 
     * Initializes the module with the given parameters. The initial state of charge is set to the value specified in the configuration file, while the previous battery current is set to zero, as there is no previous current at the start of the simulation.
     */
    SCA_CTOR(Harvester_${harvester_name}_battery_voc): v_oc("v_oc"),
                                             r_s("r_s"),
                                             soc("soc"),
                                             tmpsoc((${soc_init/100})),
                                             prev_i_batt(0) {}

    void set_attributes();
    void initialize();
    void processing();
    
    private:
	    int c_nom=${capacity}; ///< Nominal battery capacity.
        double tmpsoc=${soc_init/100}f; ///< Initial state of charge.
        double prev_i_batt; ///< Previous battery current.
};


/**
 * @class Harvester_${harvester_name}_battery_char
 * @brief Class for characterizing the battery harvester including voltage and current handling.
 */
SC_MODULE(Harvester_${harvester_name}_battery_char)
{
    // Interface and internal components declaration
    sca_tdf::sca_in<double> r_s, ///< Internal resistance
                            i, ///< Battery current
                            v_oc; ///< Open-circuit voltage
    sca_tdf::sca_out<double> v; // Battery voltage

    sca_eln::sca_tdf::sca_isource* I_batt; ///< Battery current source
    sca_eln::sca_tdf::sca_vsink* V_batt; ///< Battery voltage sink

    sca_eln::sca_tdf_vsource* V_oc; ///< Open-circuit voltage source
    sca_eln::sca_node n1, n2; ///< Electrical nodes
    sca_eln::sca_node_ref gnd; ///< Ground reference
    sca_eln::sca_tdf::sca_r* R_s; ///< Series resistance element

    /**
     * @brief Constructor for Harvester_${harvester_name}_battery_char.
     * 
     * The structure of the battery is the first order equivalent circuit of a battery, consisting of an open-circuit voltage source, a series resistance, and a load current source. It can be drawn as:
     * 
     *                    I_batt
     * n1 ---------- R_s ---->----- n2
     * |                           |
     * |                           |
     * |                           |
     * V_oc                        V_batt
     * |                           |   
     * |                           |
     * |---------------------------|
     *               |
     *               |
     *              Gnd
     */
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

/**
 * @class Harvester_${harvester_name}
 * @brief Harvester class integrating both open-circuit voltage and battery characterization. This is the top-level module for the harvester, which connects the battery voltage and current to the battery characterization components. This is the one that should be instantiated in the overall system (main.cpp).
 */
SC_MODULE(Harvester_${harvester_name})
{
    // Interface and internal components declaration
    sca_tdf::sca_in<double> i; ///< Battery current
    sca_tdf::sca_out<double> v; ///< Battery voltage
    sca_tdf::sca_out<double> soc; ///< Battery SOC

    // Connecting signals
    sca_tdf::sca_signal<double> v_oc, r_s;

    // Instantiation of battery componenets
    Harvester_${harvester_name}_battery_voc* voc_module; ///< Open-circuit voltage module
    Harvester_${harvester_name}_battery_char* char_module; ///< Battery characterization module

    /**
     * @brief Constructor for Harvester_${harvester_name}.
     */
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
% endif