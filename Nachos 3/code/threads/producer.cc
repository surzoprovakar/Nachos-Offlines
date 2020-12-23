#include "copyright.h"
#include "producer.h"
#include "system.h"
#include "time.h"

const int SIZE = 6;
int buffer[SIZE];
int length = 0;
int front = 0;
int rear = 0;
Lock *lock = new Lock("lock");
Condition *full = new Condition("full",lock);
Condition *empty = new Condition("empty",lock);

int randomProducer(int n)
{
    int t;
    srand(time(NULL));
    for(int i = 0; i<n; i++) 
	t = rand()%1500 + 1000;
    //printf("random : %d\n",t);
    return t;
}

void 
producer::ProducerFunction(void *info)
{
    producerInfo* pi = (producerInfo*)info;
    lock->Acquire();
    //printf("----- %s acquired lock-----\n",pi->name);
    while(length == SIZE) {
	//printf("%s is waiting as table full\n",pi->name);	
	full->Wait();
    }
    int t = randomProducer(pi->no);
    for(int i = 0; i<t; i++); //waiting the thread idly
    buffer[front] = pi->no;
    printf("Producer thread: %s has produced : %d\n",pi->name,pi->no);
    length++;
    front = (front + 1) % SIZE;
    if(length == 1) {
	empty->Broadcast();
	printf("%s is notifying all waiting consumers\n",pi->name);
    }
    lock->Release();
    //printf("----- %s released lock-----\n",pi->name);
}
