#include "core.h"

void Core::Simulation() {
    while (1) {
        A_Out.write(101);
        D_Out.write(1);
        F_Out.write(true);
        Ready.write(true);
        wait();
        Ready.write(false);
        wait();
        //        std::cout << "First Instruction " << sc_time_stamp() << endl;
        //        std::cout << "Read Status Register from Temperature sensor: " << Data_in.read() << endl;
        //        wait(1, sc_core::SC_SEC);

        A_Out.write(201);
        D_Out.write(1);
        F_Out.write(true);
        Ready.write(true);
        wait();
        Ready.write(false);
        wait();
        //        std::cout << "Second Instruction " << sc_time_stamp() << endl;
        //        std::cout << "Read Status Register from Temperature sensor: " << Data_in.read() << endl;
        //        wait(1, sc_core::SC_SEC);

        A_Out.write(301);
        D_Out.write(1);
        F_Out.write(true);
        Ready.write(true);
        wait();
        Ready.write(false);
        wait();
        //        std::cout << "Third Instruction " << sc_time_stamp() << endl;
        //        std::cout << "Read Status Register from Temperature sensor: " << Data_in.read() << endl;
        //        wait(1, sc_core::SC_SEC);

        A_Out.write(401);
        D_Out.write(1);
        F_Out.write(true);
        Ready.write(true);
        wait();
        Ready.write(false);
        wait();
        //        std::cout << "Fourth Instruction " << sc_time_stamp() << endl;
        //        std::cout << "Read Status Register from Temperature sensor: " << Data_in.read() << endl;
        //        wait(1, sc_core::SC_SEC);

        power_signal.write(1);
        wait(CPU_ON_T_ON, sc_core::SC_SEC);
        power_signal.write(3);

        A_Out.write(501);
        D_Out.write(1);
        F_Out.write(true);
        Ready.write(true);
        wait();
        Ready.write(false);
        wait();

        wait(12, sc_core::SC_SEC);
        //        std::cout << "Fifth Instruction " << sc_time_stamp() << endl;
        //        std::cout << "Read Status Register from Temperature sensor: " << Data_in.read() << endl;
        //        wait(1, sc_core::SC_SEC);
    }
}