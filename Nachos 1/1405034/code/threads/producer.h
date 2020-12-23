
#ifndef PROD_H
#define PROD_H

#include "synch.h"

// extern int buffer[20];
extern int len;
extern Lock *buff;
extern Condition *produce;
extern Condition *consume;
class Producer {
  public:
        Producer(int a);
        void producer();
  private:
        int id;
};

#endif
