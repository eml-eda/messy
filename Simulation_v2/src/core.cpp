#include "core.h"

void Core::Simulation(){

    A_Out.write(101);
    D_Out.write(2);
    F_Out.write(true);
    wait();
    //std::cout << "First Instruction: " << sc_time_stamp() << endl;
    //std::cout << "Read Status Register from Air Quality sensor: " << Data_in.read() << endl;
    wait(1, sc_core::SC_SEC);

    A_Out.write(103);
    D_Out.write(1);
    F_Out.write(true);
    wait();
    //std::cout << "Second Instruction " << sc_time_stamp() << endl;
    //std::cout << "Read Air Measurement: " << Data_in.read() << endl;
    wait(1, sc_core::SC_SEC);

    A_Out.write(101);
    D_Out.write(0);
    F_Out.write(false);
    wait();
    //std::cout << "Third Instruction " << sc_time_stamp() << endl;
    //std::cout << "Cleand value in Air Quality Status Register: " << Data_in.read() << endl;
    wait(1, sc_core::SC_SEC);

    A_Out.write(201);
    D_Out.write(1);
    F_Out.write(true);
    wait();
    //std::cout << "Fourth Instruction " << sc_time_stamp() << endl;
    //std::cout << "Read Status Register from Temperature sensor: " << Data_in.read() << endl;
    wait(1, sc_core::SC_SEC);
        
    A_Out.write(208);
    D_Out.write(83);
    F_Out.write(false);
    wait();
    //std::cout << "Fifth Instruction " << sc_time_stamp() << endl;
    //std::cout << "Write Temperature Reg[8]: " << Data_in.read() << endl;
    wait(1, sc_core::SC_SEC);
        
    A_Out.write(203);
    D_Out.write(10);
    F_Out.write(true);
    wait();
    //std::cout << "Sixth Instruction " << sc_time_stamp() << endl;
    //std::cout << "Read Temperature Measurement: " << Data_in.read() << endl;

    //A_Out.write(300);
}