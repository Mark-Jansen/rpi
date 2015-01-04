#ifndef BUTTON_H
#define BUTTON_H

#include <generic/File.h>
#include <generic/Thread.h>

class Button : public Thread
{
public:
	Button( int pin );

	bool isDown();
	bool wasPressed();

protected:

	int delayMS() const;
	virtual void onBeforeRun();
	virtual void onRun();
private:
	File mSensor;
	int mPin;
	bool mDown;
	bool mWasDown;
};


#endif // BUTTON_H
