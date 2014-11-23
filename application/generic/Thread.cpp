#include "Thread.h"

Thread::Thread()
: mThread( 0 )
, mRun( false )
{
}

Thread::~Thread()
{
	stop();
}

void Thread::start()
{
	if( !mRun ) {
		mRun = true;
		if( mThread ) {
			delete mThread;
		}
		mThread = new std::thread( &Thread::run, this );
	}
}

void Thread::stop()
{
	if( mRun ) {
		mRun = false;
		if( mThread->get_id() != std::this_thread::get_id() ) {
			mEvent.notify_all();
			mThread->join();
		}
	}
}

void Thread::run()
{
	onBeforeRun();
	if( mRun ) {
		std::unique_lock<std::mutex> lck( mMutex );
		while( mRun ) {
			onRun();
			mEvent.wait_for( lck, std::chrono::milliseconds( delayMS() ) );	// == std::cv_status::timeout
		};
	}
}
