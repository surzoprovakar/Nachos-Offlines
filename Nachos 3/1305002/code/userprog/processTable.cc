#include "copyright.h"
#include "processTable.h"
#include "synch.h"

Lock *lockTable = new Lock("lockTable");
ProcessTable::ProcessTable(int size)
{
	//lockTable->Acquire();
	sizeTable = size;	
	process = new void*[size];
	for(int i = 0; i<size; i++)
		process[i] = NULL;
	//process = malloc(sizeof(void *) * sizeTable);
	activeProcessNo = 0;
	//lockTable->Release();
}

int
ProcessTable::Alloc(void *object)
{
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
