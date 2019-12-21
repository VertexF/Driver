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
const int TOTAL_MOUSE_PRESSES = 5

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
public:
	ConsoleGameEngine() : _screenWidth(80), _screenHeight(30), _consoleInFocus(true),
    _enableSound(false), _mousePosX(0), _mousePosY(0), _appName(L"Default")
    {
        _hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        _hConsoleIn = GetStdHandle(STD_OUTPUT_HANDLE);
        
        std::memset(_keyNewState, 0, TOTAL_KEYS * sizeof(short));
        std::memset(_keyOldState, 0, TOTAL_KEYS * sizeof(short));
        std::memset(_keys, 0, TOTAL_KEYS * sizeof(KeyState));
    }
    
    void enableSound()
    {
        _enableSound = true;
    }
    
    int constructConsole(int width, int height, int fontw, int fonth)
    {
        if(_hConsole == INVALID_HANDLE_VALUE)
        {
            return Error(L"Bad Handle");
        }
        
        _screenWidth = width;
        _screenHeight = height;
        
        _rectWindow = {0, 0, 1, 1};
        SetConsoleWindowInfo(_hConsole, true, &_rectWindow);
        
        //Here we set the screen buffer up.
        COORD coord = {static_cast<short>(_screenWidth), static_cast<short>(_screenHeight)};
        
        if(!SetConsoleScreenBufferSize(_hConsole, coord))
        {
            Error(L"SetConsoleScreenBufferSize");
        }
        
        //Assign screen buffer to the console.
        if(!SetConsoleActiveScreenBuffer(_hConsole))
        {
            return Error(L"SetConsoleActiveScreenBuffer");
        }
        
        //Set the font size.
        CONSOLE_FONT_INFOEX cfi;
        cfi.cbSize = sizeof(cfi);
        cfi.nFont = 0;
        cfi.dwFontSize.X = fontw;
        cfi.dwFontSize.Y = fonth;
        cfi.FontFamily = FF_DONTCARE;
        cfi.FontWeight = FW_Normal;
        
        DWORD version = GetVersion();
        DWORD major static_cast<DWORD>(LOBYTE(LOWORD(version)));
        DWORD minor = static_cast<DWORD>(HIBYTE(LOWORD(version)));
        
        wcscpy(cfi.FaceName, L"Console");
        if(!SetCurrentConsoleFontEx(_hConsole, false, &cfi))
        {
            return Error(L"SetCurrentConsoleFontEx");
        }
        
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if(!GetConsoleScreenBufferInfo(_hConsole, &csbi))
        {
            return Error(L"GetConsoleScreenBufferInfo");
        }
        
        if(_screenHeight > csbi.dwMaximumWindowSize.Y)
        {
            return Error(L"Screen Height / FontHeight too large");
        }
        
        if(_screenWidth > csbi.dwMaximumWindowSize.X)
        {
            return Error(L"Screen Width / FontWidth too large");
        }
        
        _bufScreen = new CHAR_INFO[_screenWidth * _screenHeight];
        memset(_bufScreen, 0, sizeof(CHAR_INFO) * _screenWidth * _screenHeight);
        
        SetConsoleCtrlHandler(static_cast<PHANDLER_ROUTINE>(CloseHandler), true);
        
        return 1;
    }
    
    void draw(int x, int y, short c = 0x2588, short col = 0x000F)
    {
        if(x >= 0 && x < _screenWidth && y >= 0 && y < _screenHeight)
        {
            _bufScreen[y * _screenWidth + x].Char.UnicodeChar = c;
            _bufScreen[y * _screenWidth + x].Attributes = col;
        }
    }
protected:

	struct KeyState
	{
		bool pressed;
		bool released;
		bool held;
	} _keys[TOTAL_KEYS], _mouse[TOTAL_MOUSE_PRESSES];

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
};

}; //olc

#endif 