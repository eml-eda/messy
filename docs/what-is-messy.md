MESSY is an open-source framework that integrates functional RISC-V simulation (achieved with GVSoC for the PULP cores) with SystemC-AMS (used to model extra-functional aspects, in detail power storage and distribution). The combination of GVSoC and SystemC-AMS in a single simulation framework allows to perform a DSE that is dependent on the mutual impact between functional and extra-functional aspects. 

In simpler words, it allows the user to simulate a complete system that contains both the CPU and the sensors, batteries etc. This is useful to understand how the system behaves in different conditions and to optimize it. The modularity of MESSY allows to define the system using a JSON configuration file. 
Thanks to SystemC-AMS, it is possible to model the power consumption of each component of the system. 

A brief view of the system that can be generated thanks to MESSY is shown in the following image: 

