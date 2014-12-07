#include "Battery.h"
 
  Battery::Battery(void)
  : mSensor( "/dev/battery", O_RDWR )
  {  
  }
  
  Battery::~Battery() 
  {
  }

  void Battery::start(void)
  {
  }
  
  void Battery::stop(void)
  {
  }
  
  int Battery::getCharge(void)
  {
  }
  

