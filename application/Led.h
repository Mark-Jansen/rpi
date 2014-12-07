#ifndef APPLICATION_LED_H
#define APPLICATION_LED_H

#include "generic/File.h"

class Led
{
  public:
  Led(int pinNr);
  ~Led();
  void on(void);
  void off(void);
  void blink(void);
  int getStatus(void);

private:
  File mSensor;
  int status;
  int pinNr;
};

#endif // APPLICATION_LED_H
