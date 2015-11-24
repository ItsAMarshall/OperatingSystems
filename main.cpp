/*
	NOTE:

		Nameing Schemes:
			Functions: first letter of each word is capitalized
			variables: first letter of each word is capitalized except first word

	TODO:

		1) DONE Add process to queue when it is their arrival time
		2) Add Round Robin functionality 
		3) Incorporate Lucas's memory manager
		4) Print out memory block 
		5) Add process to memory block

*/



#include <algorithm>
#include <deque>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <vector>

using namespace std;

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

void PushBack( deque<Process>* cpuQueue, Process* proc, const string& mode ) {
	deque<Process>::iterator itr;
	proc->timeWaiting = 0;
	for( itr = cpuQueue->begin(); itr != cpuQueue->end(); ++itr ) {
		if ( mode == "SRT" ) {
			if( itr->burstTime > proc->burstTime ) {
				cpuQueue->insert(itr, *proc);
				return;
			}
		}
	}
	cpuQueue->push_back(*proc);
	return;
}

//Reads in Processes.txt, parses data and adds themto Queue
bool ReadFile(fstream & file, vector<Process>* processVector, const string& mode) {
	char c = '|';
	string s;
	while( getline(file, s) ) {
		if( s[s.find_first_not_of(' ')] == '#') {
			continue;
		}
		char procNum_;
		int arrivalTime_, burstTime_, burstCount_, ioTime_, memory_;
		stringstream temp(s);

		temp >> procNum_ >> c;
		temp >> arrivalTime_ >> c;
		temp >> burstTime_ >> c;
		temp >> burstCount_ >> c;
		temp >> ioTime_ >> c;
		temp >> memory_ >> c;

		Process proc(procNum_, arrivalTime_, burstTime_, burstCount_, ioTime_, memory_);
		processVector->push_back(proc);
	}
	return false;
}

//Used to output the time in correct format
void PrintTime(int timer) {
	cout << "time " << timer <<"ms: ";
	return;
}

//Used to output the prcoesses in the Queue in correct order
void PrintQueue( deque<Process>* cpuQueue) {
	cout << "[Q";
	for(unsigned int bin = 0; bin < cpuQueue->size(); ++bin) {
		//cout << " " << cpuQueue[0][bin].procNum;
		cout << " " << cpuQueue[0][bin].procNum;
	}
	cout << "]" << endl;
}

//USed to print memory block
void PrintMemory () {

}

//Used for debugging the I/O priority queue not used in final program
void PrintIOQueue( priority_queue<Process>* ioQueue) {
	int size = ioQueue->size();
	cout << "[IO";
	for( int bin = 0; bin < size; ++bin) {
		cout << " + " << ioQueue->top().procNum;
		ioQueue->pop();
	}
	cout << "]" << endl;
}

bool Preempt( Process* cpu, Process* proc, const string& mode ) {
	if( proc->burstTime < (cpu->burstTime - cpu->cpuTimer ) ) {
			cpu->preempted = true;
			return true;
	}
	return false;
}

//Removes Process from Queue and adds to "CPU" 
void LoadCPU( deque<Process>* cpuQueue, Process* proc, Process* cpu , int timer, int t_cs) {
	timer += t_cs;
	*cpu = *proc;
	if( cpu->preempted == false)
		cpu->cpuTimer = 0;
	PrintTime(timer);
	cout << "Process '" << cpu->procNum << "' started using the CPU ";
	//cout << "==END TIME: " << cpu->burstTime - cpu->cpuTimer + timer;
	PrintQueue(cpuQueue);

	return;
}

//Moves process from "CPU" to I/O priority queue
void LoadIO( Process* proc, deque<Process>* cpuQueue, priority_queue<Process>* ioQueue, int timer ) {
	proc->burstCount--;
	if( proc->burstCount == 0 ) {
		PrintTime(timer);
		cout << "Process '" << proc->procNum << "' terminated ";
		PrintQueue(cpuQueue);
		proc = NULL;
		return;
	}

	PrintTime(timer);
	cout << "Process '" << proc->procNum << "' completed its CPU burst ";
	PrintQueue(cpuQueue);

	PrintTime(timer);
	cout << "Process '" << proc->procNum << "' performing I/O ";
	PrintQueue(cpuQueue);
	proc->ioTimeEnd = proc->ioTime + timer;
	ioQueue->push(*proc);
	proc = NULL;
}

Process* CheckIO( priority_queue<Process>* ioQueue, deque<Process>* cpuQueue, Process* cpu, int timer, const string& mode ) {

	if( ioQueue->size() != 0 ) {
		if( ioQueue->top().ioTimeEnd <= timer ) {
			Process* temp = new Process(ioQueue->top());

			PrintTime(timer);

			//Still needs to use CPU
			if( temp->burstCount != 0 ) {
				return temp;
			}
		}
	}
	return NULL;
}

void SwapPreempt(deque<Process>* cpuQueue, Process* cpu, Process* preemptCatch, const string& mode, int timer, int t_cs) {
		PrintTime(timer);
		cout << "Process '" << cpu->procNum << "' preempted by P" << preemptCatch->procNum << " ";
		PushBack(cpuQueue, cpu, mode);
		PrintQueue(cpuQueue);
		//timer += t_cs;
		LoadCPU(cpuQueue, preemptCatch, cpu, timer, t_cs);		
}

void CheckArrival(vector<Process>* processVector, deque<Process>* cpuQueue, int timer, const string& mode) {
	vector<Process>::iterator itr = processVector->begin();

	while( itr != processVector->end() ) {
		if( itr->arrivalTime == timer ) {
			PushBack(cpuQueue, &(*itr), mode);

			PrintTime(timer);
			cout << "Process '" << itr->procNum << "' added to system";
			PrintQueue(cpuQueue);

			processVector->erase(itr);

		}
		else {
			++itr;
		}
	}
}

void Perform(vector<Process>* processVector, int t_cs, const string& mode) {
	static int timer = 0;
	timer = 0;
	Process* preemptCatch = NULL;
	deque<Process>* cpuQueue = new deque<Process>;
	priority_queue<Process>* ioQueue = new priority_queue<Process>;
	bool cpuInUse = false;
	Process* cpu = new Process();
	
	PrintTime(timer);
	cout << "Simulator started for " << mode << " ";
	PrintQueue(cpuQueue);
	
	//check priority of cpuqueue top
	//if less than cpu priority thats a preempt

	while( true ) {

		CheckArrival(processVector, cpuQueue, timer, mode);
		//increment wait times
		//if wait time / burstTime >= 3 increase priority.
		preemptCatch = NULL;
		if( cpuInUse && cpu != NULL) {
			if( cpu->cpuTimer == cpu->burstTime) {
				LoadIO( cpu, cpuQueue, ioQueue, timer );
				cpuInUse = false;

			}
		}
		preemptCatch = CheckIO( ioQueue, cpuQueue, cpu, timer, mode );
		if (preemptCatch != NULL) {

			cout << "Process '" << preemptCatch->procNum << "' completed I/O ";
			if( Preempt(cpu, preemptCatch, mode) ) {
				PrintQueue(cpuQueue);
				if( cpu->burstCount > 0){
					PushBack(cpuQueue, cpu, mode);
				
					PrintTime(timer);
					cout << "Process '" << cpu->procNum << "' preempted by P" << preemptCatch->procNum << " ";
					PrintQueue(cpuQueue);
				}
				
				LoadCPU(cpuQueue, preemptCatch, cpu, timer, t_cs);
				cpuInUse = true;
			}
			else {
				preemptCatch->preempted = false;
				PushBack(cpuQueue, preemptCatch, mode);
				PrintQueue(cpuQueue);
			}

			ioQueue->pop();
		}

		//Add next process to CPU
		if( !cpuInUse && cpuQueue->size() != 0 ) {
			//timer += t_cs;
			preemptCatch = &cpuQueue->front();
			//cout << preemptCatch->cpuTimer << endl;
			cpuQueue->pop_front();
			//cout << preemptCatch->cpuTimer << endl;
			LoadCPU( cpuQueue, preemptCatch, cpu, timer, t_cs);
			//cout << preemptCatch->cpuTimer << endl;
			cpuInUse = true;

		}

		//No more processes to run
		if( !cpuInUse && cpuQueue->size() == 0 && ioQueue->size() == 0
		) {     
			PrintTime(timer);     
			cout << "Simulator for " << mode << " ended ";
			PrintQueue(cpuQueue);     
			return; 
		}
		
		++timer;
		++cpu->cpuTimer;
		
	}
}


//Gets the ball rolling by populating
//the Queue and then calling Perform()
int main(int argc, char* argv[]) {
	string mode;
	int t_cs = 13;
	int n;
	bool err;
	fstream file("processes.txt");
	vector<Process>* processVector = new vector<Process>;

	mode = "SRT";
	// file.clear();
	// file.seekg( 0, file.beg);
	err = ReadFile(file, processVector, mode);
	if( err )
		return -1;
	n = processVector->size();
	Perform( processVector, t_cs, mode);

	cout << endl << endl;

	return 0;
}



