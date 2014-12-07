#ifndef APPLICATION_BATTERY_H
#define APPLICATION_BATTERY_H

#include "generic/File.h"

class Battery
{
  public:
  Battery(void);
  ~Battery();
  void start(void);
  void stop(void);
  int getCharge(void);

private:	
  File mSensor;
  int charge;
};

#endif // APPLICATION_BATTERY_H


