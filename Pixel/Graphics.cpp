#include "Graphics.h"

#include <cassert>
#include <stdexcept>

Graphics::Graphics(HWND windowHandle, const RECT windowDimensions)
{
	assert(windowHandle != nullptr);

	HRESULT resultHandle;

	// Create D3D device and swapchain
	{
		DXGI_SWAP_CHAIN_DESC  swapchainDescription = { };
		swapchainDescription.BufferCount = 1;
		swapchainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDescription.BufferDesc.Width = windowDimensions.right - windowDimensions.left;
		swapchainDescription.BufferDesc.Height = windowDimensions.bottom - windowDimensions.top;
		swapchainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapchainDescription.OutputWindow = windowHandle;
		swapchainDescription.SampleDesc.Count = 1;
		swapchainDescription.SampleDesc.Quality = 0;
		swapchainDescription.Windowed = TRUE;

		resultHandle = D3D11CreateDeviceAndSwapChain
		(
			nullptr,						// Video adapter - use default
			D3D_DRIVER_TYPE_HARDWARE,		// Driver type - D3D_DRIVER_TYPE_HARDWARE uses GPU
			nullptr,						// Software rasterizer handle - none since using hardware rendering
			0u,								// Runtime layers to enable - D3D11_CREATE_DEVICE_DEBUG outputs debug messages
			nullptr,						// Array of feature levels - use default
			0u,								// Quantity of feature levels - none since nullptr passed
			D3D11_SDK_VERSION,				// DirectX SDK version - always D3D11_SDK_VERSION
			&swapchainDescription,			// Swapchain description
			swapchain.GetAddressOf(),		// Swapchain
			device.GetAddressOf(),			// DirectX device
			nullptr,						// First feature level supported by device (OUT) - not needed
			deviceContext.GetAddressOf()	// DirectX device context
		);

		if (FAILED(resultHandle))
		{
			throw std::runtime_error{ "Could not create D3D device and swapchain" };
		}
	}
}