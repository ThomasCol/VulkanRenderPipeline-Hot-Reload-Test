#include "MyTestDLL.h"

#include <iostream>

void DLL::DoStuff()
{
	_myInt += 2;
	std::cout << _myInt << std::endl;
}

extern "C" IDLL* CreateDLL()
{
	return new DLL;
}