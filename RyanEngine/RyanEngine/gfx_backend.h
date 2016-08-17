#pragma once
#include "universal.h"

#if USE( DX12_API )
typedef ID3D12Device gfx_device;
typedef ID3D12GraphicsCommandList gfx_cmdlist;
typedef ID3D12CommandQueue gfx_cmdqueue;
typedef IDXGIAdapter1  gfx_adapter;
typedef IDXGISwapChain1 gfx_swapchain1;
typedef IDXGISwapChain2 gfx_swapchain2;
typedef IDXGISwapChain3 gfx_swapchain3;
typedef IDXGIFactory4 gfx_factory;
typedef ID3D12DescriptorHeap gfx_descheap;
typedef ID3D12Resource gfx_resource;
typedef ID3D12CommandAllocator gfx_cmdalloc;
#endif 

static void Gfx_CreateDevice(gfx_adapter* hardwareAdapter, gfx_device** gfxDevice )
{
	DXERROR(D3D12CreateDevice(
		hardwareAdapter,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(gfxDevice)
	));
}

static void Gfx_CreateCommandQueue(gfx_device* gfxDevice, gfx_cmdqueue** gfxCmdQueue)
{
	assert(gfxDevice);
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	DXERROR(gfxDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(gfxCmdQueue)));
}

static void Gfx_CreateSwapChain(gfx_factory* factory, gfx_cmdqueue* gfxCmdQueue, _CTX_HANDLE_ hwnd, gfx_swapchain3** gfxSwapChain)
{
	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = NUM_FRAMEBUFFERS;
	swapChainDesc.Width = WINDOWED_WIDTH;
	swapChainDesc.Height = WINDOWED_HEIGHT;
	swapChainDesc.Format = GFX_FRAMEBUFFER_FORMAT;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	gfx_swapchain1* swapChain;
	DXERROR(factory->CreateSwapChainForHwnd(
		gfxCmdQueue,		// Swap chain needs the queue so that it can force a flush on it.
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	));

	DXERROR(swapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)gfxSwapChain));
}

static void Gfx_CreateDescriptorHeap(gfx_device* gfxDevice, gfx_descheap** gfxDescHeap)
{
	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = NUM_FRAMEBUFFERS;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		DXERROR(gfxDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(gfxDescHeap)));

	}
}

static void Gfx_InitFrameBuffers(gfx_device* gfxDevice, gfx_descheap* gfxDescHeap, gfx_swapchain3* gfxSwapChain, gfx_resource** gfxRenderTargets[NUM_FRAMEBUFFERS], gfx_cmdalloc** gfxCmdAllocators[NUM_FRAMEBUFFERS] )
{
	int m_frameIndex = gfxSwapChain->GetCurrentBackBufferIndex();
	uint m_rtvDescriptorSize = gfxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


	// Create frame resources.
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(gfxDescHeap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV and a command allocator for each frame.
	for (UINT n = 0; n < NUM_FRAMEBUFFERS; n++)
	{
		DXERROR(gfxSwapChain->GetBuffer(n, IID_PPV_ARGS(gfxRenderTargets[n])));
		gfxDevice->CreateRenderTargetView(*gfxRenderTargets[n], nullptr, rtvHandle);
		rtvHandle.Offset(1, m_rtvDescriptorSize);

		DXERROR(gfxDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(gfxCmdAllocators[n])));
	}
	
}

struct gfx_state
{
	gfx_cmdlist* m_commandList;

	gfx_state( const gfx_device* device )
	{
		//Init commandlist

	}
};
