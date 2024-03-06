#include "${name}_converter.h"


void ${name}_converter::set_attributes()
{
    i_out.set_timestep(1, SIM_RESOLUTION);
    // i_out.set_delay(1);
}

void ${name}_converter::initialize() {}

void ${name}_converter::processing()
{
    double ideal_power;
    double eta; // Efficiency
    double i_tmp;
    double v_tmp;

    // Read input quantities
    i_tmp = i_in.read();
    v_tmp = v_in.read();

    // Get efficiency
    eta = lut_eta.get_val(${"v_tmp" if input_variable=="voltage" else "i_tmp"}) / 100;
    
    % if not out_dir:
    i_out.write(i_tmp * v_tmp / (VREF_BUS*eta));
    % else:
    i_out.write((eta * (i_tmp * VREF_BUS)) / v_tmp);
    % endif
}
