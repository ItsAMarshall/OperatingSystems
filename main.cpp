/*  
 *  Author: Marshall Shortledge
 *  Date: October 5th, 2015
 *  Class: Operating Systems
 *  Assignment: Project 2
*/

#include <algorithm>
#include <deque>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>

using namespace std;

class Process
{
public:
	Process() { }

	Process(int procNum_, int burstTime_, int burstCount_, int ioTime_, int priority_) : procNum( procNum_ ), burstTime( burstTime_ ), burstCount( burstCount_ ), ioTime( ioTime_), priority( priority_ ) {	
			cpuTimer = 0;
			timeWaiting = 0;
			preempted = false;
	}

	Process( const Process& other ) : procNum( other.procNum ), burstTime( other.burstTime ), burstCount( other.burstCount ), ioTime( other.ioTime), ioTimeEnd( other.ioTimeEnd ), priority( other.priority), cpuTimer( other.cpuTimer), preempted(other.preempted) { 
		timeWaiting = 0;
	}

	~Process() { }

	int procNum;
	int burstTime;
	int burstCount;
	int ioTime;
	int ioTimeEnd;
	int priority;
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

bool PWADequeSort(const Process &a, const Process &b) {
	if( a.priority < b.priority ) {
		return true;
	}
	else if( a.priority == b.priority && a.procNum < b.procNum ) {
		return true;
	}
	return false;
}

void PushBack( deque<Process>* cpuQueue, Process* proc, const string& mode ) {
	deque<Process>::iterator itr;
	proc->timeWaiting = 0;
	for( itr = cpuQueue->begin(); itr != cpuQueue->end(); ++itr ) {
		if( mode == "PWA" ){
			if( itr->priority > proc->priority ||( itr->priority == proc->priority && itr->procNum > proc->procNum) ) {
				cpuQueue->insert(itr, *proc);
				return;
			}
		}
		else if ( mode == "SRT" ) {
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
bool ReadFile(fstream & file, deque<Process>* cpuQueue, const string& mode) {
	char c = '|';
	string s;
	while( getline(file, s) ) {
		if( s[s.find_first_not_of(' ')] == '#') {
			continue;
		}
		int procNum_, burstTime_, burstCount_, ioTime_, priority_;
		stringstream temp(s);

		temp >> procNum_ >> c;
		temp >> burstTime_ >> c;
		temp >> burstCount_ >> c;
		temp >> ioTime_ >> c;
		temp >> priority_ >> c;

		Process proc(procNum_, burstTime_, burstCount_, ioTime_, priority_);
		if( mode == "FCFS")
			cpuQueue->push_back(proc);
		else 
			PushBack(cpuQueue, &proc, mode);
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
	if( mode == "PWA" ){
		if( proc->priority < cpu->priority ){
			cpu->preempted = true;
			return true;
		}
	}
	else {
		if( proc->burstTime < (cpu->burstTime - cpu->cpuTimer ) ) {
			cpu->preempted = true;
			return true;
		}
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
	cout << "P" << cpu->procNum << " started using the CPU ";
	//cout << "==END TIME: " << cpu->burstTime - cpu->cpuTimer + timer;
	PrintQueue(cpuQueue);

	return;
}

//Moves process from "CPU" to I/O priority queue
void LoadIO( Process* proc, deque<Process>* cpuQueue, priority_queue<Process>* ioQueue, int timer ) {
	proc->burstCount--;
	if( proc->burstCount == 0 ) {
		PrintTime(timer);
		cout << "P" << proc->procNum << " terminated ";
		PrintQueue(cpuQueue);
		proc = NULL;
		return;
	}

	PrintTime(timer);
	cout << "P" << proc->procNum << " completed its CPU burst ";
	PrintQueue(cpuQueue);

	PrintTime(timer);
	cout << "P" << proc->procNum << " performing I/O ";
	PrintQueue(cpuQueue);
	proc->ioTimeEnd = proc->ioTime + timer;
	ioQueue->push(*proc);
	proc = NULL;
}

//Checks to see if Process is done with its I/O
//If so, it moves it to the end of the Queue.
//Only checks top of priority queue because it is
//sorted by end time of I/O and then by name if there
//is a tie
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

void IncrementWait(deque<Process>* cpuQueue) {
	Process* ptr;
	for( unsigned int index = 0; index < cpuQueue->size(); ++index) {
		ptr = &cpuQueue[0][index];
		ptr->timeWaiting++;
		if( ptr->timeWaiting/ptr->burstTime >= 3 && ptr->timeWaiting%ptr->burstTime != 0 && ptr->priority > 0 ) {
				ptr->priority--;
				ptr->timeWaiting = 0;
		}
	}
	sort(cpuQueue->begin(), cpuQueue->end(), PWADequeSort);
}

void SwapPreempt(deque<Process>* cpuQueue, Process* cpu, Process* preemptCatch, const string& mode, int timer, int t_cs) {
		PrintTime(timer);
		cout << "P" << cpu->procNum << " preempted by P" << preemptCatch->procNum << " ";
		PushBack(cpuQueue, cpu, mode);
		PrintQueue(cpuQueue);
		//timer += t_cs;
		LoadCPU(cpuQueue, preemptCatch, cpu, timer, t_cs);
		
}

//Main function that handles the calling of other helper functions
void Perform(deque<Process>* cpuQueue, int t_cs, const string& mode) {
	static int timer = 0;
	timer = 0;
	Process* preemptCatch = NULL;
	priority_queue<Process>* ioQueue = new priority_queue<Process>;
	bool cpuInUse = false;
	Process* cpu = new Process();
	
	PrintTime(timer);
	cout << "Simulator started for " << mode << " ";
	PrintQueue(cpuQueue);
	
	//check priority of cpuqueue top
	//if less than cpu priority thats a preempt

	while( true ) {
		if( mode == "PWA" && cpuInUse && cpuQueue->size() > 0 ) {
			preemptCatch = new Process(cpuQueue->front() );
			if( Preempt(cpu, preemptCatch, mode) ){
				cpuQueue->pop_front();
				
				SwapPreempt(cpuQueue, cpu, preemptCatch, mode, timer, t_cs);
				
			}
			delete preemptCatch;
		}

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

			cout << "P" << preemptCatch->procNum << " completed I/O ";
			if( mode == "FCFS" ) {
				cpuQueue->push_back(*preemptCatch);
				PrintQueue(cpuQueue);
			}
			else {
				if( Preempt(cpu, preemptCatch, mode) ) {
					PrintQueue(cpuQueue);
					if( cpu->burstCount > 0){
						PushBack(cpuQueue, cpu, mode);
					
						PrintTime(timer);
						cout << "P" << cpu->procNum << " preempted by P" << preemptCatch->procNum << " ";
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
		if( mode == "PWA") {
			IncrementWait(cpuQueue);
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
	deque<Process>* cpuQueue = new deque<Process>;

	mode = "FCFS";
	err = ReadFile(file, cpuQueue, mode);
	if( err )
		return -1;
	n = cpuQueue->size();
	Perform( cpuQueue, t_cs, mode);

	cout << endl << endl;

	mode = "SRT";
	file.clear();
	file.seekg( 0, file.beg);
	err = ReadFile(file, cpuQueue, mode);
	if( err )
		return -1;
	n = cpuQueue->size();
	Perform( cpuQueue, t_cs, mode);

	cout << endl << endl;

	mode = "PWA";
	file.clear();
	file.seekg( 0, file.beg);
	err = ReadFile(file, cpuQueue, mode);
	if( err )
		return -1;
	n = cpuQueue->size();
	Perform( cpuQueue, t_cs, mode);

	return 0;
}
