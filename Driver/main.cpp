#include "Driver.h"

int main() 
{
	Driver driver;
	driver.constructConsole(160, 100, 8, 8);

	driver.start();

	return 0;
}