// threadtest.cc
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield,
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "producer.h"
#include "consumer.h"
#include "synch.h"

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread
//	each iteration.
//
//	"which" is simply a number identifyzing the thread, for debugging
//	purposes.
//----------------------------------------------------------------------
// int buffer[20];
int len;
Lock *buff;
Condition *produce;
Condition *consume;

/*void
SimpleThread(int which)
{
    int num;

    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}*/



//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void producers(int i)
{
	//printf("Running prod\n");
	Producer* prod = new Producer(i);
	prod->producer();

}

void consumers(int i)
{
	//printf("Running con\n");
	Consumer* con = new Consumer(i);
	con->consumer();
}

void
ThreadTest()
{
    //DEBUG('t', "Entering SimpleTest");

    //printf("Running test\n");
	len = 0;
    buff = new Lock((char *)"some");
    produce = new Condition((char *)"produce");
    consume = new Condition((char *)"consume");
    Thread *producer[5];
    Thread *consumer[5];
    int i;
    for(i=0;i<5;i++) {
		producer[i] = new Thread("Producer");
        producer[i]->Fork((VoidFunctionPtr)producers, (void*)i);
		//printf("Running test2\n");
    }

    for(i=0;i<5;i++) {
		consumer[i] = new Thread("Consumer");
        consumer[i]->Fork((VoidFunctionPtr)consumers, (void*)i);
		//printf("Running test3\n");
    }

}




