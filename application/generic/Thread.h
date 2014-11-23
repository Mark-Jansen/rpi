#ifndef THREAD_H
#define THREAD_H

#include <thread>
#include <mutex>
#include <condition_variable>

class Thread
{
public:
	Thread();
	virtual ~Thread();
	void start();
	void stop();

protected:

	virtual int delayMS() const = 0;
	virtual void onBeforeRun() {;}
	virtual void onRun() = 0;

private:
	void run();

	std::thread* mThread;
	std::condition_variable mEvent;
	std::mutex mMutex;
	volatile bool mRun;
};

#endif // THREAD_H
