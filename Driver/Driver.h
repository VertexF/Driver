#ifndef DRIVER_HDR
#define DRIVER_HDR

#include "olcConsoleGameEngine.h"

class Driver : public olc::ConsoleGameEngine
{
public:
	Driver(int w, int h, int FontW, int FontH);
protected:
	virtual bool onUserCreate() override;
	virtual bool onUserUpdate(float elapsedTime) override;
};

#endif // !DRIVER_HDR
