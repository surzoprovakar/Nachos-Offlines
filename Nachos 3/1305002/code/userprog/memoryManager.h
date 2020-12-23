#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "copyright.h"
#include "filesys.h"
#include "processTable.h"
//#include "synch.h"
#include "bitmap.h"
#include "translate.h"

class MemoryManager{

	public:
		MemoryManager(int numPages);
		int AllocPage();
		int AllocPage(int pid, TranslationEntry *pte);
		//unsigned int AllocPageByForce();
		//unsigned int TotalFree();
		void FreePage(int physPageNum);
		int frameToReleaseRandom(int n);
		int frameToReleaseLRU();
		//void PrintMem();
		//bool PageIsAllocated(unsigned int physPageNum);

		int *pidArray;
		TranslationEntry **entries;
		//int ProcessCounter = 0;
	private:
		//Lock *lockMemMan = new Lock("lockMemMan");
		//BitMap *bitmap;
		int numPhysPgs;
};

#endif // MEMORYMANAGER_H
