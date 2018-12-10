#include "Graphics.h"

#include <cassert>
#include <stdexcept>

Graphics::Graphics(HWND windowHandle, const RECT windowDimensions)
{
	assert(windowHandle != nullptr);

	const int width = windowDimensions.right - windowDimensions.left;
	const int height = windowDimensions.bottom - windowDimensions.top;

	HRESULT resultHandle;

	// Create D3D device and swapchain
	{
		DXGI_SWAP_CHAIN_DESC  swapchainDescription = { };
		swapchainDescription.BufferCount = 1;
		swapchainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDescription.BufferDesc.Width = width;
		swapchainDescription.BufferDesc.Height = height;
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

	// Create render target (backbuffer)
	{
		// Retrieve address of backbuffer in swapchain (only one backbuffer enabled)
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backbuffer;
		resultHandle = swapchain->GetBuffer(0u, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backbuffer.GetAddressOf()));

		if (FAILED(resultHandle))
		{
			throw std::runtime_error{ "Could not find backbuffer in swapchain" };
		}

		// Create render target from backbuffer
		resultHandle = device->CreateRenderTargetView(backbuffer.Get(), nullptr, renderTargetView.GetAddressOf());

		if (FAILED(resultHandle))
		{
			throw std::runtime_error{ "Could not create render target view from backbuffer" };
		}
	}

	// Set the render target to created view
	deviceContext->OMSetRenderTargets(1u, renderTargetView.GetAddressOf(), nullptr);

	// Create viewport dimensions
	{
		D3D11_VIEWPORT viewport;
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		deviceContext->RSSetViewports(1, &viewport);
	}
}