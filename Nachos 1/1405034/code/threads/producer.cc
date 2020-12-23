#include "producer.h"
#include "system.h"

Producer::Producer(int a) {
    id=a;
}

void
Producer::producer() {
	while(1){
        //printf("pp\n");
		buff->Acquire();
		if(len==10)
		{
			produce->Wait(buff);
		}

		len=len+1;
		printf("Producer %d has produced food\n", id);

		if(len==1)
		{
			consume->Signal(buff);
		}

		buff->Release();
		currentThread->Yield();
	}
}

