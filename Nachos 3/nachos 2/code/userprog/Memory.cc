#include "copyright.h"
#include "Memory.h"
#include "synch.h"

Lock* MemoryLock = new Lock("MemoryLock");

MemoryManager::MemoryManager(int pagesnum)
{
    bitMap = new BitMap(pagesnum);
}


int MemoryManager::AllocatePage()
{
    int page = -1;
    MemoryLock->Acquire();
    page = bitMap->Find();
    MemoryLock->Release();
    return page;
}


void MemoryManager::FreePage(int physicalPageNumber)
{
    MemoryLock->Acquire();
    bitMap->Clear(physicalPageNumber);
    MemoryLock->Release();

    return;
}


bool MemoryManager::PageIsAllocated(int physicalPageNum)
{
    MemoryLock->Acquire();
    bool CheckPage =  bitMap->Test(physicalPageNum);
    MemoryLock->Release();

    return CheckPage;
}

int MemoryManager::GetAvailableMemory()
{
    MemoryLock->Acquire();
    int available = bitMap->NumClear();
    MemoryLock->Release();

    return available;
}

