#ifndef DRIVER_HDR
#define DRIVER_HDR

#include "olcConsoleGameEngine.h"

class Driver : public olc::ConsoleGameEngine
{
public:
	Driver();
protected:
	virtual bool onUserCreate() override;
	virtual bool onUserUpdate(float elapsedTime) override;
};

#endif // !DRIVER_HDR
