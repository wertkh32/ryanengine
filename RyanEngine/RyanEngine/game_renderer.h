#pragma once
#include "universal.h"
#include "gfx_backend.h"

class Game_Renderer
{
	
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
	IDXGISwapChain3* m_swapChain;
	ID3D12Device* m_device;
	ID3D12Resource* m_renderTargets[NUM_FRAMEBUFFERS];
	ID3D12CommandAllocator* m_commandAllocators[NUM_FRAMEBUFFERS];
	ID3D12CommandQueue* m_commandQueue;
	ID3D12RootSignature* m_rootSignature;
	ID3D12DescriptorHeap* m_rtvHeap;
	ID3D12PipelineState* m_pipelineState;

	ID3D12GraphicsCommandList* m_commandList;

	uint m_rtvDescriptorSize;

	// App resources.
	ID3D12Resource* m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	// Synchronization objects.
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	ID3D12Fence* m_fence;
	UINT64 m_fenceValues[NUM_FRAMEBUFFERS];

public:
	Game_Renderer();

	void InitRenderer(_CTX_HANDLE_ hwnd);
	void InitAssets();
	void InitFrame();
	void DrawFrame();
	void WaitForGPU();
	void EndFrame();

	~Game_Renderer();
};

