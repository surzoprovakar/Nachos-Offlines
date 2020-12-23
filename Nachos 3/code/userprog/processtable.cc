#include "processtable.h"
#include "copyright.h"
#include "synch.h"

Lock *lockTable = new Lock("lockTable");

/*Process::Process()
{

}

Process::~Process()
{

}

void
Process::Set(int threadId, void *object)
{
	id = threadId;
	process = object;
}

int
Process::GetID()
{
	return id;
}

void *
Process::GetProcess()
{
	return process;
}*/

ProcessTable::ProcessTable(int size)
{
	/*maxSize = size;
	nowSize = 0;
	processTableLock = new Lock("process table lock");
	bitMap = new BitMap(size);
	processes = new Process*[size];
	for(int i = 0; i < size; ++i)
		processes[i] = new Process();*/
    sizeTable = size;
	process = new void*[size];
	for(int i = 0; i<size; i++)
		process[i] = NULL;
	//process = malloc(sizeof(void *) * sizeTable);
	activeProcessNo = 0;
}

/*ProcessTable::~ProcessTable()
{
	delete processTableLock;
	delete bitMap;
	for(int i = 0; i < maxSize; ++i)
		delete processes[i];
	delete[] processes;
}*/

int
ProcessTable::Alloc(void *object)
{
	/*if(bitMap->NumClear() == 0)
		return -1;
	processTableLock->Acquire();
	processes[nowSize] = new Process();
	int temp = bitMap->Find();
	processes[nowSize]->Set(temp, object);
	nowSize++;
	processTableLock->Release();
	return temp;*/
	lockTable->Acquire();
	for(int i = 0; i<sizeTable; i++)
	{
		if(Get(i)==NULL)
		{
			printf("available\n");
			activeProcessNo++;
			process[i] = object;
			lockTable->Release();
			return i;
		}
	}
	lockTable->Release();
	return -1;
}

void*
ProcessTable::Get(int index)
{
	//lockTable->Acquire();
	ASSERT(index >= 0 && index < sizeTable);
	//lockTable->Release();
	return process[index];
}

void
ProcessTable::Release(int index)
{
	lockTable->Acquire();
	if(activeProcessNo != 0)
	{
		if(index >= 0 && index < sizeTable)
		{
			process[index] = NULL;
			activeProcessNo--;
		}
	}
	lockTable->Release();
}

bool
ProcessTable::Available(int index)
{
	//lockTable->Acquire();
	ASSERT(index >= 0 && index < sizeTable);
	if(process[index] == NULL) return true;
	else return false;
}

int
ProcessTable::GetActiveProcessNo()
{
	return activeProcessNo;
}
