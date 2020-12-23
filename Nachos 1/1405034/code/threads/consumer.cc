#include "consumer.h"
#include "system.h"

Consumer::Consumer(int a) {
    id=a;
}

void
Consumer::consumer() {

   while(1){
        //printf("cc\n");
		buff->Acquire();
		if(len==0)
		{
			consume->Wait(buff);
		}

		len=len-1;
		printf("Consumer %d has consumed food\n", id);

		if(len==9)
		{
			produce->Signal(buff);
		}

		buff->Release();
		currentThread->Yield();
	}

}
