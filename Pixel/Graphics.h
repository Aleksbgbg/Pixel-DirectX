#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Color.h"

class Graphics
{
public:
	Graphics(HWND windowHandle, const RECT windowDimensions);

public:
	~Graphics();

public:
	void Clear() const;
	void Render() const;

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

	Color* buffer = nullptr;
};