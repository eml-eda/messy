#include <power_bus.hpp>

void Power_bus::set_attributes() {}

void Power_bus::initialize() {}

/**
 * @warning This function should not be called directly. It is called by the SystemC-AMS kernel.
 *
 * @brief This function calculates and updates the total current on the power bus.
 *
 * It reads the current values from the sensors, the core, and the sources.
 * It sums the current from all sensors and core, and then subtracts the
 * current from the power sources. If there are any batteries, the calculated
 * total current is divided equally among the batteries and written to each
 * battery.
 *
 * The function performs the following steps:
 * - Initializes the `total_current` to 0.
 * - Loops through the current sensors to read and accumulate their current values.
 * - Reads the current from the core and adds it to `total_current`.
 * - If there are any current sources, their values are read and subtracted from `total_current`. The current sources are assumed to be supplying current to the system, so they are subtracted from the total current.
 * - If there are any batteries, the total current is divided by the number of batteries,
 *   and the value is written to each battery, representing the current drawn from each battery.
 *
 * @note The number of sensors, sources, and batteries is determined by the constants
 * `NUM_SENSORS`, `NUM_SOURCES`, and `NUM_BATTERIES` respectively.
 *
 * @warning If `NUM_SOURCES` or `NUM_BATTERIES` is 0, the respective section of code
 * will not be executed.
 *
 */
void Power_bus::processing() {
    double tmp_i;
    _total_current = 0;

    for (int i = 0; i < NUM_SENSORS; i++) {
        tmp_i = i_current_sensors_a[i].read();
        _total_current += tmp_i;
    }

    tmp_i = i_current_core_a.read();

    _total_current += tmp_i;

#if NUM_SOURCES > 0
    for (int i = 0; i < NUM_SOURCES; i++) {
        tmp_i = i_current_sources_a[i].read();
        _total_current -= tmp_i;
    }
#endif

#if NUM_BATTERIES > 0
    for (int i = 0; i < NUM_BATTERIES; i++)
        i_current_batteries_a[i].write(_total_current / NUM_BATTERIES);
#endif
}