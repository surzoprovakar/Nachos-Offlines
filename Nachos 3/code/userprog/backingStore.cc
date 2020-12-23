#include "copyright.h"
#include "backingStore.h"
#include "system.h"

static int bsId = 0;

BackingStore::BackingStore(int numPhysPgs)
{
	char name[10];
	sprintf(name,"mySwap_%d",++bsId);
	if(fileSystem->Create(name, numPhysPgs*PageSize))
	{
		f = fileSystem->Open(name);
	}
	bmp = new BitMap(numPhysPgs);	
}

BackingStore::~BackingStore()
{
	delete f;
	delete bmp;
}

void
BackingStore::PageOut(TranslationEntry *pte)
{
	//printf("Page Out: Physical: %d --- Virtual: %d\n",pte->physicalPage,pte->virtualPage);
	pte->valid = false;
	if(!IsPresent(pte) || pte->dirty)
	{
		f->WriteAt(&(machine->mainMemory[pte->physicalPage*PageSize]),PageSize,pte->virtualPage*PageSize);
		bmp->Mark(pte->virtualPage);
	}
}

void
BackingStore::PageIn(TranslationEntry *pte)
{
	//printf("Page In: Virtual: %d --- Physical: %d\n",pte->virtualPage,pte->physicalPage);
	f->ReadAt(&(machine->mainMemory[pte->physicalPage*PageSize]),PageSize,pte->virtualPage*PageSize);
}

bool
BackingStore::IsPresent(TranslationEntry *pte)
{
	//printf("Present Check: %d\n",pte->virtualPage);
	if(bmp->Test(pte->virtualPage))
		return true;
	return false;
}
