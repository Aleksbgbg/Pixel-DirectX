#pragma once

#include <Windows.h>

class Window
{
public:
	Window(HINSTANCE programInstanceHandle, const char* windowClassName, const char* windowName, RECT dimensions);

public:
	~Window();

private:
	HINSTANCE programInstanceHandle;
	const char* windowClassName;
	HWND handle;
};