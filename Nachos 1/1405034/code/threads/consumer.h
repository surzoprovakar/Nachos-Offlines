

#ifndef Con_H
#define Con_H

#include "synch.h"

// extern int buffer[20];
extern int len;
extern Lock *buff;
extern Condition *produce;
extern Condition *consume;
class Consumer {
  public:
        Consumer(int a);
        void consumer();
  private:
        int id;
};


#endif