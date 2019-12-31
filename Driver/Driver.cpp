#include "Driver.h"

#include <string>

Driver::Driver(int w, int h, int FontW, int FontH) :
	olc::ConsoleGameEngine(w, h, FontW, FontH), carPos(0.f),
	carDis(0.f), curv(0.f), speed(0.f), playerCurv(0.f), targetCuvr(0.f)
{
	
}

bool Driver::onUserUpdate(float elapsedTime)
{
	//carPos = 0;
	if (getKey(VK_UP).held)
	{
		speed += 2.f * elapsedTime;
	}
	else 
	{
		speed -= 1.f * elapsedTime;
	}

	if (getKey(VK_LEFT).held) 
	{
		playerCurv -= 0.7f * elapsedTime;
	}
	if (getKey(VK_RIGHT).held) 
	{
		playerCurv += 0.7f * elapsedTime;
	}

	if (std::fabs(playerCurv - targetCuvr) >= 0.8f)
	{
		speed -= 5.f * elapsedTime;
	}

	if (speed > 1.f) 
	{
		speed = 1.f;
	}
	if (speed < 0.f) 
	{
		speed = 0.f;
	}

	carDis += (70.f * speed) * elapsedTime;

	float offset = 0.f;
	int indexTrackSec = 0;

	while (indexTrackSec < trackSec.size() && offset <= carDis)
	{
		offset += trackSec[indexTrackSec].second;
		indexTrackSec++;
	}

	float target = trackSec[indexTrackSec - 1].first;
	float curDiff = (target - curv) * elapsedTime * speed;
	curv += curDiff;

	targetCuvr += (curv) * elapsedTime * speed;

	fill(0, 0, getScreenWidth(), getScreenHeight(), L' ', 0);

	for (int y = 0; y < getScreenHeight() / 2; ++y)
	{
		for (int x = 0; x < getScreenWidth(); ++x) 
		{
			float perspective = static_cast<float>(y) / (getScreenHeight() / 2.f);
			float midPoint = 0.5f + curv * std::pow(1.f - perspective, 3);
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

	//fCarPos = fPlayerCurvature - fTrackCurvature;
	carPos = playerCurv - targetCuvr;
	int pos = (getScreenWidth() / 2) + (static_cast<int>(getScreenWidth() * carPos) / 2.f) - 7;

	drawStringAlpha(pos, 80, L"   ||####||   ");
	drawStringAlpha(pos, 81, L"      ##      ");
	drawStringAlpha(pos, 82, L"     ####     ");
	drawStringAlpha(pos, 83, L"     ####     ");
	drawStringAlpha(pos, 84, L"|||  ####  |||");
	drawStringAlpha(pos, 85, L"|||########|||");
	drawStringAlpha(pos, 86, L"|||  ####  |||");

	drawString(0, 0, L"Distance: " + std::to_wstring(carDis));
	drawString(0, 1, L"Target Curvature: " + std::to_wstring(curv));
	drawString(0, 2, L"Player Curvature: " + std::to_wstring(playerCurv));
	drawString(0, 3, L"Player Speed: " + std::to_wstring(speed));
	drawString(0, 4, L"Track Curvature: " + std::to_wstring(targetCuvr));

	return true;
}

bool Driver::onUserCreate() 
{
	trackSec.push_back(std::pair<float, float>(0.f, 10.f)); //Start, finish
	trackSec.push_back(std::pair<float, float>(0.f, 200.f));
	trackSec.push_back(std::pair<float, float>(1.f, 200.f));
	trackSec.push_back(std::pair<float, float>(0.f, 400.f));
	trackSec.push_back(std::pair<float, float>(-1.f, 200.f));
	trackSec.push_back(std::pair<float, float>(0.f, 100.f));
	trackSec.push_back(std::pair<float, float>(-1.f, 200.f));
	trackSec.push_back(std::pair<float, float>(1.f, 200.f));

	return true;
}