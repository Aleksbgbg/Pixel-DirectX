#include "Graphics.h"

#include <cassert>
#include <stdexcept>

namespace Shaders
{
#include "PixelShader.shh"
#include "VertexShader.shh"
}

Graphics::Graphics(HWND windowHandle, const RECT windowDimensions)
	:
	width{ windowDimensions.right - windowDimensions.left },
	height{ windowDimensions.bottom - windowDimensions.top }
{
	assert(windowHandle != nullptr);

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

	// Create render target texture
	{
		D3D11_TEXTURE2D_DESC textureDescription;
		textureDescription.Width = width;
		textureDescription.Height = height;
		textureDescription.MipLevels = 1u;
		textureDescription.ArraySize = 1u;
		textureDescription.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDescription.SampleDesc.Count = 1u;
		textureDescription.SampleDesc.Quality = 0u;
		textureDescription.Usage = D3D11_USAGE_DYNAMIC;
		textureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		textureDescription.MiscFlags = 0u;

		resultHandle = device->CreateTexture2D(&textureDescription, nullptr, textureBuffer.GetAddressOf());

		if (FAILED(resultHandle))
		{
			throw std::runtime_error{ "Could not create render target texture" };
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescription = { };
		shaderResourceViewDescription.Format = textureDescription.Format;
		shaderResourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDescription.Texture2D.MipLevels = textureDescription.MipLevels;

		resultHandle = device->CreateShaderResourceView(textureBuffer.Get(), &shaderResourceViewDescription, textureBufferView.GetAddressOf());

		if (FAILED(resultHandle))
		{
			throw std::runtime_error{ "Could not create view for render target texture" };
		}
	}

	// Create and pixel and vertex shaders
	{
		resultHandle = device->CreatePixelShader
		(
			Shaders::PixelShaderBytecode,
			sizeof(Shaders::PixelShaderBytecode),
			nullptr,
			pixelShader.GetAddressOf()
		);

		if (FAILED(resultHandle))
		{
			throw std::runtime_error{ "Could not create pixel shader" };
		}

		resultHandle = device->CreateVertexShader
		(
			Shaders::VertexShaderBytecode,
			sizeof(Shaders::VertexShaderBytecode),
			nullptr,
			vertexShader.GetAddressOf()
		);

		if (FAILED(resultHandle))
		{
			throw std::runtime_error{ "Could not create vertex shader" };
		}

		deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0u);
		deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0u);

		deviceContext->PSSetShaderResources(0u, 1u, textureBufferView.GetAddressOf());
	}

	// Create vertex buffer with rendering quad
	{
		const Vertex renderQuad[] =
		{
			{ -1.0f,  1.0f, 0.5f, 0.0f, 0.0f },
			{  1.0f,  1.0f, 0.5f, 1.0f, 0.0f },
			{  1.0f, -1.0f, 0.5f, 1.0f, 1.0f },
			{ -1.0f,  1.0f, 0.5f, 0.0f, 0.0f },
			{  1.0f, -1.0f, 0.5f, 1.0f, 1.0f },
			{ -1.0f, -1.0f, 0.5f, 0.0f, 1.0f }
		};

		D3D11_BUFFER_DESC bufferDescription = { };
		bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDescription.ByteWidth = sizeof(renderQuad);
		bufferDescription.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA subresourceData = { };
		subresourceData.pSysMem = renderQuad;

		resultHandle = device->CreateBuffer(&bufferDescription, &subresourceData, vertexBuffer.GetAddressOf());

		if (FAILED(resultHandle))
		{
			throw std::runtime_error{ "Could not create vertex buffer" };
		}

		const UINT stride = sizeof(Vertex);
		const UINT offset = 0u;

		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &stride, &offset);
	}

	// Create input layout for quad
	{
		const D3D11_INPUT_ELEMENT_DESC inputElementDescription[] =
		{
			{ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
			{ "TEXCOORD", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0u }
		};

		resultHandle = device->CreateInputLayout
		(
			inputElementDescription,
			sizeof(inputElementDescription) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			Shaders::VertexShaderBytecode,
			sizeof(Shaders::VertexShaderBytecode),
			inputLayout.GetAddressOf()
		);

		if (FAILED(resultHandle))
		{
			throw std::runtime_error{ "Could not create input layout" };
		}

		deviceContext->IASetInputLayout(inputLayout.Get());
	}

	// Create sampler state for textured quad
	{
		D3D11_SAMPLER_DESC samplerDescription = { };
		samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDescription.MinLOD = 0.0f;
		samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

		resultHandle = device->CreateSamplerState(&samplerDescription, samplerState.GetAddressOf());

		if (FAILED(resultHandle))
		{
			throw std::runtime_error{ "Could not create sampler state" };
		}

		deviceContext->PSSetSamplers(0u, 1u, samplerState.GetAddressOf());
	}

	// Allocate color buffer (16-byte aligned for fast access)
	buffer = reinterpret_cast<Color*>(_aligned_malloc(sizeof(Color) * width * height, 16u));
}

Graphics::~Graphics()
{
	if (buffer != nullptr)
	{
		_aligned_free(buffer);
		buffer = nullptr;
	}
}

void Graphics::Render() const
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource = { };

	HRESULT resultHandle = deviceContext->Map(textureBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedSubresource);

	if (FAILED(resultHandle))
	{
		throw std::runtime_error{ "Could not map texture buffer" };
	}

	Color* const colorTexture = reinterpret_cast<Color*>(mappedSubresource.pData);

	const size_t sourcePitch = width;
	const size_t bytesPerRow = sourcePitch * sizeof(Color);

	const size_t destinationPitch = mappedSubresource.RowPitch / sizeof(Color);

	const size_t height = this->height;

	for (size_t y = 0u; y < height; ++y)
	{
		memcpy(&colorTexture[y * destinationPitch], &buffer[y * sourcePitch], bytesPerRow);
	}

	deviceContext->Unmap(textureBuffer.Get(), 0u);

	deviceContext->Draw(6u, 0u);

	resultHandle = swapchain->Present(1u, 0u);

	if (FAILED(resultHandle))
	{
		if (resultHandle == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw std::runtime_error{ "GPU device removed while presenting backbuffer" };
		}

		throw std::runtime_error{ "Could not present backbuffer" };
	}
}