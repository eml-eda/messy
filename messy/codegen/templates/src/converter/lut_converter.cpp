#include <converter/${name}_converter.hpp>


void ${name}_converter::set_attributes()
{
    current_out.set_timestep(1, SIM_RESOLUTION);
}

void ${name}_converter::initialize() {}

void ${name}_converter::processing()
{
    double ideal_power;
    double eta; // Efficiency
    double i_tmp;
    double v_tmp;

    // Read input quantities
    i_tmp = current_in.read();
    v_tmp = voltage_in.read();

    // Get efficiency
    eta = lut_eta.get_val(${"v_tmp" if input_variable=="voltage" else "i_tmp"}) / 100;
    
    % if not out_dir:
    current_out.write(i_tmp * v_tmp / (VREF_BUS*eta));
    % else:
    current_out.write((eta * (i_tmp * VREF_BUS)) / v_tmp);
    % endif
}
