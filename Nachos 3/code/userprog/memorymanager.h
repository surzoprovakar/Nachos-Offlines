#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#ifndef BITMAP_H
#include "bitmap.h"
#endif

#ifndef SYNCH_H
#include "synch.h"
#endif

#include "copyright.h"
#include "filesys.h"
#include "processtable.h"
#include "translate.h"

class MemoryManager{
public:
	MemoryManager(int numPages);
	//~MemoryManager();
	int AllocPage();

        int AllocPage(int pid, TranslationEntry *pte);
	void FreePage(int physPageNum);

        int frameToReleaseRandom(int n);
	int frameToReleaseLRU();

	//bool PageIsAllocated(int physPageNum);
	//bool IsAnyPageFree();
	//int NumFreePages();

        int *pidArray;
	TranslationEntry **entries;
private:
        int numPhysPgs;
	//BitMap *bitMap;
	//Lock *lock;
};

#endif
