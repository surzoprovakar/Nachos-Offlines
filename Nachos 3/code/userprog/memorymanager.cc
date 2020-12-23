#include "copyright.h"
#include "memorymanager.h"
#include "synch.h"
#include "system.h"
#include "time.h"

Lock *lockMemMan = new Lock("LockMemMan");
extern ProcessTable *processTable;

MemoryManager::MemoryManager(int numPages)
{
    numPhysPgs = numPages;
	//bitMap = new BitMap(numPages);
	//lock = new Lock("lock of memory manager");
	pidArray = new int[numPages];
	entries = new TranslationEntry*[numPages];
	for(int i = 0; i < numPages; i++) { entries[i] = 0;}
}

/*MemoryManager::~MemoryManager()
{
	delete bitMap;
	delete lock;
}

int
MemoryManager::AllocPage()
{
	lock->Acquire();
	int ret = bitMap->Find();
	lock->Release();
	return ret;
}*/

int
MemoryManager::frameToReleaseRandom(int n)
{
	int t;
    srand(time(NULL));
    for(int i = 0; i<=n; i++)
    t = rand()%numPhysPgs;
    //printf("random : %d\n",t);
    return t;
}

int
MemoryManager::frameToReleaseLRU()
{
	int t = entries[0]->timeStamp;
	int index = 0;
	for(int i = 1; i < numPhysPgs; i++)
	{
		if(entries[i]->timeStamp < t)// && !entries[i]->dirty)
		{
			t = entries[i]->timeStamp;
			index = i;
		}
	}
	return index;
}

int
MemoryManager::AllocPage(int pid, TranslationEntry *pte)
{
	lockMemMan->Acquire();
	for(int i = 0; i< numPhysPgs; i++)
	{
		if(entries[i]==0)
		{
			//printf("========Page Available========\n");
			pidArray[i] = pid;
			entries[i] = pte;
			lockMemMan->Release();
			return i;
		}
	}
	//printf("<<<<<<<<Page Replacement>>>>>>>\n");
	//int pageFrame = frameToReleaseRandom(pte->virtualPage);
	int pageFrame = frameToReleaseLRU();
	entries[pageFrame]->valid = false;
	Thread *t = (Thread *)processTable->Get(pidArray[pageFrame]);
	t->space->bs->PageOut(entries[pageFrame]);
	stats->PageOut++;
	pidArray[pageFrame] = pid;
	entries[pageFrame] = pte;
	lockMemMan->Release();
	return pageFrame;
}

void
MemoryManager::FreePage(int physPageNum)
{
	//lock->Acquire();
	//bitMap->Clear(physPageNum);
	//lock->Release();
	entries[physPageNum] = 0;
}

/*bool
MemoryManager::PageIsAllocated(int physPageNum)
{
	lock->Acquire();
	bool ret = bitMap->Test(physPageNum);
	lock->Release();
	return ret;
}

bool
MemoryManager::IsAnyPageFree()
{
	lock->Acquire();
	bool ret;
	if(bitMap->NumClear() == 0)
		ret = false;
	else
		ret = true;
	lock->Release();
	return ret;
}

int
MemoryManager::NumFreePages()
{
	lock->Acquire();
	int ret = bitMap->NumClear();
	lock->Release();
	return ret;
}*/
