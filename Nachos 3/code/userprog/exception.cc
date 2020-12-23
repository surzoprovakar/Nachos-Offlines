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
#include "synch.h"
#include "processtable.h"
#include "memorymanager.h"
#include "synchconsole.h"
#define max_fileName_length 64

extern ProcessTable *processTable;
//extern Lock *syscallLock;
extern MemoryManager *memoryManager;
extern CustomConsole * console;
extern Semaphore *myread;
extern Semaphore *mywrite;
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


void processCreator(void *arg)
{
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();
	//printf("fork successfully executed\n");
	// load page table register
	machine->Run(); // jump to the user progam
	ASSERT(false); // machine->Run never returns;
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
	t->Fork(processCreator,(void *)0);
	//printf("going to return from my EXEC\n");
	return id;
}

void ExitProcess()
{
	//syscallLock->Acquire();
	unsigned int ptSize = machine->pageTableSize;
	unsigned int i;

	//int size = machine->pageTableSize;
    for(i=0; i<ptSize; i++)
	{
		if(machine->pageTable[i].valid)
			memoryManager->FreePage(machine->pageTable[i].physicalPage);
	}
	processTable->Release(currentThread->getThreadID());
    printf("Going to finish the currentThread, total process now : %d\n",processTable->GetActiveProcessNo());
	//printf("exited with exit code %d\n", machine->ReadRegister(4));
	//syscallLock->Release();

	if(processTable->GetActiveProcessNo()==0)
	{
		interrupt->Halt();
	}
	currentThread->Finish();
}

void myRead(int buffer, int size, int id)
{
	console->lockConsole->Acquire();
	for(int i = 0; i<size; i++)
	{
		//printf("\nIn SC_read\n");
		char ch;
		myread->P();
		ch = console->con->GetChar();
		if(ch!='\n')
			machine->WriteMem(buffer+i,1,ch);
		else
		{
			machine->WriteMem(buffer+i,1,'\0');
			break;
		}
	}
	console->lockConsole->Release();
	return;
}

void myWrite(int buffer, int size, int id)
{
	console->lockConsole->Acquire();
	for(int i = 0; i<size; i++)
	{
		int ch;
		//printf("i: %d\n",i);
		machine->ReadMem(buffer+i,1,&ch);
		//printf("%c",ch);
		console->con->PutChar((char)ch);
		mywrite->P();
	}
	console->con->PutChar('\n');
	mywrite->P();
	console->lockConsole->Release();
}

void updateAllPCReg()
{
	/* routine task – do at last -- generally manipulate PCReg,
	PrevPCReg, NextPCReg so that they point to proper place*/
	int pc;
	pc=machine->ReadRegister(PCReg);
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

    pte->physicalPage = memoryManager->AllocPage(currentThread->getThreadId(),pte);
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

    if (which == SyscallException) {
    	if(type == SC_Halt){
			DEBUG('a', "Shutdown, initiated by user program.\n");
		   	interrupt->Halt();
	    }
	    else if(type == SC_Exec){
	   	    //IntStatus oldLevel = interrupt->SetLevel(IntOff);   //disable interrupt

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
	    	/*syscallLock->Acquire();
	    	int bufadd = machine->ReadRegister(4);

	    	char *filename = new char[100];
			//find a proper place to free this allocation
			int ch;
			if(!machine->ReadMem(bufadd,1,&ch))
				return;
			unsigned int i=0;
			while( ch != 0 )
			{
				filename[i] = (char)ch;
				bufadd += 1;
				i++;
				if(!machine->ReadMem(bufadd,1,&ch))
					return;
			}
			filename[i]=(char)0;*/
			// now filename contains the file

			/*OpenFile *executable = fileSystem->Open(filename);
			AddrSpace *space = new AddrSpace(executable);
			Thread * t = new Thread("tname");
			t->space = space;

			delete executable;

			t->id = processTable->Alloc( (void *) t );
			unsigned int processId = t->id;

			syscallLock->Release();

			t->Fork(processCreator, (void *) &processId);

			/* return the process id for the newly created process, return value
			is to write at R2 */
			/*machine->WriteRegister(2, processId);
			updateAllPCReg();*/
			//printf("thread with id %d created\n", processId);
			//(void) interrupt->SetLevel(oldLevel);               //re enable interrupt
	    }
	    else if(type == SC_Exit)
	    {
	    	//ExitProcess();
	    	DEBUG('a', "Exiting a process, initiated by user program.\n");
			int retVal = machine->ReadRegister(4);
			printf("value in exit : %d\n",retVal);
			machine->WriteRegister(2,retVal);
			ExitProcess();
			updateAllPCReg();
	    }
	    else if(type == SC_Read)
	    {
	    	/*syscallLock->Acquire();
	    	printf("in reading\n");
	    	int addr = machine->ReadRegister(4);
	    	int size = machine->ReadRegister(5);
	    	synchConsole->Read(addr, size, currentThread->id);
	    	syscallLock->Release();
	    	updateAllPCReg();*/
	    	int buffer = machine->ReadRegister(4);
			int size = machine->ReadRegister(5);
			int id = machine->ReadRegister(6);
			printf("buffer: %d size: %d id: %d\n",buffer,size,id);
			myRead(buffer,size,id);
			updateAllPCReg();
	    }
	    else if(type == SC_Write)
	    {
	    	/*syscallLock->Acquire();
	    	printf("in writing\n");
	    	int addr = machine->ReadRegister(4);
	    	int size = machine->ReadRegister(5);
	    	synchConsole->Write(addr, size, currentThread->id);
	    	syscallLock->Release();
	    	updateAllPCReg();*/
	    	int buffer = machine->ReadRegister(4);
			int size = machine->ReadRegister(5);
			int id = machine->ReadRegister(6);
			printf("buffer: %d size: %d id: %d\n",buffer,size,id);
			myWrite(buffer,size,id);
			updateAllPCReg();
	    }
	    else {
	    	printf("Unexpected user mode exception %d %d\n", which, type);
	    	ExitProcess();
	    }
    }
    else if(which == PageFaultException)
    {
    	//printf("PageFaultException\n");
    	stats->numPageFaults++;
    	int addr = machine->ReadRegister(39);
    	pageFaultHandler(addr);

    	//ExitProcess();
    }
    else if(which == ReadOnlyException)
    {
    	printf("ReadOnlyException\n");
    	ExitProcess();
    }
    else if(which == BusErrorException)
    {
    	printf("BusErrorException\n");
    	ExitProcess();
    }
    else if(which == AddressErrorException)
    {
    	printf("AddressErrorException\n");
    	ExitProcess();
    }
    else if(which == OverflowException)
    {
    	printf("OverflowException\n");
    	ExitProcess();
    }
    else if(which == NumExceptionTypes)
    {
    	printf("NumExceptionTypes\n");
    	ExitProcess();
    }
    else if(which == IllegalInstrException)
    {
    	printf("IllegalInstrException\n");
    	ExitProcess();
    }
    else {
		printf("Unexpected user mode exception %d %d\n", which, type);
		ExitProcess();
    }
}
