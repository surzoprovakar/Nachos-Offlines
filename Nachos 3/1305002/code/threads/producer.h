#ifndef PRODUCER_H
#define PRODUCER_H

#include "synch.h"

extern const int SIZE;
extern int buffer[];
extern int length;
extern int front;
extern int rear;
extern Lock *lock;
extern Condition *full;
extern Condition *empty;

class producerInfo {

  public:
    char * name = new char[100];
    int no;

};

class producer {

  public:
    static void ProducerFunction(void *info);
};

#endif // PRODUCER_H
