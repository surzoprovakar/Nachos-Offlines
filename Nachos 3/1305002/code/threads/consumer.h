#ifndef CONSUMER_H
#define CONSUMER_H

#include "synch.h"

extern const int SIZE;
extern int buffer[];
extern int length;
extern int front;
extern int rear;
extern Lock *lock;
extern Condition *full;
extern Condition *empty;

class consumer {

  public:
    static void ConsumerFunction(void *name);
};

#endif // CONSUMER_H
