#include <vector>
#include <string>

using namespace std;

class MemMgr {
	public:
	
		explicit MemMgr(int s) : size(s) {
			this->memory = new char[s];
			this->clockTime = 0;
			this->lastProcess = 0;
			this->timeMax = 0;
		}
		
		~MemMgr() { delete[] this->memory; }
		void UpdateClock();
		int GetClock();
		void SetTimeMax(int max);
		int GetMax();
		void InitMemory(vector<Process> & processes);
		void InsertProcess(Process & process, string algorithm, 
							vector<Process> & processes, bool isDefragging);
		void RemoveProcess(Process & process);
		void VomitData();
		
	private:
	
		int size;
		char* memory;
		int clockTime;
		int lastProcess;
		int timeMax;
		
		bool InsertFirst(const Process& process);
		bool InsertBest(const Process& process);
		bool InsertNext(const Process& process);
		bool InsertWorst(const Process& process);
		void InsertNon(const Process& process);
		
		int GetNextFree(int hintIndex);
		int GetFreeAmount(int index);
		void Defrag(std::vector<Process> &processes, const Process &process);
}