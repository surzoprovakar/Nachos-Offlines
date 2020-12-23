#include "copyright.h"
#include "consumer.h"
#include "system.h"
#include "time.h"

void 
consumer::ConsumerFunction(void *name)
{
    char* threadName = (char*)name;
    lock->Acquire();
    //printf("----- %s acquired lock-----\n",threadName);
    while(length == 0){
	//printf("%s is waiting as table empty\n",threadName);	
	empty->Wait();
    }
    int m = buffer[rear];
    printf("Consumer thread: %s has consumed : %d\n",threadName,m);
    length--;
    rear = (rear + 1) % SIZE;
    if(length == SIZE-1) {   
	full->Broadcast();
	printf("%s is notifying all waiting producers\n",threadName);
    }
    lock->Release();
    //printf("----- %s released lock-----\n",threadName);
}
