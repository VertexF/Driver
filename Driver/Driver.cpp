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
		carDis += 100.f * elapsedTime;
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
			int grassColour = std::sin(20.f * std::pow(1.f - perspective, 3) + carDis * 0.1f) > 0.0f ? FG_GREEN : FG_DARK_GREEN;
			int curbColour = std::sin(80.f * std::pow(1.f - perspective, 2) + carDis) > 0.0f ? FG_RED : FG_WHITE;

			//Left side of the road
			if (x >= 0 && x < letfGrass)
			{
				draw(x, row, 0xDB, grassColour);
			}
			if (x >= letfGrass && x < leftClip)
			{
				draw(x, row, 0xDB, curbColour);
			}

			//The road itself
			if (x >= leftClip && x < rightClip)
			{
				draw(x, row, 0xDB, FG_GREY);
			}

			//Right side of the road
			if (x >= rightClip && x < rightGrass)
			{
				draw(x, row, 0xDB, curbColour);
			}
			if (x >= rightGrass && x < getScreenWidth())
			{
				draw(x, row, 0xDB, grassColour);
			}
		}
	}

	carPos = (getScreenWidth() / 2) + (static_cast<int>(getScreenWidth() * carPos) / 2.f) - 7;

	drawStringAlpha(carPos, 80, L"   ||####||   ");
	drawStringAlpha(carPos, 81, L"      ##      ");
	drawStringAlpha(carPos, 82, L"     ####     ");
	drawStringAlpha(carPos, 83, L"     ####     ");
	drawStringAlpha(carPos, 84, L"|||  ####  |||");
	drawStringAlpha(carPos, 85, L"|||########|||");
	drawStringAlpha(carPos, 86, L"|||  ####  |||");

	return true;
}