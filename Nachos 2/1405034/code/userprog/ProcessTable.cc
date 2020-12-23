#include "copyright.h"
#include "synch.h"
#include "ProcessTable.h"

Lock* ProcessTableLock = new Lock("ProcessTableLock");


ProcessTable::ProcessTable(int size)
{
    int i;
    ThreadPointers = new void*[size+1];

    for(i=1;i<=size;i++) {
        ThreadPointers[i] = NULL;
    }

    ProcessesNum = 0;
    TableSize = size;
}


int ProcessTable::Allocate(void* object)
{
    int i;
    ProcessTableLock->Acquire();
    int ret = -1;
    for(i = 1; i <= TableSize; i++)
    {
        if(ThreadPointers[i] == NULL)
        {
            ThreadPointers[i] = object;
            ProcessesNum++;
            ret = i;
            break;
        }
    }
    ProcessTableLock->Release();
    return ret;

}

void* ProcessTable::Get(int index)
{
    ProcessTableLock->Acquire();

    if(ThreadPointers[index] != NULL)
    {
        ProcessTableLock->Release();
        return ThreadPointers[index];
    }

    ProcessTableLock->Release();
    return NULL;
}

void ProcessTable::Release(int index)
{
    ProcessTableLock->Acquire();
    ThreadPointers[index] = NULL;
    ProcessesNum--;
    ProcessTableLock->Release();
}

int ProcessTable::GetProcessNumber()
{
    return ProcessesNum;
}
