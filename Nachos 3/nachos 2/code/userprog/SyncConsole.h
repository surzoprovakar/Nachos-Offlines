#ifndef SYNCCONSOLE_H
#define SYNCCONSOLE_H

#include "copyright.h"
#include "console.h"

class SynchronizedConsole
{
private:
    Console *console;
public:
    SynchronizedConsole();

    void GetLock();

    void ReleaseLock();

    void PutChar(char ch);

    char GetChar();

};

#endif //SYNCCONSOLE_H
