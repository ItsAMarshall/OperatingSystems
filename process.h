#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <iostream>

class Process
{
	public:
		Process() { }
		Process(char procNum_, int arrivalTime_, int burstTime_, int burstCount_, int ioTime_, int memorySize_) : procNum( procNum_ ), arrivalTime( arrivalTime_ ), burstTime( burstTime_ ), burstCount( burstCount_ ), ioTime( ioTime_), memorySize( memorySize_ ) {	
				inMemory = false;
				cpuTimer = 0;
				timeWaiting = 0;
				preempted = false;
		}
		Process( const Process& other ) : procNum( other.procNum ), arrivalTime( other.arrivalTime), burstTime( other.burstTime ), burstCount( other.burstCount ), ioTime( other.ioTime), memorySize( other.memorySize), ioTimeEnd( other.ioTimeEnd ), cpuTimer( other.cpuTimer), preempted(other.preempted), inMemory(other.inMemory), turnAroundStart(other.turnAroundStart) { 
			timeWaiting = 0;
		}
		~Process() { }

		int const getSize() const {
		  return memorySize;
		}

		bool hasArrived(int timer) {
			return (arrivalTime == timer);
		}

		char procNum;
		int arrivalTime;
		int burstTime;
		int burstCount;
		int ioTime;
		int memorySize;

		int ioTimeEnd;
		int cpuTimer;
		int timeWaiting;
		bool preempted;
		bool inMemory;

		int turnAroundStart;
		int turnAroundTotal;
};

//Used for Priority Queue
bool operator < (const Process &a, const Process &b) {
	if( a.ioTimeEnd < b.ioTimeEnd ) {
		return false;
	}
	else if( a.ioTimeEnd == b.ioTimeEnd ) {
		return a.procNum > b.procNum;
	}
	return true;
}
#endif