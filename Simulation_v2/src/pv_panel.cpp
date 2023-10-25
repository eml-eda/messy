#include "pv_panel.h"

void pv_panel::set_attributes() {
    i.set_timestep(1, sc_core::SC_SEC);
    v.set_timestep(1, sc_core::SC_SEC);
}

void pv_panel::initialize() {
    // Open file with measurements of irradiance
    top.open("input_files/gmonths.txt");
    if (!top) {
        cout << "couldn't open irradiance file.\n"
             << endl;
        exit(-1);
    }

    cnt = 0;
}

void pv_panel::processing() {
    // sensing of irradiance takes place every TRACE_PERIOD s.
    if (cnt % TRACE_PERIOD == 0) {
        top >> g_top;
    }

    if (g_top <= 0) {
        i.write(0);
        v.write(0);
    } else {
        i.write(lut_i.get_val(g_top));
        v.write(lut_v.get_val(g_top));
    }
    cnt++;
}
