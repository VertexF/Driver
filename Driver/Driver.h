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
private:
	//Complete left -1, 1 is completely right.
	float carPos;
	float carDis;
};

#endif // !DRIVER_HDR
