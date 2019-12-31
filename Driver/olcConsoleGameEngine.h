#ifndef OLC_CONSOLE_GAME_ENGINE_HDR
#define OLC_CONSOLE_GAME_ENGINE_HDR

#include <windows.h>

#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>

//Standard namespace for static colour and "pixel" types.
namespace 
{

class ConsoleGameEngine;

enum COLOUR
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007, // Thanks MS :-/
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_DARK_BLUE = 0x0010,
	BG_DARK_GREEN = 0x0020,
	BG_DARK_CYAN = 0x0030,
	BG_DARK_RED = 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW = 0x0060,
	BG_GREY = 0x0070,
	BG_DARK_GREY = 0x0080,
	BG_BLUE = 0x0090,
	BG_GREEN = 0x00A0,
	BG_CYAN = 0x00B0,
	BG_RED = 0x00C0,
	BG_MAGENTA = 0x00D0,
	BG_YELLOW = 0x00E0,
	BG_WHITE = 0x00F0,
};

enum PIXEL_TYPE 
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};

const int TOTAL_KEYS = 256;
const int TOTAL_MOUSE_PRESSES = 5;

std::atomic<bool> ATOM_ACTIVE = false;
std::condition_variable GAME_FINISHED;
std::mutex MUX_GAME;

}; //TED


//Taking the One Lone Coders engine and copying it.
namespace olc
{

class Sprite 
{
public:
	Sprite() : _width(0), _height(0)
	{}

	Sprite(int w, int h) : 
		_glyphs(std::make_unique<short[]>(w * h)),
		_colours(std::make_unique<short[]>(w * h))
	{
		create();
	}

	Sprite(const std::wstring &sFile, int w, int h) : 
		_width(w), _height(h), 
		_glyphs(std::make_unique<short[]>(w * h)),
		_colours(std::make_unique<short[]>(w * h))
	{
		if (!load(sFile)) 
		{
			//Note create should be 8, 8.
			create();
		}
	}

	void setGlyph(int x, int y, short c) 
	{
		if (x >= 0 || x < _width || y >= 0 || y < _height)
		{
			_glyphs[y * _width + x] = c;
		}
	}

	void setColour(int x, int y, short c) 
	{
		if (x >= 0 || x < _width || y >= 0 || y < _height) 
		{
			_colours[y * _width + x] = c;
		}
	}

	short getGlyph(int x, int y)
	{
		if (x >= 0 || x < _width || y >= 0 || y < _height)
		{
			return _glyphs[y * _width + x];
		}
	}

	short getColour(int x, int y)
	{
		if (x >= 0 || x < _width || y >= 0 || y < _height)
		{
			return _colours[y * _width + x];
		}
	}

	short sampleGlyph(float x, float y) 
	{
		short result = 0; 
		int sx = static_cast<int>(x * static_cast<float>(_width));
		int sy = static_cast<int>(y * static_cast<float>(_height - 1.f));

		if (sx < 0 || sx >= _width || sy < 0 || sy >= _height) 
		{
			result = L' ';
		}
		else 
		{
			result = _glyphs[sy * _width + sx];
		}

		return result;
	}

	short sampleColour(float x, float y) 
	{
		short result = 0;
		int sx = static_cast<int>(x * static_cast<float>(_width));
		int sy = static_cast<int>(y * static_cast<float>(_height - 1.f));

		if (sx < 0 || sx >= _width || sy < 0 || sy >= _height)
		{
			result = FG_BLACK;
		}
		else
		{
			result = _colours[sy * _width + sx];
		}

		return result;
	}

	bool save(const std::wstring &sFile) 
	{
		bool success = false;
		FILE *f = nullptr;
		_wfopen_s(&f, sFile.c_str(), L"wb");

		if (f != nullptr) 
		{
			fwrite(&_width, sizeof(int), 1, f);
			fwrite(&_height, sizeof(int), 1, f);

			fwrite(_colours.get(), sizeof(short), _width * _height, f);
			fwrite(_glyphs.get(), sizeof(short), _width * _height, f);

			fclose(f);

			success = true;
		}

		return success;
	}

	//I have gone with smart pointer. I am not sure this function can work the same
	//as I am not sure we can use smart pointers like this.
	//TODO: come up with a design pattern that works for this type of thing.
	//Also fix the clearly broken function
	bool load(const std::wstring &sFile) 
	{
		bool success = false;
		_height = 0;
		_width = 0;

		FILE *f = nullptr;
		_wfopen_s(&f, sFile.c_str(), L"wb");
		if (f != nullptr) 
		{
			std::fread(&_width, sizeof(int), 1, f);
			std::fread(&_height, sizeof(int), 1, f);

			create();

			std::fread(_colours.get(), sizeof(short), _width * _height, f);
			std::fread(_glyphs.get(), sizeof(short), _width * _height, f);

			std::fclose(f);

			success = true;
		}

		return success;
	}

	int _width;
	int _height;

private:

	void create()
	{
		for (int i = 0; i < _width * _height; i++)
		{
			_glyphs[i] = L' ';
			_colours[i] = FG_BLACK;
		}
	}

	std::unique_ptr<short[]> _glyphs;
	std::unique_ptr<short[]> _colours;
};

class ConsoleGameEngine
{
protected:

	struct KeyState
	{
		bool pressed;
		bool released;
		bool held;
	} _keys[TOTAL_KEYS], _mouse[TOTAL_MOUSE_PRESSES];

public:
	ConsoleGameEngine(int w, int h, int fontW, int fontH) : _screenWidth(w), _screenHeight(h),
		_consoleInFocus(true),
		_enableSound(false), _mousePosX(0), _mousePosY(0), _appName(L"Default")
	{
		_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		_hConsoleIn = GetStdHandle(STD_OUTPUT_HANDLE);

		std::memset(_keyNewState, 0, TOTAL_KEYS * sizeof(short));
		std::memset(_keyOldState, 0, TOTAL_KEYS * sizeof(short));
		std::memset(_keys, 0, TOTAL_KEYS * sizeof(KeyState));

		constructConsole(fontW, fontH);
	}

	void enableSound()
	{
		_enableSound = true;
	}

	int constructConsole(int fontw, int fonth)
	{
		int errorCode = 1;
		if (_hConsole == INVALID_HANDLE_VALUE)
		{
			errorCode = errorMsg(L"Bad Handle");
		}

		//_rectWindow = { 0, 0, 1, 1 };
		//SetConsoleWindowInfo(_hConsole, TRUE, &_rectWindow);

		//Here we set the screen buffer up.
		COORD coord = { static_cast<short>(_screenWidth), static_cast<short>(_screenHeight) };

		if (!SetConsoleScreenBufferSize(_hConsole, coord))
		{
			errorCode = errorMsg(L"Bad Handle");
		}

		//Assign screen buffer to the console.
		if (!SetConsoleActiveScreenBuffer(_hConsole))
		{
			errorCode = errorMsg(L"SetConsoleActiveScreenBuffer");
		}

		//Set the font size.
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = fontw;
		cfi.dwFontSize.Y = fonth;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;

		//DWORD version = GetVersion();
		//DWORD major = static_cast<DWORD>(LOBYTE(LOWORD(version)));
		//DWORD minor = static_cast<DWORD>(HIBYTE(LOWORD(version)));

		wcscpy(cfi.FaceName, L"Consolas");
		if (!SetCurrentConsoleFontEx(_hConsole, false, &cfi))
		{
			errorCode = errorMsg(L"SetCurrentConsoleFontEx");
		}

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(_hConsole, &csbi))
		{
			errorCode = errorMsg(L"GetConsoleScreenBufferInfo");
		}

		if (_screenHeight > csbi.dwMaximumWindowSize.Y)
		{
			errorCode = errorMsg(L"Screen Height / FontHeight too large");
		}

		if (_screenWidth > csbi.dwMaximumWindowSize.X)
		{
			errorCode = errorMsg(L"Screen Width / FontWidth too large");
		}

		_rectWindow = { 0, 0, static_cast<short>(_screenWidth) - 1, static_cast<short>(_screenHeight) - 1 };
		if (!SetConsoleWindowInfo(_hConsole, TRUE, &_rectWindow)) 
		{
			errorCode = errorMsg(L"SetConsoleWindowInfo");
		}

		if (!SetConsoleMode(_hConsole, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
		{
			errorCode = errorMsg(L"SetConsoleMode");
		}

		_bufScreen = new CHAR_INFO[_screenWidth * _screenHeight];
		memset(_bufScreen, 0, sizeof(CHAR_INFO) * _screenWidth * _screenHeight);

		SetConsoleCtrlHandler(reinterpret_cast<PHANDLER_ROUTINE>(closeHandler), TRUE);

		return errorCode;
	}

	void draw(int x, int y, short c = 0x2588, short col = 0x000F)
	{
		if (x >= 0 && x < _screenWidth && y >= 0 && y < _screenHeight)
		{
			_bufScreen[y * _screenWidth + x].Char.UnicodeChar = c;
			_bufScreen[y * _screenWidth + x].Attributes = col;
		}
	}

	void clip(int &x, int &y)
	{
		if (x < 0)
		{
			x = 0;
		}

		if (x > _screenWidth)
		{
			x = _screenWidth;
		}

		if (y < 0)
		{
			y = 0;
		}

		if (y > _screenHeight)
		{
			y = _screenHeight;
		}
	}

	void fill(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F)
	{
		clip(x1, y1);
		clip(x2, y2);
		for (int x = x1; x < x2; x++)
		{
			for (int y = y1; y < y2; y++)
			{
				draw(x, y, c, col);
			}
		}
	}

	void drawLine(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F)
	{
		int x, y, deltaX, deltaY, deltaAbsX, deltaAbsY, px, py, xe, ye;
		deltaX = x2 - x1;
		deltaY = y2 - y1;

		deltaAbsX = std::abs(deltaX);
		deltaAbsY = std::abs(deltaY);

		px = 2 * (deltaAbsX - deltaAbsY);
		py = 2 * (deltaAbsY - deltaAbsX);

		if (deltaAbsY <= deltaAbsX)
		{
			if (deltaX >= 0)
			{
				x = x1;
				y = y1;
				xe = x2;
			}
			else
			{
				x = x2;
				y = y2;
				xe = x1;
			}

			draw(x, y, c, col);

			for (int i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
				{
					px = px + 2 * deltaAbsY;
				}
				else
				{
					if ((deltaX < 0 && deltaY < 0) || (deltaX > 0 && deltaY > 0))
					{
						y = y + 1;
					}
					else
					{
						y = y - 1;
					}

					px = px + 2 * (deltaAbsY - deltaAbsX);
				}

				draw(x, y, c, col);
			}
		}
		else
		{
			if (deltaY >= 0)
			{
				x = x1;
				y = y1;
				ye = y2;
			}
			else
			{
				x = x2;
				y = y2;
				ye = y1;
			}

			draw(x, y, c, col);

			for (int i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
				{
					py = py + 2 * deltaAbsX;
				}
				else
				{
					if ((deltaX < 0 && deltaY < 0) || (deltaX > 0 && deltaY > 0))
					{
						x = x + 1;
					}
					else
					{
						x = x - 1;
					}

					py = py + 2 * (deltaAbsX - deltaAbsY);
				}

				draw(x, y, c, col);
			}
		}
	}

	void drawString(int x, int y, const std::wstring &c, short col = 0x000F)
	{
		for (std::size_t i = 0; i < c.size(); i++) 
		{
			_bufScreen[y * _screenWidth + x + i].Char.UnicodeChar = c.at(i);
			_bufScreen[y * _screenWidth + x + i].Attributes = col;
		}
	}

	void drawStringAlpha(int x, int y, const std::wstring &c, short col = 0x000F) 
	{
		for (std::size_t i = 0; i < c.size(); i++)
		{
			if (c[i] != L' ')
			{
				wchar_t character = c.at(i);
				int index = (y * _screenWidth) + x + i;
				_bufScreen[index].Char.UnicodeChar = character;
				_bufScreen[index].Attributes = col;
			}
		}
	}

	void drawSprite(int x, int y, Sprite *sprite) 
	{
		if (sprite != nullptr) 
		{
			for (int i = 0; i < sprite->_width; i++) 
			{
				for (int j = 0; j < sprite->_height; j++)
				{
					if (sprite->getGlyph(i, j) != L' ') 
					{
						draw(x + i, y + j, sprite->getGlyph(i, j), sprite->getColour(i, j));
					}
				}
			}
		}
	}

	void start() 
	{
		ATOM_ACTIVE = true; 
		std::thread t = std::thread(&ConsoleGameEngine::gameThread, this);

		t.join();
	}

	void setScreenWidth(int w) { _screenWidth = w; }
	void setScreenHeight(int h) { _screenHeight = h; }
	int getScreenWidth() const { return _screenWidth; }
	int getScreenHeight() const { return _screenHeight; }
	KeyState getKey(int key) { if (sizeof(_keys) > key) return _keys[key];  }

	virtual bool onUserCreate() = 0;
	virtual bool onUserUpdate(float elapsedTime) = 0;

	//optional clean up
	virtual bool onUserDestroy() { return true; }
protected:

	int errorMsg(const wchar_t *msg)
	{
		char buf[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
		SetConsoleActiveScreenBuffer(_hOriginalConsole);
		printf("ERROR: %s\n\t%s\n", msg, buf);
		return 0;
	}


	static bool closeHandler(DWORD evt)
	{
		// Note this gets called in a seperate OS thread, so it must
		// only exit when the game has finished cleaning up, or else
		// the process will be killed before OnUserDestroy() has finished
		if (evt == CTRL_CLOSE_EVENT)
		{
			//atomActive = false;

			//// Wait for thread to be exited
			//std::unique_lock<std::mutex> ul(m_muxGame);
			//cvGameFinished.wait(ul);
		}

		return true;
	}

	int _mousePosX;
	int _mousePosY;

    int _screenWidth;
    int _screenHeight;
    
    CHAR_INFO *_bufScreen;
	std::wstring _appName;
	HANDLE _hOriginalConsole;
	CONSOLE_SCREEN_BUFFER_INFO _originalConsoleInfo;
	HANDLE _hConsole;
	HANDLE _hConsoleIn;
	SMALL_RECT _rectWindow;
	short _keyOldState[TOTAL_KEYS] = { 0 };
	short _keyNewState[TOTAL_KEYS] = { 0 };
	bool _mouseOldState[TOTAL_MOUSE_PRESSES] = { 0 };
	bool _mouseNewState[TOTAL_MOUSE_PRESSES] = { 0 };
	bool _consoleInFocus;
	bool _enableSound;

private:
	void gameThread() 
	{
		//Create user resources as part of this thread.
		if (! onUserCreate()) 
		{
			ATOM_ACTIVE = false;
		}

		//TODO: Create the audio for this game engine
		//// Check if sound system should be enabled
		//if (m_bEnableSound)
		//{
		//	if (!CreateAudio())
		//	{
		//		m_bAtomActive = false; // Failed to create audio system			
		//		m_bEnableSound = false;
		//	}
		//}

		auto tp1 = std::chrono::system_clock::now();
		auto tp2 = std::chrono::system_clock::now();

		while (ATOM_ACTIVE) 
		{
			while (ATOM_ACTIVE) 
			{
				//Now we need to handle timing.
				tp2 = std::chrono::system_clock::now();
				std::chrono::duration<float> elasedTime = tp2 - tp1;
				tp1 = tp2;
				float elapsedTime = elasedTime.count();

				for (int i = 0; i < TOTAL_KEYS; i++) 
				{
					_keyNewState[i] = GetAsyncKeyState(i);

					_keys[i].pressed = false;
					_keys[i].released = false;

					if (_keyNewState[i] != _keyOldState[i]) 
					{
						if (_keyNewState[i] & 0x8000) 
						{
							_keys[i].pressed = !(_keys[i].held);
							_keys[i].held = true;
						}
						else 
						{
							_keys[i].released = true;
							_keys[i].held = false;
						}
					}

					_keyOldState[i] = _keyNewState[i];
				}

				//Handle mouse events
				INPUT_RECORD inBuf[32];
				DWORD events = 0;
				GetNumberOfConsoleInputEvents(_hConsole, &events);
				if (events > 0) 
				{
					ReadConsoleInput(_hConsole, inBuf, events, &events);
				}

				//Handle event of clicks and movement
				for (DWORD i = 0; i < events; i++) 
				{
					switch (inBuf[i].EventType)
					{
					case FOCUS_EVENT:
						_consoleInFocus = inBuf[i].Event.FocusEvent.bSetFocus;
					break;

					case MOUSE_EVENT:
						switch (inBuf[i].Event.MouseEvent.dwEventFlags) 
						{
						case MOUSE_MOVED:
							_mousePosX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
							_mousePosY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
						break;

						case 0:
							for (int m = 0; m < TOTAL_MOUSE_PRESSES; m++)
							{
								_mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
							}
						break;

						default:
						break;

						}
					break;

					default:
					break;
					}
				}

				for (int m = 0; m < TOTAL_MOUSE_PRESSES; m++) 
				{
					_mouse[m].pressed = false;
					_mouse[m].released = false;

					if (_mouseNewState[m] != _mouseOldState[m]) 
					{
						if (_mouseNewState[m]) 
						{
							_mouse[m].pressed = true;
							_mouse[m].held = true;
						}
						else 
						{
							_mouse[m].released = true;
							_mouse[m].held = false;
						}
					}

					_mouseOldState[m] = _mouseNewState[m];
				}

				if (!onUserUpdate(elapsedTime)) 
				{
					ATOM_ACTIVE = false;
				}

				wchar_t s[256];
				swprintf_s(s, L"One Lone Code Engine Revision - %s - FPS: %3.2f",
						  _appName.c_str(), 1.f / elapsedTime);
				//SetConsoleTitle(s);
				WriteConsoleOutput(_hConsole, _bufScreen, 
								  { static_cast<short>(_screenWidth), static_cast<short>(_screenHeight) },
								  {0, 0}, &_rectWindow);
			}

			if (_enableSound) 
			{
				//TODO: Clean up audio
			}

			if (onUserDestroy()) 
			{
				delete[] _bufScreen;
				SetConsoleActiveScreenBuffer(_hOriginalConsole);
				GAME_FINISHED.notify_one();
			}
			else 
			{
				ATOM_ACTIVE = true;
			}
		}
	}
};

}; //olc

#endif 