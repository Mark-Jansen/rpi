#include "Ultrasoon.h"
 
  Ultrasoon::Ultrasoon(int triggerPin, int echoPin)
  : mSensor( "/dev/ultrasoon", O_RDWR )
  {  
  }
  
  Ultrasoon::~Ultrasoon() 
  {
  }

  void Ultrasoon::start(void)
  {
  }
  
  void Ultrasoon::stop(void)
  {
  }
  
  int Ultrasoon::getDistance(void)
  {
  }
  

