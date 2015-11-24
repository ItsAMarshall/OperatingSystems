#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <iostream>

class Process
{
	public:
		Process() { }
		Process(char procNum_, int arrivalTime_, int burstTime_, int burstCount_, int ioTime_, int memory_) : procNum( procNum_ ), arrivalTime( arrivalTime_ ), burstTime( burstTime_ ), burstCount( burstCount_ ), ioTime( ioTime_), memory( memory_ ) {	
				cpuTimer = 0;
				timeWaiting = 0;
				preempted = false;
		}
		Process( const Process& other ) : procNum( other.procNum ), arrivalTime( other.arrivalTime), burstTime( other.burstTime ), burstCount( other.burstCount ), ioTime( other.ioTime), ioTimeEnd( other.ioTimeEnd ), memory( other.memory), cpuTimer( other.cpuTimer), preempted(other.preempted) { 
			timeWaiting = 0;
		}
		~Process() { }

		int const getSize() const {
		  return memory;
		}

		bool hasArrived(int timer) {

			return (arrivalTime == timer);
		  // bool arrival = false;
		  // for (int i = 0; i < static_cast<int>(this->arrivalTimes_.size()); ++i) {
		  //   if (this->arrivalTimes_[i] == timer) {
		  //     arrival = true;
		  //     break;
		  //   }
		  // }

		  // if (arrival) this->isActive_ = true;
		  // return arrival;
		}

		char procNum;
		int arrivalTime;
		int burstTime;
		int burstCount;
		int ioTime;
		int memory;

		int ioTimeEnd;
		int cpuTimer;
		int timeWaiting;
		bool preempted;
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