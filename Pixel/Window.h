#pragma once

#include <Windows.h>

#include "Graphics.h"

class Window
{
public:
	Window(HINSTANCE programInstanceHandle, const char* windowClassName, const char* windowName, RECT dimensions);

public:
	~Window();

public:
	void Show() const;

	Graphics LoadGraphics() const;

private:
	static LRESULT __stdcall CreateWindowProcedure(HWND windowHandle, const UINT message, const WPARAM wparam, const LPARAM lparam);
	static LRESULT __stdcall DefaultWindowProcedure(HWND windowHandle, const UINT message, const WPARAM wparam, const LPARAM lparam);

private:
	LRESULT HandleMessage(HWND windowHandle, const UINT message, const WPARAM wparam, const LPARAM lparam) const;

private:
	HINSTANCE programInstanceHandle;
	const char* windowClassName;
	RECT dimensions;
	HWND handle;
};
