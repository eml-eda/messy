#include "systemc-ams.h"

SCA_TDF_MODULE (battery_peukert)
{
  sca_tdf::sca_in<double> in;// Ib
  sca_tdf::sca_out<double> out,out2,out3;// Vbatt,Soc,Lt
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
