#include "Window.h"

#include <stdexcept>

Window::Window(HINSTANCE programInstanceHandle, const char* windowClassName, const char* windowName, RECT dimensions)
	:
	programInstanceHandle{ programInstanceHandle },
	windowClassName{ windowClassName }
{
	WNDCLASS windowClass = { };
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
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
		this								// lparam (allows access to window instance in static WindowProcedure method)
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