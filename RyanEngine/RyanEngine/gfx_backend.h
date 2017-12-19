#pragma once
#include "universal.h"
#include "gfx_globals.h"
#include "gfx_descheap.h"
#include "gfx_constantbuffers.h"
#include "gfx_math.h"
#include "gfx_resourcemanager.h"


struct GfxPerFrameData
{	// for static perframe data
	gfx_resource_t			*gfxRenderTarget;
	gfx_cmdalloc_t			*gfxCmdAllocator;

	gfx_descheap_t			*gfxCBVDescHeap;
	gfx_resource_t			*gfxConstantBuffers[GFX_CONSTANT_BUFFER_COUNT];
	byte					*gfxConstantBufferData[GFX_CONSTANT_BUFFER_COUNT];

	gfx_fence_t				*gfxEndFrameFence;
	uint64_t				gfxEndFrameFenceValue;
};


struct GfxViewInfo
{
	mat4x4 projectionMat;
	mat4x4 viewMat;
	vec3   cameraPos;
};


struct GfxPerFrameDataDyn
{	// for dynamic perframe data
	uint frameNum;
};


static gfx_factory_t* Gfx_CreateFactory()
{	// create factory to interface with hardware graphics adapter options ( gpus )
	gfx_factory_t* gfxFactory;
	DXERROR(CreateDXGIFactory1(IID_PPV_ARGS(&gfxFactory)));
	return gfxFactory;
}


static gfx_adapter_t* Gfx_SelectAdapter( gfx_factory_t* gfxFactory )
{	// return the first adapter that supports DX12
	gfx_adapter_t* _adapter;

	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != gfxFactory->EnumAdapters1(adapterIndex, &_adapter); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc;
		_adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) // no software emulator
		{
			continue;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the
		// actual device yet.
		if (SUCCEEDED(D3D12CreateDevice( _adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) 
		{
			break;
		}
	}

	return _adapter;
}


static gfx_device_t* Gfx_CreateDevice( gfx_adapter_t* hardwareAdapter )
{	// actually create the device
	gfx_device_t* gfxDevice;
	DXERROR(D3D12CreateDevice( hardwareAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &gfxDevice ) ) );
	return gfxDevice;
}


static gfx_cmdqueue_t* Gfx_CreateCommandQueue ( gfx_device_t* gfxDevice )
{	// A thread records command lists with regular draw API or command bundles
	// threads submit command lists to command queues. Command queues execute command lists
	// command queues are similar to the idea of contexts in dx11 and gnm
	gfx_cmdqueue_t* gfxCmdQueue;

	assert( gfxDevice );
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	DXERROR( gfxDevice->CreateCommandQueue( &queueDesc, IID_PPV_ARGS( &gfxCmdQueue ) ) );

	return gfxCmdQueue;
}


static gfx_swapchain_t* Gfx_CreateSwapChain( gfx_factory_t* factory, gfx_cmdqueue_t* gfxCmdQueue, ctx_handle_t hwnd )
{
	gfx_swapchain_t* gfxSwapChain;
	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = NUM_FRAMEBUFFERS;
	swapChainDesc.Width = WINDOWED_WIDTH;
	swapChainDesc.Height = WINDOWED_HEIGHT;
	swapChainDesc.Format = GFX_FRAMEBUFFER_FORMAT;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	gfx_swapchain1_t* swapChain;
	DXERROR(factory->CreateSwapChainForHwnd(
		gfxCmdQueue,		// Swap chain needs the queue so that it can force a flush on it. This used to be the device object (context)
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	));

	DXERROR( swapChain->QueryInterface( __uuidof(IDXGISwapChain3), (void**)&gfxSwapChain ) );

	return gfxSwapChain;
}


static gfx_descheap_t* Gfx_CreateRTVDescriptorHeapForRenderTargets( gfx_device_t* gfxDevice )
{
	gfx_descheap_t* gfxDescHeap;
	// Create descriptor heap for rendertargets
	// A descriptor heap is a collection of descriptors that the gpu uses to access resources
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = NUM_FRAMEBUFFERS;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		DXERROR( gfxDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS( &gfxDescHeap ) ) );
	}

	return gfxDescHeap;
}


static gfx_descheap_t* Gfx_CreateDSVDescriptorHeapForRenderTargets( gfx_device_t* gfxDevice )
{
	gfx_descheap_t* gfxDescHeap;
	// Create descriptor heap for depth stencil viewa of rendertargets
	// A descriptor heap is a collection of descriptors that the gpu uses to access resources
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = NUM_FRAMEBUFFERS;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		DXERROR( gfxDevice->CreateDescriptorHeap( &dsvHeapDesc, IID_PPV_ARGS( &gfxDescHeap ) ) );
	}

	return gfxDescHeap;
}

static void Gfx_InitFrameBuffersAndCommandAllocators(gfx_device_t* gfxDevice, gfx_descheap_t* gfxRTVDescHeap, gfx_descheap_t* gfxDSVDescHeap, gfx_swapchain_t* gfxSwapChain,
													 GfxPerFrameData perFrameData[NUM_FRAMEBUFFERS], gfx_resource_t **gfxDepthStencilTarget )
{
	uint m_rtvDescSize = gfxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Create frame resources.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle( gfxRTVDescHeap->GetCPUDescriptorHandleForHeapStart() );

	// Create a RTV and a command allocator for each frame.
	for ( uint n = 0; n < NUM_FRAMEBUFFERS; n++ )
	{
		GfxPerFrameData *curFrameData = perFrameData + n;

		// render target memory is already in the swap chain
		DXERROR( gfxSwapChain->GetBuffer( n, IID_PPV_ARGS( &curFrameData->gfxRenderTarget ) ) );

		// create views for rendertarget and depth stencil
		gfxDevice->CreateRenderTargetView( curFrameData->gfxRenderTarget, nullptr, rtvHandle );

		rtvHandle.Offset( 1, m_rtvDescSize );

		DXERROR(gfxDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &curFrameData->gfxCmdAllocator ) ) );
	}
	
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = GFX_DEPTH_STENCIL_FORMAT;
	clearValue.DepthStencil = { 0.0f, 0U };

	D3D12_DEPTH_STENCIL_VIEW_DESC depthDesc;
	depthDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthDesc.Format = GFX_DEPTH_STENCIL_FORMAT;
	depthDesc.Texture2D.MipSlice = 0;
	depthDesc.Flags = D3D12_DSV_FLAG_NONE;

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle ( gfxDSVDescHeap->GetCPUDescriptorHandleForHeapStart () );

	// depth stencil memory does not exist on the swapchain
	DXERROR ( gfxDevice->CreateCommittedResource (
		&CD3DX12_HEAP_PROPERTIES ( D3D12_HEAP_TYPE_DEFAULT ),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D ( GFX_DEPTH_STENCIL_FORMAT, WINDOWED_WIDTH, WINDOWED_HEIGHT, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL ),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS ( gfxDepthStencilTarget ) ) );

	gfxDevice->CreateDepthStencilView ( *gfxDepthStencilTarget, &depthDesc, dsvHandle );
}


/// stuff after here should be replaced by proper asseting loading stuff
static void Gfx_InitConstantBufferDescHeaps( gfx_device_t* gfxDevice, GfxPerFrameData perFrameData[NUM_FRAMEBUFFERS] )
{	// create a collection of constant buffer descriptors
	// one heap per frame

	for ( uint n = 0; n < NUM_FRAMEBUFFERS; n++ )
	{
		GfxPerFrameData *curFrameData = perFrameData + n;

		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
		cbvHeapDesc.NumDescriptors = GFX_CONSTANT_BUFFER_COUNT;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		DXERROR ( gfxDevice->CreateDescriptorHeap ( &cbvHeapDesc, IID_PPV_ARGS ( &curFrameData->gfxCBVDescHeap ) ) );
	}
}


static void Gfx_InitConstantBuffers( gfx_device_t* gfxDevice, GfxPerFrameData perFrameData[NUM_FRAMEBUFFERS] )
{
	// we need two copies for cpu-write, gpu read resources for proper double buffering
	// this is not the case for cpu only or gpu only resources.
	// constant buffers are cpu write, gpu read, so it needs two copies of the constant buffer
	uint cbufferDescSize = gfxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	

	for ( uint n = 0; n < NUM_FRAMEBUFFERS; n++ )
	{
		GfxPerFrameData *curFrameData = perFrameData + n;
		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle ( curFrameData->gfxCBVDescHeap->GetCPUDescriptorHandleForHeapStart () );

		for (uint cbufferIndex = 0; cbufferIndex < GFX_CONSTANT_BUFFER_COUNT; cbufferIndex++)
		{
			gfx_resource_t *cbuffer = curFrameData->gfxConstantBuffers[cbufferIndex];

			Gfx_CreateConstantBuffer( gfxDevice, &cbuffer, cbvHandle, g_constantBufferSizes[cbufferIndex] );

			cbvHandle.Offset(1, cbufferDescSize);

			byte *dataPtr;
			CD3DX12_RANGE readRange(0, 0); // ( begin <= end ) means not read by the cpu 
			cbuffer->Map( 0, &readRange, reinterpret_cast<void**>( &dataPtr ) );
			// use persistent map here, since heap type is upload only. Make sure cpu updates this before execute command lists is called.
			// pointer is to write-combine memory. Ensure cpu does not read this, not even accidentally.

			curFrameData->gfxConstantBufferData[cbufferIndex] = dataPtr;
		}
	}
}



static gfx_rootsignature_t* Gfx_CreateRootSignature( gfx_device_t* gfxDevice )
{	// this function sets up all the bindings required for the shader resources

	gfx_desc_range_t  descriptorTableRanges[1];															// only one range right now for const buffers, add more ranges for buffers, textures etc
	descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;								// this is a range of constant buffer views (descriptors)
	descriptorTableRanges[0].NumDescriptors = GFX_CONSTANT_BUFFER_COUNT;								// we only have one constant buffer, so the range is only 1
	descriptorTableRanges[0].BaseShaderRegister = 0;													// start index of the shader registers in the range
	descriptorTableRanges[0].RegisterSpace = 0;															// space 0. can usually be zero
	descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;  // this appends the range to the end of the root signature descriptor tables

																									   // create a descriptor table
	gfx_desc_table_t descriptorTable;
	descriptorTable.NumDescriptorRanges = _countof ( descriptorTableRanges );							// we only have one range
	descriptorTable.pDescriptorRanges = &descriptorTableRanges[0];										// the pointer to the beginning of our ranges array


	gfx_root_parameter_t  rootParameters[1];															// only one parameter right now
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;						// this is a descriptor table
	rootParameters[0].DescriptorTable = descriptorTable;												// this is our descriptor table for this root parameter
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;								// our pixel shader will be the only shader accessing this parameter for now


	gfx_rootsignature_t* gfxRootSignature;
	
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init ( _countof ( rootParameters ),												// we have 1 root parameter
		rootParameters,																					// a pointer to the beginning of our root parameters array
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |									// we can deny shader stages here for better performance
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS );


	gfx_memblob_t* signature;
	gfx_memblob_t* error;
	DXERROR( D3D12SerializeRootSignature( &rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error ) );
	DXERROR( gfxDevice->CreateRootSignature( 0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS( &gfxRootSignature ) ) );

	return gfxRootSignature;
}


struct GfxGameRenderer
{
	ctx_handle_t			ctxHandle;
	gfx_adapter_t			*gfxAdapter;
	gfx_factory_t			*gfxFactory;
	gfx_device_t			*gfxDevice;
	gfx_cmdqueue_t			*gfxCmdQueue;
	gfx_swapchain_t			*gfxSwapChain;
	gfx_cmdlist_t			*gfxCmdList;
	gfx_descheap_t			*gfxRTDescHeap;
	gfx_descheap_t			*gfxDSVDescHeap;
	uint					gfxRTDescSize;
	uint					gfxDSVDescSize;
	gfx_resource_t			*gfxDepthStencilTarget;

	gfx_rootsignature_t		*gfxRootSignature;

	GfxPerFrameData			perFrameData[NUM_FRAMEBUFFERS];
	GfxViewInfo				viewInfo;

	uint					frameCount;
	uint					frameIndex;
	
	gfx_fence_event_t		gfxEndFrameFenceEvent;

	// Default stuff, need to create material system later
	gfx_memblob_t			*defaultVertexShader;
	gfx_memblob_t			*defaultPixelShader;
	gfx_pipeline_state_t	*defaultPSO;
	gfx_resource_t			*defaultVertexBuffer;
	gfx_resource_t			*defaultIndexBuffer;
	gfx_vert_buffer_view_t	defaultVertexBufferView;
	gfx_index_buffer_view_t	defaultIndexBufferView;

	gfx_viewport_t			gfxViewport;
	gfx_rect_t				gfxScissorRect;

	GfxResourceManager			*memManager;

	GfxGameRenderer()
	{
		
	}
	
	void InitViewPortAndScissor ()
	{
		gfxViewport.Width = static_cast< float >( WINDOWED_WIDTH );
		gfxViewport.Height = static_cast< float >( WINDOWED_HEIGHT );
		gfxViewport.MinDepth = 0.0f;
		gfxViewport.MaxDepth = 1.0f;

		gfxScissorRect.right = static_cast<LONG>( WINDOWED_WIDTH );
		gfxScissorRect.bottom = static_cast<LONG>( WINDOWED_HEIGHT );
	}


	void InitViewInfo ()
	{
		viewInfo.cameraPos = vec3 ( 0, 0, 50 );
		viewInfo.projectionMat = Gfx_CalculateProjectionMatrix ( 4.0f, static_cast< float >( WINDOWED_WIDTH ), static_cast< float >( WINDOWED_HEIGHT ), TO_RAD( 90.0f ) );
		viewInfo.viewMat = mat4x4 ();

		viewInfo.viewMat.row[0].w = -viewInfo.cameraPos.x;
		viewInfo.viewMat.row[1].w = -viewInfo.cameraPos.y;
		viewInfo.viewMat.row[2].w = -viewInfo.cameraPos.z;
	}


	void InitDebugLayer ()
	{
		gfx_info_queue_t *infoQueue;

		DXERROR( gfxDevice->QueryInterface ( __uuidof(ID3D12InfoQueue), (void**)&infoQueue ) );


		infoQueue->SetBreakOnSeverity ( D3D12_MESSAGE_SEVERITY_CORRUPTION, true );
		infoQueue->SetBreakOnSeverity ( D3D12_MESSAGE_SEVERITY_ERROR, true );
		infoQueue->SetBreakOnSeverity ( D3D12_MESSAGE_SEVERITY_WARNING, false );

		infoQueue->Release ();		
	}


	void InitHWInterfaces ()
	{
		gfx_debug_t* debugController;
		if ( SUCCEEDED ( D3D12GetDebugInterface ( IID_PPV_ARGS ( &debugController ) ) ) )
		{
			debugController->EnableDebugLayer ();
		}

		gfxFactory = Gfx_CreateFactory ();
		gfxAdapter = Gfx_SelectAdapter ( gfxFactory );
		gfxDevice = Gfx_CreateDevice ( gfxAdapter );

		InitDebugLayer ();

		DXERROR ( gfxFactory->MakeWindowAssociation ( ctxHandle, DXGI_MWA_NO_ALT_ENTER ) );

		memManager = GfxResourceManager::Instance ();
		memManager->Init ( gfxDevice );
	}
	

	void InitEndFrameFence ()
	{
		for ( uint n = 0; n < NUM_FRAMEBUFFERS; n++ )
		{
			GfxPerFrameData *curFrameData = perFrameData + n;
			curFrameData->gfxEndFrameFenceValue = 0;

			DXERROR ( gfxDevice->CreateFence ( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS ( &curFrameData->gfxEndFrameFence ) ) );
		}

		// Create an event handle to use for frame synchronization.
		gfxEndFrameFenceEvent = CreateEvent ( nullptr, FALSE, FALSE, nullptr );
		if ( gfxEndFrameFenceEvent == nullptr )
		{
			DXERROR ( HRESULT_FROM_WIN32 ( GetLastError () ) );
		}
	}


	void InitGfxPipeline () // call InitHWInterfaces before this
	{
		assert ( gfxDevice );
		gfxCmdQueue = Gfx_CreateCommandQueue ( gfxDevice );
		gfxSwapChain = Gfx_CreateSwapChain ( gfxFactory, gfxCmdQueue, ctxHandle );
		
		frameCount = 0;
		frameIndex = gfxSwapChain->GetCurrentBackBufferIndex ();
		
		gfxRTDescHeap = Gfx_CreateRTVDescriptorHeapForRenderTargets ( gfxDevice );
		gfxDSVDescHeap = Gfx_CreateDSVDescriptorHeapForRenderTargets ( gfxDevice );

		gfxRTDescSize = gfxDevice->GetDescriptorHandleIncrementSize ( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
		gfxDSVDescSize = gfxDevice->GetDescriptorHandleIncrementSize ( D3D12_DESCRIPTOR_HEAP_TYPE_DSV );

		Gfx_InitFrameBuffersAndCommandAllocators ( gfxDevice, gfxRTDescHeap, gfxDSVDescHeap, gfxSwapChain, perFrameData, &gfxDepthStencilTarget );
		Gfx_InitConstantBufferDescHeaps ( gfxDevice, perFrameData );
		Gfx_InitConstantBuffers ( gfxDevice, perFrameData );

		gfxRootSignature = Gfx_CreateRootSignature ( gfxDevice );

		InitEndFrameFence ();
	}


	// default stuff the renderer has. Please change as soon as we have new assets
	void InitDefaultPipelineState ()
	{
		UINT compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
		gfx_memblob_t *vsErrors;
		gfx_memblob_t *psErrors;

		DXERROR ( D3DCompileFromFile ( L"C:\\Users\\wert\\Desktop\\RyanAndFriends\\RyanEngine\\RyanEngine\\Shaders\\shaders.hlsl", nullptr, nullptr, "vsmain", "vs_5_0", compileFlags, 0, &defaultVertexShader, &vsErrors ) );
		
		if ( vsErrors )
		{
			const char *errorMsg = (const char*)vsErrors->GetBufferPointer ();
			printf ( "%s\n", errorMsg );
		}

		DXERROR ( D3DCompileFromFile ( L"C:\\Users\\wert\\Desktop\\RyanAndFriends\\RyanEngine\\RyanEngine\\Shaders\\shaders.hlsl", nullptr, nullptr, "psmain", "ps_5_0", compileFlags, 0, &defaultPixelShader, &psErrors ) );

		if ( psErrors )
		{
			const char *errorMsg = (const char*)psErrors->GetBufferPointer ();
			printf ( "%s\n", errorMsg );
		}

		gfx_input_desc_t inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		// Define the vertex input layout.
		gfx_pipeline_desc_t psoDesc = {};
		psoDesc.InputLayout = { inputElementDescs, _countof ( inputElementDescs ) };
		psoDesc.pRootSignature = gfxRootSignature;
		psoDesc.VS = CD3DX12_SHADER_BYTECODE ( defaultVertexShader );
		psoDesc.PS = CD3DX12_SHADER_BYTECODE ( defaultPixelShader );
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC ( D3D12_DEFAULT );
		psoDesc.BlendState = CD3DX12_BLEND_DESC ( D3D12_DEFAULT );
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC ( D3D12_DEFAULT );
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.DSVFormat = GFX_DEPTH_STENCIL_FORMAT;
		psoDesc.SampleDesc.Count = 1;
		DXERROR ( gfxDevice->CreateGraphicsPipelineState ( &psoDesc, IID_PPV_ARGS ( &defaultPSO ) ) );
	}


	void InitDefaultGeo ()
	{
		// make a temporary triangle with vert buffers and index buffers stored in upload memory ( write combine memory )
		// upload memory is cpu writable and gpu R/W. This may be slower than default memory. 
		// upload memory usually connects through the memory controller instead of the dram controller, which makes gpu access slower

		// Create the vertex buffer.
		{
			// Define the geometry for a triangle.
			VertexPosCol triangleVertices[] =
			{
				{ { 0.0f, 20.0f, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
				{ { 20.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
				{ { 0.0f, 0.0f,	 0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } },

				{ { 0.0f, 30.0f, -1.0f },{ 1.0f, 1.0f, 0.0f, 1.0f } },
				{ { 30.0f, 0.0f, -1.0f },{ 0.0f, 1.0f, 1.0f, 1.0f } },
				{ { -30.0f, 0.0f, -1.0f },{ 1.0f, 0.0f, 1.0f, 1.0f } }
			};

			const uint vertexBufferSize = sizeof ( triangleVertices );

			// shouldnt use upload heaps, but this is just some temporary junk.
			DXERROR ( gfxDevice->CreateCommittedResource (
				&CD3DX12_HEAP_PROPERTIES ( D3D12_HEAP_TYPE_UPLOAD ),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer ( vertexBufferSize ),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS ( &defaultVertexBuffer ) ) );

			// Copy the triangle data to the vertex buffer.
			byte* vertData;
			CD3DX12_RANGE readRange ( 0, 0 );		// We do not intend to read from this resource on the CPU.
			DXERROR ( defaultVertexBuffer->Map ( 0, &readRange, reinterpret_cast<void**>(&vertData) ) );
			memcpy ( vertData, triangleVertices, vertexBufferSize );
			defaultVertexBuffer->Unmap ( 0, nullptr );

			// Initialize the vertex buffer view.
			defaultVertexBufferView.BufferLocation = defaultVertexBuffer->GetGPUVirtualAddress ();
			defaultVertexBufferView.StrideInBytes = sizeof ( VertexPosCol );
			defaultVertexBufferView.SizeInBytes = vertexBufferSize;
		}


		//create index buffer
		{
			ushort indexes[] = { 0, 1, 2, 3, 4, 5 };

			const uint indexBufferSize = sizeof ( indexes );

			// shouldnt use upload heaps, but this is just some temporary junk.
			DXERROR ( gfxDevice->CreateCommittedResource (
				&CD3DX12_HEAP_PROPERTIES ( D3D12_HEAP_TYPE_UPLOAD ),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer ( indexBufferSize ),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS ( &defaultIndexBuffer ) ) );

			byte* indexData;
			CD3DX12_RANGE readRange ( 0, 0 );		// We do not intend to read from this resource on the CPU.
			DXERROR ( defaultIndexBuffer->Map ( 0, &readRange, reinterpret_cast<void**>( &indexData ) ) );
			memcpy ( indexData, indexes, indexBufferSize );
			defaultIndexBuffer->Unmap ( 0, nullptr );

			// Initialize the vertex buffer view.
			defaultIndexBufferView.BufferLocation = defaultIndexBuffer->GetGPUVirtualAddress ();
			defaultIndexBufferView.Format = DXGI_FORMAT_R16_UINT;
			defaultIndexBufferView.SizeInBytes = indexBufferSize;
		}
	}


	void InitCommandList ()
	{
		DXERROR ( gfxDevice->CreateCommandList ( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, perFrameData[frameIndex].gfxCmdAllocator, defaultPSO, IID_PPV_ARGS ( &gfxCmdList ) ) );

		// Command lists are created in the recording state, but there is nothing
		// to record yet. The main loop expects it to be closed, so close it now.
		DXERROR ( gfxCmdList->Close () );
	}


	void Init( ctx_handle_t windowHandle )
	{
		ctxHandle = windowHandle;

		InitHWInterfaces();
		InitViewPortAndScissor ();
		InitViewInfo ();
		InitGfxPipeline();

		InitDefaultPipelineState (); // temporary
		InitDefaultGeo (); // temporary

		InitCommandList ();
		WaitEndFrameFence ();
	}

	

	void WaitEndFrameFence ()
	{
		GfxPerFrameData *curFrameData = perFrameData + frameIndex;
		gfx_fence_t *curFrameEndFence = curFrameData->gfxEndFrameFence;

		uint64_t endFrameFenceValue = curFrameData->gfxEndFrameFenceValue;

		// Schedule a Signal command in the queue.
		DXERROR ( gfxCmdQueue->Signal ( curFrameEndFence, endFrameFenceValue ) );

		// Wait until the fence has been processed.
		DXERROR ( curFrameEndFence->SetEventOnCompletion ( endFrameFenceValue, gfxEndFrameFenceEvent ) );
		WaitForSingleObjectEx ( gfxEndFrameFenceEvent, INFINITE, FALSE );

		// Increment the fence value for the current frame.
		curFrameData->gfxEndFrameFenceValue++;
	}


	void WaitPreviousEndFrameFence ()
	{	// this is called after the current frame to ensure that the previous frame work is done before we start modifying the next(prev) frame resources
		// the fence value for the next frame is updated here

		frameIndex = gfxSwapChain->GetCurrentBackBufferIndex ();

		GfxPerFrameData *prevFrameData = perFrameData + frameIndex;
		gfx_fence_t *prevFrameEndFence = prevFrameData->gfxEndFrameFence;

		uint64_t endFrameFenceValue = prevFrameData->gfxEndFrameFenceValue;

		if ( prevFrameEndFence->GetCompletedValue () < endFrameFenceValue )
		{
			// Wait until the fence has been processed.
			DXERROR ( prevFrameEndFence->SetEventOnCompletion ( endFrameFenceValue, gfxEndFrameFenceEvent ) );
			WaitForSingleObjectEx ( gfxEndFrameFenceEvent, INFINITE, FALSE );
		}

		// Increment the fence value for the current frame.
		prevFrameData->gfxEndFrameFenceValue++;
	}


	void UpdateTransformConstantBuffer ( GfxPerFrameData *curFrameData  )
	{
		gfx_transform_buffer_t transformData;
		quatn worldRot ( vec3 ( 0, 0, -1 ), TO_RAD ( frameCount / 60.f ) );

		transformData.worldMatrix = worldRot.toRotMatrix();
		transformData.viewMatrix = viewInfo.viewMat;
		transformData.projectionMatrix = viewInfo.projectionMat;
		
		memcpy ( curFrameData->gfxConstantBufferData[GFX_TRANSFORM_CONSTANT_BUFFER], &transformData, GFX_TRANSFORM_CONSTANT_BUFFER_SIZE );
	}


	void Render ()
	{
		GfxPerFrameData	*curFrameData = perFrameData + frameIndex;

		gfx_cmdalloc_t *curCmdAllocator = curFrameData->gfxCmdAllocator;

		// Command list allocators can only be reset when the associated 
		// command lists have finished execution on the GPU; apps should use 
		// fences to determine GPU execution progress.
		DXERROR ( curCmdAllocator->Reset () );

		// However, when ExecuteCommandList() is called on a particular command 
		// list, that command list can then be reset at any time and must be before 
		// re-recording.
		DXERROR ( gfxCmdList->Reset ( curCmdAllocator, defaultPSO ) );

		UpdateTransformConstantBuffer ( curFrameData );

		// Set necessary state.
		gfxCmdList->SetGraphicsRootSignature ( gfxRootSignature );
		gfxCmdList->RSSetViewports ( 1, &gfxViewport );
		gfxCmdList->RSSetScissorRects ( 1, &gfxScissorRect );

		//// Indicate that the back buffer will be used as a render target.
		gfxCmdList->ResourceBarrier ( 1, &CD3DX12_RESOURCE_BARRIER::Transition ( curFrameData->gfxRenderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET ) );
		
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle ( gfxRTDescHeap->GetCPUDescriptorHandleForHeapStart (), frameIndex, gfxRTDescSize );
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle ( gfxDSVDescHeap->GetCPUDescriptorHandleForHeapStart () );

		gfxCmdList->OMSetRenderTargets ( 1, &rtvHandle, FALSE, &dsvHandle );
		
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		gfxCmdList->ClearRenderTargetView ( rtvHandle, clearColor, 0, nullptr );
		gfxCmdList->ClearDepthStencilView ( dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0, 0, nullptr );
		
		
		gfx_descheap_t* descriptorHeaps[] = { curFrameData->gfxCBVDescHeap };
		gfxCmdList->SetDescriptorHeaps ( _countof ( descriptorHeaps ), descriptorHeaps );
		
		// set the root descriptor table 0 to the constant buffer descriptor heap
		gfxCmdList->SetGraphicsRootDescriptorTable ( 0, curFrameData->gfxCBVDescHeap->GetGPUDescriptorHandleForHeapStart () );
		
		gfxCmdList->IASetPrimitiveTopology ( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		gfxCmdList->IASetIndexBuffer ( &defaultIndexBufferView );
		gfxCmdList->IASetVertexBuffers ( 0, 1, &defaultVertexBufferView );
		gfxCmdList->DrawIndexedInstanced ( 6, 1, 0, 0, 0 );
		
		// Indicate that the back buffer will now be used to present.
		gfxCmdList->ResourceBarrier ( 1, &CD3DX12_RESOURCE_BARRIER::Transition ( curFrameData->gfxRenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT ) );
		
		DXERROR ( gfxCmdList->Close () );
		
		// Execute the command list.
		cmdlist_t* cmdLists[] = { gfxCmdList }; // implicit cast from gfx_cmdlist_t to cmdlist_t interface
		gfxCmdQueue->ExecuteCommandLists ( _countof ( cmdLists ), cmdLists );
		
		// Present the frame.
		DXERROR ( gfxSwapChain->Present ( 0, 0 ) );

		const UINT64 currentFenceValue = curFrameData->gfxEndFrameFenceValue;
		DXERROR ( gfxCmdQueue->Signal ( curFrameData->gfxEndFrameFence, currentFenceValue ) );

		frameCount++;

		WaitPreviousEndFrameFence ();
	}
};
