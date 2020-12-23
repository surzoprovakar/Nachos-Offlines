#ifndef PROCESSTABLE_H
#define PROCESSTABLE_H

#include "copyright.h"
#include "utility.h"

class ProcessTable
{
public:
    void  **ThreadPointers; 
    int  TableSize;
    int  ProcessesNum;

    ProcessTable(int size);

    int Allocate(void* object);

    void  *Get(int index);

    void Release(int index);

    int GetProcessNumber();
};

#endif //PROCESSTABLE_H
