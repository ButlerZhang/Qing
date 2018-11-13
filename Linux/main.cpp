//============================================================================
// Name        : main.cpp
// Author      : Butler
// Version     :
// Copyright   : 
// Description : 
//============================================================================

#include <iostream>
#include "src/HexString.h"

int main() 
{

	std::string ASCIIString;
	std::string TempHexString("AA119203BB");

	HexString MyHexString;
	MyHexString.HexStringToASCIIString(TempHexString, ASCIIString);

	std::cout << "Hex   String = " << TempHexString << std::endl;
	std::cout << "ASCII String = " << ASCIIString   << std::endl;

	std::cout << std::endl;
	return 0;
}
