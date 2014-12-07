#ifndef APPLICATION_MOTOR_H
#define APPLICATION_MOTOR_H

#include "generic/File.h"

class Motor
{
  public:
  Motor(void);
  ~Motor();
  void start(void);
  void stop(void);

private:
  File mSensor;
};

#endif // APPLICATION_MOTOR_H
