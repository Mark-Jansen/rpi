#ifndef APPLICATION_ULTRASOON_CONTROLLER_H
#define APPLICATION_ULTRASOON_CONTROLLER_H

#include "generic/File.h"
#include "generic/Thread.h"
class Ultrasoon;

class Ultrasoon_controller : public Thread
{
public:
	Ultrasoon_controller();
	virtual ~Ultrasoon_controller();

protected:

	int delayMS() const;
	virtual void onBeforeRun();
	virtual void onRun();

private:
	Ultrasoon* mFront;
	Ultrasoon* mBack;
};

#endif // APPLICATION_ULTRASOON_CONTROLLER_H


