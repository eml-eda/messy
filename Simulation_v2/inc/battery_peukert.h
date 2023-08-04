#include "systemc-ams.h"

SCA_TDF_MODULE (battery_peukert)
{
  sca_tdf::sca_in  <double> in;  // Ib
  sca_tdf::sca_out <double> Vbatt; // Vbatt
  sca_tdf::sca_out <double> Soc;// Soc
  sca_tdf::sca_out <double> Lt;// Lt
  sc_signal<double> soc,C;
  
  double k,Vnom;
  double /*Cprev,*/Iprev;
  
	
  SCA_CTOR(battery_peukert) {
  soc=1.0;
  C=2.52;// 0.0007 Ah * 3600 s
  }

  void set_attributes();

  void initialize();

  void processing();

};
