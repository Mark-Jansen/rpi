#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

class Motor;

enum class Direction
{
	None,
	Forward,
	Backward
};

class MotorController
{
public:
	MotorController();
	~MotorController();
	
	void setSpeed( int speed );
	void setDirection( Direction dir );
	// TODO: add controls to turn, and increase left/right speed as well as direction

private:
	Motor* mLeft;
	Motor* mRight;
};

#endif // MOTORCONTROLLER_H
