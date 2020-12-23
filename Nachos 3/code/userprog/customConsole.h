#ifndef CUSTOMCONSOLE_H
#define CUSTOMCONSOLE_H

#include "copyright.h"
#ifndef CONSOLE_H
#include "console.h"
#endif
#include "synch.h"

class CustomConsole
{
	public:
		Console *con;
		Lock *lockConsole;

		CustomConsole(VoidFunctionPtr readAvail, VoidFunctionPtr writeDone)
		{
			con = new Console(NULL, NULL, readAvail, writeDone, 0);
			lockConsole = new Lock("Console Lock");
		}
};

#endif //CUSTOMCONSOLE_H
