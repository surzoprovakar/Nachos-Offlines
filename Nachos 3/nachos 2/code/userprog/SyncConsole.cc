#include "copyright.h"
#include "SyncConsole.h"
#include "synch.h"

Lock *consoleLock    = new Lock("Console Lock");
Semaphore *readAvail = new Semaphore("Read Avail", 0);
Semaphore *writeDone = new Semaphore("Write Done", 0);

void ReadAvail(void* arg)
{
    readAvail->V();
}
void WriteDone(void* arg)
{
    writeDone->V();
}

SynchronizedConsole::SynchronizedConsole()
{
    console = new Console(NULL, NULL, ReadAvail, WriteDone, 0);
}


void SynchronizedConsole::GetLock()
{
    consoleLock->Acquire();
}

void SynchronizedConsole::ReleaseLock()
{
    consoleLock->Release();
}

void SynchronizedConsole::PutChar(char ch)
{
    console->PutChar(ch);
    writeDone->P();
}

char SynchronizedConsole::GetChar()
{
    readAvail->P();
    return console->GetChar();
}

