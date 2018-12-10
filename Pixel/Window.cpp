#include "Window.h"

#include <cassert>
#include <stdexcept>

Window::Window(HINSTANCE programInstanceHandle, const char* windowClassName, const char* windowName, RECT dimensions)
	:
	programInstanceHandle{ programInstanceHandle },
	windowClassName{ windowClassName }
{
	WNDCLASS windowClass = { };
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = &CreateWindowProcedure;
	windowClass.hInstance = programInstanceHandle;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = windowClassName;

	if (RegisterClass(&windowClass) == 0)
	{
		throw std::runtime_error{ "Could not register window class" };
	}

	AdjustWindowRect(&dimensions, WS_OVERLAPPEDWINDOW, false);

	handle = CreateWindowEx
	(
		0,									// extended styles
		windowClass.lpszClassName,			// window class name
		windowName,							// window name
		WS_OVERLAPPEDWINDOW,				// window style
		dimensions.left,					// x coordinate
		dimensions.top,						// y coordinate
		dimensions.right - dimensions.left, // width
		dimensions.bottom - dimensions.top, // height
		nullptr,							// parent window
		nullptr,							// menu
		programInstanceHandle,				// HINSTANCE of program
		this								// param (allows access to window instance in static WindowProcedure method)
	);

	if (handle == nullptr)
	{
		throw std::runtime_error{ "Could not create window" };
	}
}

Window::~Window()
{
	UnregisterClass(windowClassName, programInstanceHandle);
}

void Window::Show() const
{
	ShowWindow(handle, SW_SHOW);
}

LRESULT Window::CreateWindowProcedure(HWND windowHandle, const UINT message, const WPARAM wparam, const LPARAM lparam)
{
	// Use Window* passed in to CreateWindow to store Window* at WinAPI side, for later retrieval and dispatch
	if (message == WM_NCCREATE)
	{
		const CREATESTRUCT* const createStruct = reinterpret_cast<CREATESTRUCT*>(lparam);

		const Window* const window = reinterpret_cast<Window*>(createStruct->lpCreateParams);

		assert(window != nullptr);

		SetWindowLongPtr(windowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&DefaultWindowProcedure));
		SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));

		return window->HandleMessage(windowHandle, message, wparam, lparam);
	}

	return DefWindowProc(windowHandle, message, wparam, lparam);
}

LRESULT Window::DefaultWindowProcedure(HWND windowHandle, const UINT message, const WPARAM wparam, const LPARAM lparam)
{
	const Window* const window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));

	return window->HandleMessage(windowHandle, message, wparam, lparam);
}

LRESULT Window::HandleMessage(HWND windowHandle, const UINT message, const WPARAM wparam, const LPARAM lparam) const
{
	switch (message)
	{
	case WM_CLOSE:
		DestroyWindow(windowHandle);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(windowHandle, message, wparam, lparam);
	}

	return 0;
}