#include "battery_peukert.h"
#include "time.h"

void battery_peukert::set_attributes() {
    v_batt.set_timestep(1, sc_core::SC_SEC);

    soc.set_timestep(1, sc_core::SC_SEC);
}

void battery_peukert::initialize() {
    // v_batt.initialize(4.2);
    k = 1.1;
    Vnom = 4.2;
}

// Process to calculate the Soc and the current LifeTime extimation
void battery_peukert::processing() {
    double tmp_in;
    double tmp_lt;
    double t;
    sc_time time;
    tmp_in = i_batt.read();

    time = sc_time_stamp();

    t = time.to_seconds();

    if (t == 0) {
        tmp_lt = 0;

    } else {

        C = C * pow((Iprev / tmp_in), k - 1);
        if (Iprev == 0)
            C = 20800;
        tmp_lt = C / (pow(tmp_in, k));
    }
    soc_tmp = soc_tmp - ((tmp_in) / 20800);

    if (soc_tmp <= 0) {

        cout << "\nSoc is less then or equal to zero: " << soc_tmp << " @" << sc_time_stamp() << endl;
        sc_stop();
    }

    // Cprev=C;
    Iprev = i_batt.read();

    v_batt.write(4.2);

    soc.write(soc_tmp);
}
