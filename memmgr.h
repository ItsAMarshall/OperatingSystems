#ifndef _MEMMGR_H_
#define _MEMMGR_H_

#include <iostream>
#include <string>
#include "process.h"

using namespace std;

class MemMgr {
	public:
	
		MemMgr(string mode_) {
			size = 256;
			memory = new char[size];
			clockTime = 0;
			lastProcess = 0;
			timeMax = 0;
			algorithm = mode_;
		}
		
		~MemMgr() { delete[] this->memory; }
		void UpdateClock();
		int GetClock();
		void SetTimeMax(int max);
		int GetMax();
		void InitMemory() {
			for( int i = 0; i < size; ++i ) {
				memory[i] = '.';
			}
		}

		bool InsertProcess(Process* process) {
		  bool success = true;
		  // if (algorithm == "first") {
		  //   success = this->placeProcessFirstFit_(process);
		  // } else if (algorithm == "best") {
		  //   success = this->placeProcessBestFit_(process);
		  // } else if (algorithm == "next") {
		  //   success = this->placeProcessNextFit_(process);
		  // } else {
		  //   std::cerr << "ERROR: INVALID ALGORITHM" << std::endl;
		  //   exit(EXIT_FAILURE);
		  if (!success) {
		      return false;
		  } else {
		    process->inMemory = true;
		    return true;
		  }
		}
		void RemoveProcess(Process & process);
		void PrintMemory() {
			cout << "Simulated Memory:" << endl;
			cout << string(32, '=');
			for( int i = 0; i < size; ++i) {
				if( i%32 == 0) {
					cout << endl;
				}
				cout << memory[i];
			}
			cout << endl << string(32, '=') << endl;;
		}
		
	private:
	
		int size;
		char* memory;
		int clockTime;
		int lastProcess;
		int timeMax;
		string algorithm;
		
		bool InsertFirst(const Process& process);
		bool InsertBest(const Process& process);
		bool InsertNext(const Process& process);
		bool InsertWorst(const Process& process);
		void InsertNon(const Process& process);
		
		int GetNextFree(int hintIndex);
		int GetFreeAmount(int index);
		void Defrag();
};

#endif