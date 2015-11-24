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

		void InitMemory() {
			for( int i = 0; i < size; ++i ) {
				memory[i] = '.';
			}
		}

		bool InsertProcess(Process* process) {
		  bool success = false;
		  if (algorithm == "first") {
		    success = this->InsertFirst(process);
		  } else if (algorithm == "best") {
		 	  success = this->InsertBest(process);
		  } else if (algorithm == "next") {
		    success = this->InsertNext(process);
		  } else {
		    std::cerr << "ERROR: INVALID ALGORITHM" << std::endl;
		    exit(EXIT_FAILURE);
		  }
		  if (!success) {
		      return false;
		  } else {
		    process->inMemory = true;
		    return true;
		  }
		}
		
		void RemoveProcess(Process* process) {
			for (int i = 0; i < size; ++i) {
		    if (memory[i] == process->procNum) {
		      memory[i] = '.';
		    }
	  	}	
			process->inMemory = false;
		}

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

		int Defrag() {
		  
		  int count = 0;

		  int index = GetNextFree(0);
		  index += GetFreeAmount(index);
		  while( index < size ) {
		  	while(memory[index] == '.' && index < size-1) {
		  		++index;
		  	}
		  	memory[GetNextFree(0)] = memory[index];
		  	memory[index] = '.';
		  	++count;
		  	++index;
		  }

		  return count;
		}
		
	private:
	
		int size;
		char* memory;
		int clockTime;
		int lastProcess;
		int timeMax;
		string algorithm;
		
		bool InsertFirst( Process* process)  {
		  // Start at the first index not taken up by the system
		  // processes.
		  int index = GetNextFree(0);
		  int blockSize = 0;
		  int nextFree;

		  // Get the size of the next free contiguous block in memory.
		  while (blockSize < process->memorySize && index < size) {
		    nextFree = GetNextFree(index);
		    blockSize = GetFreeAmount(nextFree);
		    index = nextFree + blockSize;
		  }
		  // If there is not enough memory for this process, return false.
		  if (blockSize < process->memorySize) {
		    return false;
		  }
		  for (int i = 0; i < process->memorySize; ++i) {
		    memory[nextFree + i] = process->procNum;
		  }
		  return true;
		}

		bool InsertBest(Process* process) {
			int index = GetNextFree(0);
		  int blockSize = 0;
		  int nextFree;

		  int bestFit = size;
		  int bestIndex = -1;
		  // Loop through memory.
		  while (index < size) {
		    // Get the size of the next free block of memory.
		    nextFree = GetNextFree(index);
		    blockSize = GetFreeAmount(nextFree);
		    // If this is the best fit free block for this process,
		    // place it there.
		    if (blockSize < bestFit && blockSize >= process->memorySize) {
		      bestFit = blockSize;
		      bestIndex = nextFree;
		    }
		    index = nextFree + blockSize;
		  }
		  // If no place for this process could be found, return false.
		  if (bestIndex < 0) {
		    return false;
		  }
		  // Place the process in memory.
		  for (int i = 0; i < process->memorySize; ++i) {
		    memory[bestIndex + i] = process->procNum;
		  }
		  return true;
		}

		bool InsertNext(Process* process) {
			int index = lastProcess;
		  int blockSize = 0;
		  int nextFree;
		  // Loop through the memory until there is a free block
		  // big enough to fit this process.
		  while (blockSize < process->memorySize && index < size) {
		    nextFree = GetNextFree(index);
		    blockSize = GetFreeAmount(nextFree);
		    index = nextFree + blockSize;
		  }

		  if (blockSize < process->memorySize) {
		    return false;
		  }

		  for (int i = 0; i < process->memorySize; ++i) {
		    memory[nextFree + i] = process->procNum;
		  }

		  lastProcess = nextFree;
		  return true;
		}
		
		int GetNextFree(int index)  {
		  for (int i = index; i < size; ++i) {
		    if (memory[i] == '.') {
		      return i;
		    }
		  }
		  return 0;
		}
		int GetFreeAmount(int index)  {
		  for (int i = index; i < size; ++i) {
		    if (memory[i] != '.') {
		      return i - index;
		    }
		  }

		  return size - index;
		}

};

#endif