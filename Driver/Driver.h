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
	float curv;
	float speed;

	float playerCurv;
	float targetCuvr;
	float trackDis;

	float currentLapTime;

	//Curvature, distance 
	std::vector<std::pair<float, float>> trackSec;
	std::list<float> lapTimes;
};

#endif // !DRIVER_HDR
