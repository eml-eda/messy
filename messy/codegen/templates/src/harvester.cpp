#include <harvester_${harvester_name}.hpp>
% if harvester_type!="battery":
/**
 * @brief Sets the attributes of the Harvester module.
 * 
 * This function sets the timestep for the output current (i) and voltage (v) signals.
 */
void Harvester_${harvester_name}::set_attributes()
{
    i.set_timestep(1, SIM_RESOLUTION);
    v.set_timestep(1, SIM_RESOLUTION);
}

/**
 * @brief Initializes the Harvester module.
 * 
 * This function opens the input files for iref and vref if they are defined.
 */
void Harvester_${harvester_name}::initialize()
{   
    % if vref and type(vref)==dict:
    // Open file with simulation
    vref_input_file.open("${vref['input_file']}");
    if(!vref_input_file)
    {
        cout << "couldn't open ${vref['input_file']} file.\n" << endl;
        exit(-1);	
    }
    % endif

    % if iref and type(iref)==dict:
    // Open file with simulation
    iref_input_file.open("${iref['input_file']}");
    if(!iref_input_file)
    {
        cout << "couldn't open ${iref['input_file']} file.\n" << endl;
        exit(-1);	
    }
    % endif
}

/**
 * @brief Processes the Harvester module's functionality.
 * 
 * This function reads the input values for iref and vref from their respective files
 * and writes the corresponding current and voltage values to the output ports.
 */
void Harvester_${harvester_name}::processing()
{
    // sensing of irradiance takes place every TRACE_PERIOD s.
    % if vref and type(vref)==dict:
    double vref_input=0.0;
    vref_input_file >> vref_input;
    v.write(lut_v.get_val(vref_input));
    % endif

    % if iref and type(iref)==dict:
    double iref_input=0.0;
    iref_input_file >> iref_input;
    i.write(lut_i.get_val(iref_input));
    % endif

}
% else:
/**
 * @warning This function is called by the SystemC kernel before the simulation starts. It should not be called by the user.
 * 
 * @brief Sets the attributes of the battery-based harvester.
 * 
 * This function configures the timestep and delay for the open-circuit voltage (v_oc), series resistance (r_s), and state of charge (soc) outputs.
 */
void Harvester_${harvester_name}_battery_voc::set_attributes()
{
    v_oc.set_timestep(1, SIM_RESOLUTION);
    r_s.set_timestep(1, SIM_RESOLUTION);
    soc.set_timestep(1, SIM_RESOLUTION);
    r_s.set_delay(1);
    soc.set_delay(1);
}

/**
 * @warning This function is called by the SystemC kernel before the simulation starts. It should not be called by the user.
 * 
 * @brief Initializes the battery harvester.
 * 
 * This function initializes the temporary state of charge (tmpsoc) based on the provided initial SOC value.
 */
void Harvester_${harvester_name}_battery_voc::initialize() {
    tmpsoc = ${soc_init/100}f;
}

/**
 * @warning This function is called by the SystemC kernel during the simulation. It should not be called by the user.
 * 
 * @brief Processing method for the battery-based harvester.
 * 
 * This function computes the state of charge (SOC), battery open-circuit voltage (Voc),
 * and series resistance (R_s) based on the current and SOC relationships. If SOC drops below
 * a threshold, the simulation is stopped.
 */
void Harvester_${harvester_name}_battery_voc::processing(){
    double tmpcurrent = i.read(); ///< Read the battery current. If negative, the battery is charging.

    /** 
    * Compute actual state-of-charge solving the integral:
    * SOC_t = SOC_{t-1} - \int^{t}_{-inf} i(\tau) / C d\tau
    */
    // TODO: Quickfix for scaling resulution
    int scaling_factor_resulution;
    if (SIM_RESOLUTION == SC_MS)
    {
        // MS
        scaling_factor_resulution = 1000;
    }
    else
    {
        // SEC
        scaling_factor_resulution = 1;
    }
    tmpsoc -= (((tmpcurrent + prev_i_batt) * 1) / (2 * (3600) * c_nom * scaling_factor_resulution));
    prev_i_batt = tmpcurrent; // Update

    // Output the battery SOC
    if(tmpsoc >= 1) // Not let the SOC overflow
    {
        soc.write(1); ///< Since the battery is full, the SOC is set to 1.
        tmpsoc = 1;
    }
    else
    {    
        soc.write(tmpsoc); ///< Output the current SOC
    }

    // SOC and battery Voc relationship
    // val(x) = p1*x^4 + p2*x^3 + p3*x^2 + p4*x + p5
    double v_p1=-13.26, v_p2=27.4, v_p3=-17.99, v_p4=4.496, v_p5=3.45;
    v_oc.write((v_p1 * tmpsoc * tmpsoc * tmpsoc * tmpsoc) + (v_p2 * tmpsoc * tmpsoc * tmpsoc) + (v_p3 * tmpsoc * tmpsoc) + (v_p4 * tmpsoc) + v_p5); // Place interpolated funct here

    // SOC and battery internal resistance relationship
    // val(x) = p1*x^4 + p2*x^3 + p3*x^2 + p4*x + p5
    double r_p1=-0.1055, r_p2=0.1968, r_p3=-0.1119, r_p4=0.01607, r_p5=0.003117;
    r_s.write((r_p1 * tmpsoc * tmpsoc * tmpsoc * tmpsoc) + (r_p2 * tmpsoc * tmpsoc * tmpsoc) + (r_p3 * tmpsoc * tmpsoc) + (r_p4 * tmpsoc) + r_p5); // Place interpolated funct here

    // When the battery SOC decreases under 5% of starting voc, the simulation stops.	
    if(tmpsoc <= ${soc_end/100})
    {
        cout << "SOC is less than or equal to {{starting_voc-perc_voc}}/100%:" << " @" << sc_time_stamp() << " ; it is "<< tmpsoc << endl;
        sc_stop();
    }
}
% endif
