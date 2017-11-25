#pragma once

#include "platform.h"

#if USE( DX12_API )

// interfaces
typedef ID3D12Debug					gfx_debug_t;
typedef ID3D12Device				gfx_device_t;
typedef ID3D12CommandList			cmdlist_t;
typedef ID3D12GraphicsCommandList	gfx_cmdlist_t;
typedef ID3D12CommandAllocator		gfx_cmdlist_alloc_t;
typedef ID3D12PipelineState			gfx_pipeline_state_t;
typedef ID3D12CommandQueue			gfx_cmdqueue_t;
typedef IDXGIAdapter1				gfx_adapter_t;
typedef IDXGISwapChain1				gfx_swapchain1_t;
typedef IDXGISwapChain2				gfx_swapchain2_t;
typedef IDXGISwapChain3				gfx_swapchain_t;
typedef IDXGIFactory4				gfx_factory_t;
typedef ID3D12DescriptorHeap		gfx_descheap_t;
typedef ID3D12Resource				gfx_resource_t;
typedef ID3D12CommandAllocator		gfx_cmdalloc_t;
typedef ID3D12RootSignature			gfx_rootsignature_t;
typedef ID3DBlob					gfx_memblob_t;
typedef ID3D12Fence					gfx_fence_t;
typedef ID3D12InfoQueue				gfx_info_queue_t;

// types
typedef D3D12_COMMAND_LIST_TYPE				gfx_cmdlist_type_t;
typedef D3D12_CPU_DESCRIPTOR_HANDLE			gfx_desc_handle_t;
typedef D3D12_DESCRIPTOR_RANGE				gfx_desc_range_t;
typedef D3D12_ROOT_DESCRIPTOR_TABLE			gfx_desc_table_t;
typedef D3D12_ROOT_PARAMETER				gfx_root_parameter_t;
typedef D3D12_INPUT_ELEMENT_DESC			gfx_input_desc_t;
typedef D3D12_GRAPHICS_PIPELINE_STATE_DESC	gfx_pipeline_desc_t;
typedef D3D12_VERTEX_BUFFER_VIEW			gfx_vert_buffer_view_t;
typedef D3D12_INDEX_BUFFER_VIEW				gfx_index_buffer_view_t;
typedef UINT64								uint64_t;
typedef HANDLE								gfx_fence_event_t;

typedef D3D12_VIEWPORT						gfx_viewport_t;
typedef D3D12_RECT							gfx_rect_t;
// extended types

#endif 