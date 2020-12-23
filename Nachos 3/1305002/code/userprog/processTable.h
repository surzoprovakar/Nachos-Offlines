#ifndef PROCESSTABLE_H
#define PROCESSTABLE_H

#include "copyright.h"
#include "utility.h"
//#include "system.h"
//#include "synch.h"

class ProcessTable{

	public:
		ProcessTable(int size);
		int Alloc(void *object);
		void *Get(int index);
		void Release(int index);
		int GetActiveProcessNo();
		bool Available(int index);
	private:
		//Lock *lockTable = new Lock("lockTable");
		void **process;
		int sizeTable;
		int activeProcessNo;
};

#endif // PROCESSTABLE_H
