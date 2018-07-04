#pragma once

#include "basics.h"

class Utility
{
public:

	/*Prints an error message to the error output stream*/
	static void SetError(string errorMsg)
	{
		cerr << errorMsg << endl;
	}
};
