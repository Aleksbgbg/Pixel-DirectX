#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Color.hpp"

namespace Px
{
class Graphics
{
public:
	Graphics();

	Graphics(HWND windowHandle, const RECT windowDimensions);

	Graphics(const Graphics&) = delete;
	Graphics(Graphics&& source) noexcept;

public:
	~Graphics();

public:
	Graphics& operator=(const Graphics&) = delete;
	Graphics& operator=(Graphics&& source) noexcept;

public:
	void Clear() const;
	void Render() const;

	void PutPixel(const int x, const int y, const Color color) const;
	void PutPixel(const int x, const int y, const unsigned char red, const unsigned char green, const unsigned char blue) const;

private:
	struct Vertex
	{
		float x, y, z;	// position
		float u, v;		// texcoord
	};

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;

	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> textureBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureBufferView;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	int width;
	int height;

	Color* buffer;
};
}