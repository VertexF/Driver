#include "Driver.h"

Driver::Driver(int w, int h, int FontW, int FontH) : 
	olc::ConsoleGameEngine(w, h, FontW, FontH)
{
	
}

bool Driver::onUserCreate()
{
	return true;
}

bool Driver::onUserUpdate(float elapsedTime)
{
	//for (int y = 0; y < getScreenHeight() / 2; y++)
	//{
	//	for (int x = 0; x < getScreenWidth(); x++) 
	//	{
	//		float midPoint = 0.5f;
	//		float roadWidth = 0.6f;
	//		float clipWidth = roadWidth * 0.15f;

	//		roadWidth *= 0.5f;
	//	}
	//}

	//draw(10, 10, 0xDB, 0x000A);
	//fill(0, 0, 160, 100, 0xDB, 0x00B);
	drawLine(0, 0, 160, 100);
	return true;
}