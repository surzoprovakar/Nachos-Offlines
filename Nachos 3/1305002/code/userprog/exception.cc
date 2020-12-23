// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "addrspace.h"
//#include "processTable.h"
#include "synch.h"
#define max_fileName_length 64

extern ProcessTable *processTable;
extern MemoryManager *memMan;
extern CustomConsole *myconsole;
extern Semaphore *myread;
extern Semaphore *mywrite;

int pc;
//Lock *consoleLock = new Lock("console lock");
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

//void rAvail(void* arg) { readAvail->V(); }
//void wDone(void* arg) { writeDone->V(); }

//Console *myConsole = new Console(NULL,NULL,rAvail,wDone,0);

const char *getThreadName(Thread *t)
{
	return t->getName();
}

void regInitAndProcessRun(void *arg)
{
	//printf("process thread created successfully ===== 1\n");
	currentThread->space->InitRegisters();		// set the initial register values
	//printf("process thread created successfully ===== 2\n");    	
	currentThread->space->RestoreState();		// load page table register
	//printf("process thread created successfully ===== 3\n");
    	machine->Run();
	ASSERT(false);
}

int MyExec(int path)
{
	char filePath[max_fileName_length];
	int i=0, ch; //bool x;
	while(1)
	{
		//printf("path value: %d\n",x = machine->ReadMem(path,1,&ch));
		if(!machine->ReadMem(path,1,&ch))
		{
			if(!machine->ReadMem(path,1,&ch))
			{
				printf("1... in exec\n");
				return 0;
			}
		}
		filePath[i] = (char) ch;
		if(filePath[i]=='\0')
			break;
		i++;
		path++;
	}
	printf("allocated memory successfully\n");
	OpenFile *executable = fileSystem->Open(filePath);
    	AddrSpace *space;

    	if (executable == NULL)
	{
		printf("Unable to open file %s\n", filePath);
		return 0;
    	}
    	space = new AddrSpace(executable);
	Thread * t = new Thread (filePath);
	printf("fileName : %s\n",filePath);
	int id = processTable->Alloc((void *)t);
	//int pid = memMan->ProcessCounter++;
	printf("active Process Number: %d\n",processTable->GetActiveProcessNo());
	//printf("active Process Number: %d\n",memMan->ProcessCounter);
	if( id == -1)
	{
		printf("Process limit exceeded\n");
		return id;
	}
	//int id = processTable->Alloc(filePath)+1;
	//int id = 1;	
	t->setThreadID(id);    
    	t->space = space;
	//printf("Thread Id & space is set\n");
	//delete executable;			// close file
	//printf("executable file is closed\n");
	t->Fork(regInitAndProcessRun,(void *)0);
	//printf("going to return from my EXEC\n");
	return id;
}

void myExit()
{
	unsigned int ptSize = machine->pageTableSize;
	unsigned int i;
	//memMan->PrintMem();
	for(i=0; i<ptSize; i++)
	{
		if(machine->pageTable[i].valid)
			memMan->FreePage(machine->pageTable[i].physicalPage);
	}
	processTable->Release(currentThread->getThreadID());
	printf("Going to finish the currentThread, total process now : %d\n",processTable->GetActiveProcessNo());
	if(processTable->GetActiveProcessNo()==0)
	{
		interrupt->Halt();
	}
	//printf("Going to finish the currentThread\n");
	currentThread->Finish();
	//printf("Finished the currentThread\n");
}

void myRead(int buffer, int size, int id)
{
	myconsole->lockConsole->Acquire();
	for(int i = 0; i<size; i++)
	{
		//printf("\nIn SC_read\n");
		char ch;
		myread->P();
		ch = myconsole->con->GetChar();
		if(ch!='\n')
			machine->WriteMem(buffer+i,1,ch);
		else
		{
			machine->WriteMem(buffer+i,1,'\0');		
			break;
		}	
	}
	myconsole->lockConsole->Release();
	return;
}

void myWrite(int buffer, int size, int id)
{
	myconsole->lockConsole->Acquire();
	for(int i = 0; i<size; i++)
	{
		int ch;
		//printf("i: %d\n",i); 
		machine->ReadMem(buffer+i,1,&ch);
		//printf("%c",ch);
		myconsole->con->PutChar((char)ch);		
		mywrite->P();
	}
	myconsole->con->PutChar('\n');		
	mywrite->P();
	myconsole->lockConsole->Release();
}

void updatePCRegisters()
{
	pc = machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg,pc);
	pc=machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg,pc);
	pc += 4;
	machine->WriteRegister(NextPCReg,pc);
}


void pageFaultHandler(int addr)
{
	int vpn = addr/PageSize;
	TranslationEntry *pte = &machine->pageTable[vpn];
	
	pte->physicalPage = memMan->AllocPage(currentThread->getThreadID(),pte);
	pte->valid = true;
	pte->use = false;
	pte->dirty = false;
	pte->readOnly = false;
	if(!currentThread->space->bs->IsPresent(pte))
	{	
		//printf("Loading From Executable\n");
		stats->numDiskReads++;
		currentThread->space->loadIntoFreePage(addr, pte->physicalPage);
	}
	else
	{
		//printf("Loading From Backing Store\n");
		stats->PageIn++;
		currentThread->space->bs->PageIn(pte);
	}
}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    
    if (which == SyscallException)
    {
	switch (type)
	{
		case SC_Halt:
		{			
			DEBUG('a', "Shutdown, initiated by user program.\n");
   			interrupt->Halt();
			break;
		}
		case SC_Exec:
		{			
			DEBUG('a', "Execution of a process, initiated by user program.\n");
			printf("Execution of a process, initiated by user program.\n");
			int path = machine->ReadRegister(4);
			printf("%d\n", path);
			int id = MyExec(path);
			printf("id: %d\n", id);
			if(id == 0) exit(0);
			machine->WriteRegister(2,id);
			updatePCRegisters();
			printf("Execution of process id : %d, initiated by user program.\n",id);
			DEBUG('a', "Execution of process id : %d, initiated by user program.\n",id);
   			break;
		}
		case SC_Exit:
		{			
			DEBUG('a', "Exiting a process, initiated by user program.\n");
			int retVal = machine->ReadRegister(4);
			printf("value in exit : %d\n",retVal);
			machine->WriteRegister(2,retVal);
			myExit();
			updatePCRegisters();
   			break;
		}
		case SC_Read:
		{
			//printf("In console Read\n");
			int buffer = machine->ReadRegister(4);
			int size = machine->ReadRegister(5);
			int id = machine->ReadRegister(6);
			printf("buffer: %d size: %d id: %d\n",buffer,size,id);
			myRead(buffer,size,id);
			updatePCRegisters();
			break;
		}
		case SC_Write:
		{
			//printf("In console Write\n");
			int buffer = machine->ReadRegister(4);
			int size = machine->ReadRegister(5);
			int id = machine->ReadRegister(6);
			printf("buffer: %d size: %d id: %d\n",buffer,size,id);
			myWrite(buffer,size,id);
			updatePCRegisters();
			break;
		}
		case SC_Join:
		{
			printf("Join user mode exception %d %d\n", which, type);
			break;
		}
		case SC_Create:
		{
			printf("Create user mode exception %d %d\n", which, type);
			break;
		}
		case SC_Open:
		{
			printf("Open user mode exception %d %d\n", which, type);
			break;
		}
		case SC_Close:
		{
			printf("Close user mode exception %d %d\n", which, type);
			break;
		}
		case SC_Fork:
		{
			printf("Fork user mode exception %d %d\n", which, type);
			break;
		}
		case SC_Yield:
		{
			printf("Yield user mode exception %d %d\n", which, type);
			break;
		}
 		default: 
		{
			printf("Unexpected user mode exception %d %d\n", which, type);
			ASSERT(false);
		}
	}
    }
    else if(which == PageFaultException)
    {
    	//printf("PageFaultException\n");
	stats->numPageFaults++;
	int addr = machine->ReadRegister(39);
	pageFaultHandler(addr);
    } 
    else if(which == ReadOnlyException)
    {
    	printf("ReadOnlyException\n");
    	myExit();
    }
    else if(which == BusErrorException)
    {
    	printf("BusErrorException\n");
    	myExit();
    }
    else if(which == AddressErrorException)
    {
    	printf("AddressErrorException\n");
    	myExit();
    }
    else if(which == OverflowException)
    {
    	printf("OverflowException\n");
    	myExit();
    }
    else if(which == NumExceptionTypes)
    {
    	printf("NumExceptionTypes\n");
    	myExit();
    }
    else if(which == IllegalInstrException)
    {
    	printf("IllegalInstrException\n");
    	myExit();
    }
    else
    {
	printf("Unexpected user mode exception %d %d\n", which, type);
	myExit();
    }
}
