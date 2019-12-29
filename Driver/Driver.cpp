#include "Driver.h"

Driver::Driver(int w, int h, int FontW, int FontH) : 
	olc::ConsoleGameEngine(w, h, FontW, FontH), carPos(0.f),
	carDis(0.f)
{
	
}

bool Driver::onUserCreate()
{
	return true;
}

bool Driver::onUserUpdate(float elapsedTime)
{
	carPos = 0;
	if (getKey(VK_UP).held)
	{
		carDis += 10.f * elapsedTime;
	}

	fill(0, 0, getScreenWidth(), getScreenHeight(), L' ', 0);

	for (int y = 0; y < getScreenHeight() / 2; ++y)
	{
		for (int x = 0; x < getScreenWidth(); ++x) 
		{
			float perspective = static_cast<float>(y) / (getScreenHeight() / 2.f);
			float midPoint = 0.5f;
			float roadWidth = 0.1f + perspective * 0.8f;
			float clipWidth = roadWidth * 0.15f;

			roadWidth *= 0.5f;

			int letfGrass = (midPoint - roadWidth - clipWidth) * getScreenWidth();
			int leftClip = (midPoint - roadWidth) * getScreenWidth();

			int rightGrass = (midPoint + roadWidth + clipWidth) * getScreenWidth();
			int rightClip = (midPoint + roadWidth) * getScreenWidth();

			int row = (getScreenHeight() / 2) + y;
			int lineColour = std::sin(20.f * std::pow(1.f - perspective, 3) + carDis * 0.1f) > 0.0f ? FG_GREEN : FG_DARK_GREEN;

			//Left side of the road
			if (x >= 0 && x < letfGrass)
			{
				draw(x, row, 0xDB, lineColour);
			}
			if (x >= letfGrass && x < leftClip)
			{
				draw(x, row, 0xDB, FG_RED);
			}

			//The road itself
			if (x >= leftClip && x < rightClip)
			{
				draw(x, row, 0xDB, FG_GREY);
			}

			//Right side of the road
			if (x >= rightClip && x < rightGrass)
			{
				draw(x, row, 0xDB, FG_RED);
			}
			if (x >= rightGrass && x < getScreenWidth())
			{
				draw(x, row, 0xDB, lineColour);
			}
		}
	}

	carPos = (getScreenWidth() / 2) + (static_cast<int>(getScreenWidth() * carPos) / 2.f) - 7;

	drawStringAlpha(carPos, 45, L"   ||####||   ");
	drawStringAlpha(carPos, 46, L"      ##      ");
	drawStringAlpha(carPos, 47, L"     ####     ");
	drawStringAlpha(carPos, 48, L"     ####     ");
	drawStringAlpha(carPos, 49, L"|||  ####  |||");
	drawStringAlpha(carPos, 50, L"|||########|||");
	drawStringAlpha(carPos, 51, L"|||  ####  |||");

	return true;
}