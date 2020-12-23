// progtest.cc 
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.  
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "customConsole.h"
//#include "processTable.h"
#include "synch.h"

//MemoryManager *memMan = new MemoryManager(NumPhysPages);
//ProcessTable *processTable = new ProcessTable(32);

MemoryManager *memMan;
ProcessTable *processTable = new ProcessTable(32);
Semaphore *mywrite;
Semaphore *myread;
CustomConsole *myconsole;

void myReadAvail(void* arg) { myread->V(); }
void myWriteDone(void* arg) { mywrite->V(); }

//Console *console = new Console(NULL,NULL,ReadAvail,WriteDone,0);

//Lock *lockMemMan = new Lock("LockMemMan");
//Lock *lockTable = new Lock("lockTable");
//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------

void
StartProcess(const char *filename)
{
    memMan = new MemoryManager(NumPhysPages);
    //processTable = new ProcessTable(32);
    myread = new Semaphore("read",0);
    mywrite = new Semaphore("write",0);
    myconsole = new CustomConsole(myReadAvail,myWriteDone);
    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;

    if (executable == NULL) {
	printf("Unable to open file %s\n", filename);
	exit(0);	
	//return;
    }
    space = new AddrSpace(executable);
    int id = processTable->Alloc((void *)currentThread);
	//int pid = memMan->ProcessCounter++;
    printf("active Process Number: %d\n",processTable->GetActiveProcessNo());
    currentThread->setThreadID(id);    
    currentThread->space = space;

    //delete executable;			// close file

    space->InitRegisters();		// set the initial register values
    space->RestoreState();		// load page table register

    machine->Run();			// jump to the user progam
    ASSERT(false);			// machine->Run never returns;
					// the address space exits
					// by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

//Console *console;
//static Semaphore *readAvail;
//static Semaphore *writeDone;


//Semaphore *readAvail = new Semaphore("read avail", 0);
//Semaphore *writeDone = new Semaphore("write done", 0);


//----------------------------------------------------------------------
// ConsoleInterruptHandlers
// 	Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

//static void ReadAvail(void* arg) { readAvail->V(); }
//static void WriteDone(void* arg) { writeDone->V(); }

//void ReadAvail(void* arg) { readAvail->V(); }
//void WriteDone(void* arg) { writeDone->V(); }
//Console *console = new Console(NULL,NULL,ReadAvail,WriteDone,0);
//Console *console;


static Semaphore *readAvail;
static Semaphore *writeDone;


static void ReadAvail(void* arg) { readAvail->V(); }
static void WriteDone(void* arg) { writeDone->V(); }
static Console *console;

//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void 
ConsoleTest (const char *in, const char *out)
{
    char ch;

    console = new Console(NULL, NULL, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    //printf("in: %s\n",in);
    //printf("out: %s\n",out);
    for (;;) {
	readAvail->P();		// wait for character to arrive
	ch = console->GetChar();
	console->PutChar(ch);	// echo it!
	writeDone->P() ;        // wait for write to finish
	if (ch == 'q') return;  // if q, quit
    }
}
