#ifndef APPLICATION_ULTRASOON_H
#define APPLICATION_ULTRASOON_H

#include "generic/File.h"

class Ultrasoon
{
  public:
  Ultrasoon(int triggerPin, int echoPin);
  ~Ultrasoon();
  void start(void);
  void stop(void);
  int getDistance(void);

private:
  File mSensor;
  int distance;
  int triggerPin;
  int echoPin;
};

#endif // APPLICATION_ULTRASOON_H

