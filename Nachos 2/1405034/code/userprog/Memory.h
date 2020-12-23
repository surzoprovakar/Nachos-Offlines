#ifndef MEMORY_H
#define MEMORY_H

#include "copyright.h"
#include "bitmap.h"

class MemoryManager
{
private:
    BitMap *bitMap;

public:
    MemoryManager(int pagesnum);
    
    int AllocatePage();

    void FreePage(int physicalPageNumber);

    bool PageIsAllocated(int physicalPageNum);

    int GetAvailableMemory();

};

#endif //MEMORY_H
