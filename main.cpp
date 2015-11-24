/*
	NOTE:

		Nameing Schemes:
			Functions: first letter of each word is capitalized
			variables: first letter of each word is capitalized except first word

	TODO:

		1) Add process to queue when it is their arrival time
		2) Incorporate Lucas's memory manager
		3) Print out memory block 
		4) Add process to memory block

*/



#include <algorithm>
#include <deque>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include "memmgr.h"
#include "process.h"

using namespace std;


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
bool ReadFile(fstream & file, deque<Process>* cpuQueue, const string& mode) {
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
	deque<Process>* cpuQueue = new deque<Process>;

	mode = "SRT";
	// file.clear();
	// file.seekg( 0, file.beg);
	err = ReadFile(file, cpuQueue, mode);
	if( err )
		return -1;
	n = cpuQueue->size();
	Perform( cpuQueue, t_cs, mode);

	cout << endl << endl;

	return 0;
}



