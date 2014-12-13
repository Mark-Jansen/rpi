#ifndef APPLICATION_ULTRASOON_CONTROLLER_H
#define APPLICATION_ULTRASOON_CONTROLLER_H

#include "generic/File.h"
#include "generic/Thread.h"

class Ultrasoon_controller : public Thread
{
public:

	Ultrasoon_controller(void);
	virtual ~Ultrasoon_controller();

protected:

	int delayMS() const;
	virtual void onBeforeRun();
	virtual void onRun();


};


