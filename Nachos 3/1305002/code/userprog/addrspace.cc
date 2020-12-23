// addrspace.cc
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
//#include "noff.h"

extern MemoryManager *memMan;

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable)
{
    //NoffHeader noffH;
    unsigned int i, size;
    exe = executable;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

    bs = new BackingStore(numPages);
    //printf("numPages : %d, Available Pages : %d\n",numPages,memMan->TotalFree());
    //ASSERT(numPages <= memMan->TotalFree());
    //ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n",
					numPages, size);
	printf("Initializing address space, num pages %d, size %d\n",
					numPages, size);
// first, set up the translation
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
	pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #

	/*my  code: for page to page frame mapping*/
	//pageTable[i].physicalPage = memMan->AllocPage();
	pageTable[i].physicalPage = -1;
	pageTable[i].valid = false;
	pageTable[i].use = false;
	pageTable[i].dirty = false;
	pageTable[i].readOnly = false;  // if the code segment was entirely on
					// a separate page, we could set its
					// pages to be read-only
    }
    //noffH = noffH;
    printf("code base addr: %d code size: %d code file addr: %d\n",noffH.code.virtualAddr,noffH.code.size,noffH.code.inFileAddr);
    printf("init base addr: %d init size: %d init file addr: %d\n",noffH.initData.virtualAddr,noffH.initData.size,noffH.initData.inFileAddr);
    printf("uninit base addr: %d uninit size: %d uninit file addr: %d\n",noffH.uninitData.virtualAddr,noffH.uninitData.size,noffH.uninitData.inFileAddr);
// zero out the entire address space, to zero the unitialized data segment
// and the stack segment
//    bzero(machine->mainMemory, size);
	/*for(i = 0; i < numPages; i++)
	{
	        bzero(&machine->mainMemory[pageTable[i].physicalPage*PageSize],PageSize);
	}*/

// then, copy in the code and data segments into memory
    /*if (noffH.code.size > 0) {
	unsigned int codeSize = noffH.code.size;
	//unsigned int numCodePages = divRoundUp(codeSize, PageSize);
    	//codeSize = numCodePages * PageSize;
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
			noffH.code.virtualAddr, codeSize);
	printf("Initializing code segment, at 0x%x, size %d\n",
			noffH.code.virtualAddr, codeSize);
	unsigned int cs = noffH.code.virtualAddr;
	unsigned int fs = noffH.code.inFileAddr;
	unsigned int loadSize = PageSize;
	while(codeSize>0)
	{
		loadSize = PageSize - cs%PageSize;
		if(codeSize<PageSize) loadSize = codeSize;
		//printf("loadSize : %d\n",loadSize);
		executable->ReadAt(&(machine->mainMemory[pageTable[cs/PageSize].physicalPage*PageSize + cs%PageSize]),loadSize,fs);
		//printf("virtual Addr : %d physical Addr : %c\n",cs,machine->mainMemory[pageTable[cs/PageSize].physicalPage*PageSize + cs%PageSize]);
		cs+=loadSize;
		fs+=loadSize;
		codeSize-=loadSize;
	}
    }*/
   /* if (noffH.initData.size > 0) {
	unsigned int dataSize = noffH.initData.size;
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
			noffH.initData.virtualAddr, dataSize);
	printf("Initializing data segment, at 0x%x, size %d\n",
			noffH.initData.virtualAddr, dataSize);
	unsigned int ds = noffH.initData.virtualAddr;
	unsigned int fs = noffH.initData.inFileAddr;
	char phyAdd;
	unsigned int loadSize=PageSize;
	char ch;
	while(dataSize>0)
	{
		loadSize = PageSize - ds%PageSize;
		if(dataSize<PageSize) loadSize = dataSize;
		//printf("loadSize : %d\n",loadSize);
		executable->ReadAt(&(machine->mainMemory[pageTable[ds/PageSize].physicalPage*PageSize + ds%PageSize]),loadSize,fs);
		//printf("virtual Addr : %d physical Addr : %c\n",ds,machine->mainMemory[pageTable[ds/PageSize].physicalPage*PageSize + ds%PageSize]);
		ds+=loadSize;
		fs+=loadSize;
		dataSize-=loadSize;
	}

    }*/
    printf("Exiting address space for the process\n");

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
   delete pageTable;
   delete exe;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------
int
AddrSpace::loadIntoFreePage(int addr, int physicalPageNo)
{
	int vpn = addr/PageSize;

	//printf("virtual addr: %d physical page no: %d\n",addr,physicalPageNo);
	int cs = noffH.code.virtualAddr;
	int ids = noffH.initData.virtualAddr;
	int uids = noffH.initData.virtualAddr;
	int css = noffH.code.size;
	int idss = noffH.initData.size;
	int uidss = noffH.initData.size;
	int fcs = noffH.code.inFileAddr;
	int fids = noffH.initData.inFileAddr;
	int fuids = noffH.uninitData.inFileAddr;
	int csl = cs+css-1; //array starts from 0 index
	int idsl = ids+idss-1;
	int uidfsl = uids+uidss-1;

	int s = vpn*PageSize; //from where I will load
	int e = s + PageSize -1;  //how much to load

	if((s>=cs && s<= csl) && (e>=cs && e<=csl))
	{
		//printf("----Case: 1---- addr: %d\n start: %d in code\n end: %d in code\n",addr,s,e);
		int fs = fcs + (s - cs); //code virtual address and file virtual address is not same. some gap file ahead,that is fcs
		exe->ReadAt(&(machine->mainMemory[pageTable[vpn].physicalPage*PageSize]),PageSize,fs);
	}
	else if((s>=cs && s<= csl) && (e>=ids && e<=idsl))
	{
		int size = csl - s + 1;
		int fs = fcs + (s - cs);
		exe->ReadAt(&(machine->mainMemory[pageTable[vpn].physicalPage*PageSize]),size,fs);
		exe->ReadAt(&(machine->mainMemory[pageTable[vpn].physicalPage*PageSize + size]),PageSize - size,fids);
		//printf("----Case: 2---- addr: %d\n start: %d in code\n end: %d in data\n",addr,s,e);
	}
	else if((s>=cs && s<= csl) && (e>idsl))
	{
		bzero(&machine->mainMemory[pageTable[vpn].physicalPage*PageSize],PageSize);
		int size = csl - s + 1;
		int fs = fcs + (s - cs);
		exe->ReadAt(&(machine->mainMemory[pageTable[vpn].physicalPage*PageSize]),size,fs);
		exe->ReadAt(&(machine->mainMemory[pageTable[vpn].physicalPage*PageSize + size]),idss,fids);
		//printf("----Case: 3---- addr: %d\n start: %d in code\n end: %d in other\n",addr,s,e);
	}
	else if((s>=ids && s<= idsl) && (e>=ids && e<=idsl))
	{
		int fs = fids + (s - ids);
		exe->ReadAt(&(machine->mainMemory[pageTable[vpn].physicalPage*PageSize]),PageSize,fs);
		//printf("----Case: 4---- addr: %d\n start: %d in data\n end: %d in data\n",addr,s,e);
	}
	else if((s>=ids && s<= idsl) && (e>idsl))
	{
		bzero(&machine->mainMemory[pageTable[vpn].physicalPage*PageSize],PageSize);
		int size = idsl - s + 1;
		int fs = fids + (s - ids);
		exe->ReadAt(&(machine->mainMemory[pageTable[vpn].physicalPage*PageSize]),size,fs);
		//printf("----Case: 5---- addr: %d\n start: %d in data\n end: %d in other\n",addr,s,e);
	}
	else
	{
		bzero(&machine->mainMemory[pageTable[vpn].physicalPage*PageSize],PageSize);
		//printf("----Case: 6---- addr: %d\n start: %d in other\n end: %d in other\n",addr,s,e);
	}
	return 0;
}
void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState()
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState()
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}
