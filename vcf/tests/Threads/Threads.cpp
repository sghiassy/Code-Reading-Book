// Threads.cpp : Defines the entry point for the console application.
//


#include "FoundationKit.h"
#include "Thread.h"
#include <iostream>

using namespace VCF;
using namespace std;

class MyThread : public Thread {
public:
	MyThread() : Thread(NULL,false){};
	virtual ~MyThread(){};

	virtual bool run(){
		Thread::run();
		cout << "inside of MyThread::notifyOnRunning()" << endl;
		int c = 200;
		int id = this->getThreadID();
		for (int i = 0;i<c;i++ ){
			if ( this->canContinue() ) {
				cout << "MyThread::notifyOnRunning() threadID " << id << "\"i\" = " << i << endl;
				//sleep( 50 );
			}
		}
		return true;
	}
};

#define THREAD_COUNT		3

int main(int argc, char* argv[])
{

	cout << "Hello " << endl;

	MyThread* threads[THREAD_COUNT];
	for ( int i=0;i<THREAD_COUNT;i++){
		threads[i] = new MyThread();
		threads[i]->start();
	}

	MyThread* thread = new MyThread();

	cout << "thread will start..." << endl;

	thread->start();

	String s;
	
	cin >> s;	

	delete thread;
	cout << "Quitting" << endl;

	for ( i=0;i<THREAD_COUNT;i++){		
		delete threads[i];
	}

	cout << "All threads destroyed" << endl;
	cin >> s;

	return 0;
}

