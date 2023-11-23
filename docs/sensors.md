# Sensors

Sensors are important components for this simulator. Every sensor can be composed by two separate instances:

- **Functional**: this instance is responsible for define the internal characteristics of the sensor. This instance interfaces with the core through a bus
- **Power**: this instance controls the state of the sensor and exposes its current and voltage

The functional part retrieves details of the request, such as the address, read/write flag, and data from the core. Its logic consists of a structure of if statements where it queries an array of elements (simulated internal registers of the sensor), sets the correct power state, and waits. After this step, the power instance is activated, which updates the power information for a certain amount of time defined by the sensor’s configuration. This is done to simulate the computational time that the sensor takes in real life to analyze the data and update the corresponding register. After that, the power instance sets an IDLE state and gives control back to the functional instance, which can then prepare Data OUT and GO signals to reply to the core.

Here you can see a really simple example of a sensor:

```c++
void air_quality_sensor_functional::sensor_logic(){
    while (true)
    {
        if( enable.read() == true ){
            if(ready.read() == true){
                if( flag_wr.read() == true ){
                    //Read Operations   
                    //int rnd = 1 + (rand() % 100);
                    //data_out.write(rnd);
                    data_out.write(Register[address.read()]);
                    power_signal.write(1);
                    wait(AIR_QUALITY_SENSOR_T_ON_READ,SIM_RESOLUTION);
                    power_signal.write(3);
                    go.write(true);
                } else {
                    //Write Operations
                    Register[address.read()] = data_in.read();
                    data_out.write(data_in.read());
                    power_signal.write(2);
                    wait(AIR_QUALITY_SENSOR_T_ON_WRITE,SIM_RESOLUTION);
                    power_signal.write(3);
                    go.write(true);
                }
            } else {
                go.write(false);
            }
        } else {
            power_signal.write(0);
        }
        wait();
    }   
}
```
As you can see there are two if for the read and write operations. In each if statement, the correct power state is set (`IR_QUALITY_SENSOR_T_ON_READ` for reading, `AIR_QUALITY_SENSOR_T_ON_WRITE` for writing). Setting the value of the power signal to 1, means that the power instance of the sensor will return on the voltage and current buses the value of the current and voltage of the sensor in the reading state. 

```c++
#include "air_quality_sensor_power.h"

void air_quality_sensor_power::set_attributes()
{
    func_signal.set_rate(1);
    func_signal.set_timestep(1, SIM_RESOLUTION);
    //power_to_therm.set_timestep(0.5, SIM_RESOLUTION);
    //power_to_therm.set_rate(1);
    //power_to_therm.set_delay(1);
}

void air_quality_sensor_power::initialize() {}

void air_quality_sensor_power::processing()
{
    if(func_signal.read() == 1 ){
        //std::cout << "air_quality_sensor in ON_READ state!" << std::endl;
        voltage_state.write(AIR_QUALITY_SENSOR_VOLTAGE);
        current_state.write(AIR_QUALITY_SENSOR_I_ON_READ);
        //power_to_therm.write(5.0);
        return;
    }

    if(func_signal.read() == 2 ){
        //std::cout << "air_quality_sensor in ON_WRITE state!" << std::endl;
        voltage_state.write(AIR_QUALITY_SENSOR_VOLTAGE);
        current_state.write(AIR_QUALITY_SENSOR_I_ON_WRITE);
        //power_to_therm.write(5.0);
        return;
    }

    if(func_signal.read() == 3 ){
        //std::cout << "air_quality_sensor in IDLE state!" << std::endl;
        voltage_state.write(AIR_QUALITY_SENSOR_VOLTAGE);
        current_state.write(AIR_QUALITY_SENSOR_I_IDLE);
        //power_to_therm.write(5.0);
        return;
    }

    if(func_signal.read() == 0){
        //std::cout << "air_quality_sensor in OFF state" << std::endl;
        voltage_state.write(0.0);
        current_state.write(0.0);
        //power_to_therm.write(0.0);
        return;
    }

    std::cout << "air_quality_sensor in an Unknown state!" << std::endl;
    //power_to_therm.write(0.0);       
}
```

As you can see here, the execution will go through the first if statement, since the value on the func_signal (that is the power_signal) is 1, so it will set the correct voltage and current values. Then, going back to the functional instance, we notice that the power_signal is set to 3, that means that the power instance of the sensor will return the voltage and current values of the IDLE state.

# Supported sensors

At the moment, the simulator has been tested with the following sensors:

- **Air Quality Sensor**: this sensor is a simple sensor that can be used to measure the air quality
- **Temperature Sensor**: this sensor is a simple sensor that can be used to measure the temperature
- **Methane Sensor**: this sensor is a simple sensor that can be used to measure the methane
- **RF Sensor**: this sensor is a simple sensor that can be used to transmit and receive data
- **Microphone Sensor**: this sensor is a simple sensor that can be used to measure the sound

Ideally, any sensors can be modeled and simulated with this simulator. 