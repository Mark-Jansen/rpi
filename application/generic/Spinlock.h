#ifndef GENERIC_SPINLOCK_H
#define GENERIC_SPINLOCK_H

#include <atomic>

class SpinLock
{
public:
	void lock()
	{
		while(lck.test_and_set(std::memory_order_acquire))
		{}
	}

	void unlock()
	{
		lck.clear(std::memory_order_release);
	}
 
private:
	std::atomic_flag lck = ATOMIC_FLAG_INIT;
};

class Lock
{
public:
	Lock( SpinLock* lock ) : mLock(lock)
	{
		mLock->lock();
	}
	
	~Lock()
	{
		mLock->unlock();
	}
private:
	SpinLock* mLock;
};



#endif // GENERIC_SPINLOCK_H
