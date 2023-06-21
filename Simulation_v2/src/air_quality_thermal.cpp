#include "air_quality_thermal.h"

void air_quality_therm::set_attributes()
{
    input.set_rate(1);
    therm_out.set_timestep(100.0, sc_core::SC_MS);
}

void air_quality_therm::initialize() {}

void air_quality_therm::processing()
{
    if(input.read() == 5.0){
        std::cout << "Air quality is heating" << std::endl;
        therm_out.write(10.0);
    } else {

        if(input.read() == 0.0){
            std::cout << "Air quality is not heating" << std::endl;
            therm_out.write(0.0);
        } else {
            std::cout << "Air quality in an Unknown therm state!" << std::endl;
            therm_out.write(0.0);
        }
    }
}