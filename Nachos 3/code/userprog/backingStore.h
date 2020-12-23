#ifndef BACKINGSTORE_H
#define BACKINGSTORE_H

#include "copyright.h"
#include "filesys.h"
#include "bitmap.h"
#include "translate.h"

class BackingStore {

	public:
		//static int bsId;
		OpenFile *f;
		BitMap *bmp;

		BackingStore(int numPhysPgs);
		~BackingStore();
		void PageOut(TranslationEntry *pte);
		void PageIn(TranslationEntry *pte);
		bool IsPresent(TranslationEntry *pte);
};


#endif // BACKINGSTORE_H
