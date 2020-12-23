#include "copyright.h"
#include "memoryManager.h"
#include "synch.h"
#include "time.h"
#include "system.h"

Lock *lockMemMan = new Lock("LockMemMan");
extern ProcessTable *processTable;

MemoryManager::MemoryManager(int numPages)
{
	//lockMemMan->Acquire();
	//ProcessCounter++;
	numPhysPgs = numPages;
	//bitmap = new BitMap(numPages);
	pidArray = new int[numPages];
	entries = new TranslationEntry*[numPages];
	for(int i = 0; i < numPages; i++)
		entries[i] = 0;
	//lockMemMan->Release();
}

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
	//lockMemMan->Acquire();
	//bitmap->Clear(physPageNum);
	entries[physPageNum] = 0;
	//lockMemMan->Release();
}
