#pragma once

#include <d3d11.h>
#include <wrl.h>

class Graphics
{
public:
	Graphics(HWND windowHandle, const RECT windowDimensions);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;

	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
};