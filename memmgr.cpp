#include <vector>
#include <string>
#include <cstdlib>

#include "memmgr.h"

using namespace std;

void MemMgr::UpdateClock() {
	this->clockTime += 1;
}

int MemMgr::GetTime() {
	return this->clockTime;
}

void MemMgr::SetTimeMax(int max) {
	this->timeMax = max;
}

int MemMgr::GetMax() {
	return this->timeMax;
}

void MemMgr::InitMemory(Process & process, string algorithm, 
							vector<Process> & processes, bool isDefragging) {
								
	bool inserted = false;
	int tries = 0;
	
	while (inserted == false && tries < 2) { // need to try and insert twice in case memory is too fragmented on first attempt
		++tries;
		switch (algorithm) {
			case "firstFit":
				
				break;
			case "bestFit":
				
				break;
			case "nextFit":
			
				break;
			case "worstFit":
			
				break;
			case "nonFit": 
			
				break;
			default:
				cerr << "ERROR: Invalid Algorithm (options are 'firstFit', 'bestFit', 'nextFit', 'worstFit', 'nonFit'" << endl;
				exit(EXIT_FAILURE);
		}
		
		if (inserted == false) {
			if (tries < 2 && !isDefragging) {
				
			}
			else {
				cerr << "ERROR: Out of memory" << endl;
				exit(EXIT_FAILURE);
			}
		}
	}
}

bool MemMgr::InsertFirst(const Process & process) {
	int index = 80;
	int memLength; // size of block being checked
	int nextBlock; // next free block
	
	while (memLength < process.getSize() && index < this->size) {
		nextBlock = this->GetNextFree(index);
		memLength = this->GetFreeAmount(nextBlock);
		index = memLength + nextBlock;
	}
	
	if (memLength < process.getSize()) {
		return false;
	}
	for (int i = 0; i < process.getSize(); ++i) {
		this->memory[nextBlock + i] = process.getName();
	}
	return true;
}

bool MemMgr::InsertBest(const Process & process) {
	int index = 80;
	int memLength;
	int nextBlock;
	int bestFit = this->size;
	int bestIndex = -1;
	
	while (index < this->size) {
		nextBlock = this->GetNextFree(index);
		memLength = this->GetFreeAmount(nextBlock);
		
		if (memLength < bestFit && memLength >= process.getSize()) {
			bestFit = memLength;
			bestIndex = nextBlock;
		}
		index = nextBlock + memLength;
	}
	
	if (bestIndex < 0) {
		return false;
	}
	
	for (int i = 0; i < process.getSize(); ++i) {
		this->memory[bestIndex + i] = process.getName();
	}
	return true;
}

bool MemMgr::InsertNext(const Process & process) {
	
}

bool MemMgr::InsertWorst(const Process & process) {
	
}

void MemMgr::InsertNon(const Process & process) {
	
}

void MemMgr::VomitData() {
	cout << "time " << this->clockTime << "ms: " << endl;
	for (int i = 0; i < this->size; ++i) {
		cout << "\t" << memory[i] << endl;
	}
}