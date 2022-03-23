#include "CGameFramework.h"
#include"stdafx.h"



CGameFramework::CGameFramework()
{
	m_pdxgiFactory = NULL;//DXGI ���丮 �������̽��� ���� �������̴�.//IDXGIFactory�� ���(�׷���ī��)(adaptor),�����ġ(�����),(output),����̽��� �����ϰ� �ִ� �İ�ü�̴�.
	m_pdxgiSwapChain=NULL;///���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	m_pd3dDevice=NULL;//Direct3D ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	
	m_pd3dCommandAllocator = NULL; //Command List���� ID3D12CommandAllocator�� �ϳ� �����ȴ�.Command List�� �߰��� Command���� �� Allocator�� �޸𸮿� ����ȴ�.
	m_pd3dCommandQueue = NULL; //���� ť, CPU�� Command List�� Direct3D API�� ���� GPU�� Command Queue�� �����Ѵ�.
	m_pd3dPipelineState = NULL;//�׷��Ƚ� ���������� ���� ��ü�� ���� �������̽� �������̴�.
	m_pd3dCommandList = NULL;//���� ����Ʈ �������̽� ������->CPU���� �����Ѵ�.->Commanqueue�� ����

	for (int i = 0; i < m_nSwapChainBuffers; i++)m_ppd3dRenderTargetBuffers[i] = NULL;//�ĸ� ���۸� m_nSwapChainBuffers���� ��ŭ �����.
	m_pd3dDsvDescriptorHeap = NULL;//���� ���α׷��� �ʿ��� ������/ ����� ���� ������ ���� ������ �Ѵ�.
	m_nRtvDescriptorIncrementSize = 0;//����-���ٽ� ������ ������ ũ���̴�

	m_pd3dDepthStencilBuffer = NULL;//���� ���ٽ� ����->Z BUFFer ���ٽ�
	m_pd3dDsvDescriptorHeap = NULL;//���� ���α׷��� �ʿ��� ������/ ����� ���� ������ ���� ������ �Ѵ�
	m_nDsvDescriptorIncrementSize = 0;//����-���ٽ� ������ ������ ũ���̴�.

	m_nSwapChainBufferIndex = 0;//���� ���� ü���� �ĸ� ���� �ε����̴�.

	m_hFenceEvent = NULL;
	m_pd3dFence = NULL;
	m_nFenceValue = 0;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

}

CGameFramework::~CGameFramework()
{
}

//���� �Լ��� �������α׷��� ���� �Ǿ� �� �����찡 �����Ǹ� ȣ��ȴٴ� �Ϳ� �����϶�.
bool CGameFramework::onCreate(HINSTANCE hinstance, HWND hmainwnd)
{
	m_hinstance = hinstance;
	m_hwnd = hmainwnd;
	
	//Direct3D ����̽�,���� ť�� ���� ����Ʈ ,���� ü�� ���� �����ϴ� �Լ��� ȣ���Ѵ�.
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateRenderTargetViews();
	CreateDepthStencilView();

	BuildObjects();
	//�������� ���� ��ü�� �����Ѵ�.
	return(true);
}

void CGameFramework::OnDestroy()
{
	WaitForGpuComplete();
	//GPU�� ��� ���� ����Ʈ�� ������ �� ���� ��ٸ���.->commandqueue�� �� ���� ���� �� ���� ��ٸ���.

	ReleaseObjects();
	//���� ��ü(���� ���� ��ü)�� �Ҹ��Ѵ�.

	::CloseHandle(m_hFenceEvent);//->��鰪�� ��ȯ
	for (int i = 0; i < m_nSwapChainBuffers; i++)if (m_ppd3dRenderTargetBuffers[i])
		m_ppd3dRenderTargetBuffers[i]->Release();//->�ĸ� ���۸� ���� ����
	if (m_pd3dDepthStencilBuffer)m_pd3dDepthStencilBuffer->Release(); //rtzheap ����

	if (m_pd3dDepthStencilBuffer)m_pd3dDepthStencilBuffer->Release();//����-���ٽ� ���� ����
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();//DSVheap����

	if (m_pd3dCommandAllocator)m_pd3dCommandAllocator->Release();//->Commandlist allocator ->����
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();//�׷��� ī���� Ŀ�ǵ� ť ����
	if (m_pd3dPipelineState) m_pd3dPipelineState->Release();//cpuĿ�ǵ� ����Ʈ�� GPU Ŀ���ť�� �����ϴ� ���������� ����
	if (m_pd3dCommandList) m_pd3dCommandList->Release(); // cpu commandList����

	if (m_pd3dFence) m_pd3dFence->Release();//�׷��� ī���� �ҽ� ���� 

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);//��üȭ�� ����
	if (m_pdxgiSwapChain)m_pdxgiSwapChain->Release();//����ü��(�ĸ� ���ۿ� ���� ���۸� �մ� ü��)�� Release();
	if (m_pd3dDevice)m_pd3dDevice->Release();//���� ����̽�(�׷���ī��) ����
	if (m_pdxgiFactory)m_pdxgiFactory->Release();//���丮( ���(�׷���ī��)(adaptor),�����ġ(�����),(output),����̽��� )���� 

#if defined(_DEBUG)
	IDXGIDebug1 *pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
		DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hwnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;//���� ����
	m_nWndClientHeight = rcClient.bottom - rcClient.top;//���� ����

	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	dxgiSwapChainDesc.Width = m_nWndClientWidth; //�ػ� �ʺ� �����ϴ� ���Դϴ�
	dxgiSwapChainDesc.Height = m_nWndClientHeight;//�ػ� ���̸� �����ϴ� ��
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//���÷��� ������ �����ϴ� DXGI_FORMAT �����Դϴ�.
	dxgiSwapChainDesc.SampleDesc.Count=(m_bMsaa4xEnable)?4:1;//�ȼ��� ��Ƽ ���� �����Դϴ�.
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//�ĸ� ���ۿ� ���� ǥ�� ��뷮 �� CPU������ �ɼ��� �����ϴ� DXGI_USAGE���İ�
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;//���� ü���� ���ۼ��� �����ϴ� ��,��ü ȭ�� ���� ü���� ���� �� �Ϲ������� �̰��� ���� ���۸� ����
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;//�ĸ� ������ ũ�Ⱑ ��� ����� ���� ���� ��� ũ�� ���� DXGI_SCALING ������ ��.
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//���� ü�ο��� ���Ǵ� ���������̼� �𵨰� ǥ���� ������ �� ���������̼� ������ ������ ó���ϴ� �ɼ��� �����ϴ� DXGI_SWAP_EFFECT ������ ���Դϴ�.
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; //���� ü�� �ĸ� ������ ������ ������ �ĺ��ϴ� DXGI_ALPHA_MODE ���İ��Դϴ�.
	dxgiSwapChainDesc.Flags = 0;//��Ʈ ���� �Ǵ� �۾��� ����Ͽ� ���յ� DXGI_SWAP_CHAIN_FLAG ���İ��� �����Դϴ�. ��� ���� ���� ü�� ���ۿ� ���� �ɼ��� �����մϴ�.

	//����ü���� ��ü ȭ�� ��带 �����Ѵ�.

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;//���ΰ�ħ �� 60����
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;//���ΰ�ħ �� 1����
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//��ĵ �� �׸��� ��带 �����ϴ� DXGI_MODE_SCANLINE_ORDER  STRUCT ������ ����Դϴ�.
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;//���� ���� ��带 �����ϴ�  DXGI_MODE_SCALING Ȯ��� ������ ����Դϴ�
	dxgiSwapChainFullScreenDesc.Windowed = TRUE; //���� ü���� â ��忡 �ִ��� ���θ� �����ϴ� Boole ���Դϴ�.���� ü���� â ��忡 �ִ� ��� TRUE; �׷��� ������ FALSE.

	m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hwnd,
		&dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1**)&m_pdxgiSwapChain);//HWND �ڵ�� ����� ���� ü���� ���� ü���� ��� â�� ����ϴ�.
	//����ü���� �����Ѵ�.(in,in,in,in,in,out)->t������� &m_pdxgiSwapChain�� ����Ѵ�.,

	m_pdxgiFactory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER);

	//ALT+ENTER Ű�� ������ ��Ȱ��ȭ �Ѵ�.

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();//->���� �߿��ҵ�??
	//����ü���� ���� �ĸ���� �ε����� �����Ѵ�.

}



void CGameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;

	UINT nDXGIFactoryFlags = 0;

#if defined(_DEBUG)
	ID3D12Debug* pd3dDebugController = NULL;
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&pd3dDebugController);
	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}
	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void**)&m_pdxgiFactory);//�ٸ� DXGI ��ü�� �����ϴ� �� ����� �� �ִ� DXGI 1.3 ���͸��� ����ϴ�.
	IDXGIAdapter1* pd3dAdapter = NULL;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc; //->��� �����
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter,
			D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_pd3dDevice)))break;
	}
	//��� �ϵ���� ����Ϳ� ���Ͽ� Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� �����Ѵ�.

	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(__uuidof(IDXGIAdapter1), (void**)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}
	//Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� ������ �� ������ WARP ����̽��� �����Ѵ�.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;//msaa ����Ƽ ���� ����̽�
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//�ػ� �ʱ�ȭ
	d3dMsaaQualityLevels.SampleCount = 4;//Msaa4x ���� ���ø�
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;//���� (??) ���� ã�� ����
	d3dMsaaQualityLevels.NumQualityLevels = 0;

	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;//->���� ���ø��� ǰ�� ���� Ȯ��
	//����̽��� �����ϴ� ���� ������ ǰ�� ������ Ȯ�δ�.

	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;
	//���� ������ ǰ�� ������ 1���� ũ�� ���� ���ø��� Ȱ��ȭ�Ѵ�. 
		
	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence);
	m_nFenceValue = 0;
	//�潺�� �����ϰ� �潺 ���� 0���� �����Ѵ�.

	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	/*�潺�� ����ȭ�� ���� �̺�Ʈ ��ü�� �����ϴ�(�̺�Ʈ ��ü�� �ʱⰪ�� FALSE�̴�).
	�̺�Ʈ�� ����Ǹ�(Signal)�̺�Ʈ�� ���� �ڵ������� FALSE�� �ǵ��� �����Ѵ�.*/

	m_d3dviewport.TopLeftX = 0;
	m_d3dviewport.TopLeftY = 0;
	m_d3dviewport.Width = static_cast<float>(m_nWndClientWidth);
	m_d3dviewport.Height = static_cast<float>(m_nWndClientHeight);
	m_d3dviewport.MinDepth = 0.0f;
	m_d3dviewport.MaxDepth = 1.0f;
	//��ǥƮ�� �� �������� Ŭ���̾�Ʈ ���� ��ü�� �����Ѵ�.

	m_d3dScissorRect = { 0,0,m_nWndClientWidth,m_nWndClientHeight };
	//���� �簢���� �� �������� Ŭ���̾�Ʈ ���� ��ü�� �����Ѵ�.

	if (pd3dAdapter)pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;//ť ����
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, 
		__uuidof(ID3D12CommandQueue), (void**)&m_pd3dCommandQueue);
	//����(Direct) ���� ť�� �����Ѵ�.

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		__uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);
	//����(Direct)���� �Ҵ��ڸ� �����Ѵ�.

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);
	//����(Direct)���� ����Ʈ�� �����Ѵ�.

	hResult = m_pd3dCommandList->Close();
	//���� ����Ʈ�� �����Ǹ� ����(Open)�����̹Ƿ� ����(Closed)���·� �����.

}




void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, 
		__uuidof(ID3D12DescriptorHeap),(void**)&m_pd3dRtvDescriptorHeap);
	//���� Ÿ�� ������ ��(�������� ������ ����ü�� ������ ����)�� �����Ѵ�.
	m_nRtvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//���� Ÿ�� ������ ���� ������ ũ�⸦ �����Ѵ�.

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);
	//���� ���ٽ� ������ ��(�������� ������ 1)�� �����Ѵ�.
	m_nDsvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//����-���ٽ� ������ ���� ������ ũ�⸦ �����Ѵ�.
}

void CGameFramework::CreateRenderTargetViews()
{
	//����ü���� �� �ĸ� ���ۿ� ���� ���� Ÿ�ٺ並 �����Ѵ�.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void
			**)&m_ppd3dRenderTargetBuffers[i]);//->����ü���� �ĸ���� �� �ϳ��� ������ �մϴ�.
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], NULL,
			d3dRtvCPUDescriptorHandle);//->���� Ÿ�� ����
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;//->�̰� �迭 �������� �̸�ŭ�÷���� ���´�  ����.. �ٽ� ��ﳪ�� �����غ�
	}
}

void CGameFramework::CreateDepthStencilView()//���� ���ٽ� �並 �����.
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//���ҽ��� ����(��: D3D12_RESOURCE_DIMENSION_TEXTURE1D) �Ǵ� �������� ����((D3D12_RESOURCE_DIMENSION_BUFFER)�� �����ϴ� D3D12_RESOURCE_DIMENSION �� �� ��� .
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;//3D�� ��� ���ҽ��� ���̸� �����ϰ� 1D �Ǵ� 2D ���ҽ��� �迭�� ��� �迭 ũ�⸦ �����մϴ�.
	d3dResourceDesc.MipLevels = 1;//miplevel�� ��
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//DXGI_FORMAT �� �� ����� ���� �մϴ�.
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;//�� �Ӽ��� �����մϴ�.
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//�� ������ �����ϴ� D3D12_HEAP_TYPE ���� �� �Դϴ� .
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;//���� ���� cpu������ �Ӽ��� ����(cpu�� ������ �Ӽ��� �� �� �����ϴ�.)
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;//���� ���� �޸� Ǯ�� ����(unknown�޸� Ǯ�� �˼������ϴ�.)
	d3dHeapProperties.CreationNodeMask = 1;//���� ������� �۾��� ��� ���ҽ��� �����ؾ� �ϴ� ��带 ��Ÿ����.
	d3dHeapProperties.VisibleNodeMask = 1;//���� ������� �۾��� ��� ���ҽ��� ǥ�õǴ� ��� ����

	D3D12_CLEAR_VALUE d3dClearValue;//Ư�� ���ҽ��� ���� ����� �۾��� ����ȭ�ϴ� �� ���Ǵ� ��
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);
	//����-���ٽ� ���۸� �����Ѵ�.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();//���� ������ ��Ÿ���� CPU ������ �ڵ��� �����ɴϴ�.
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL,
		d3dDsvCPUDescriptorHandle);
	//����-���ٽ� ���� �並 �����Ѵ�.

}

void CGameFramework::BuildObjects()
{
}

void CGameFramework::ReleaseObjects()
{
}



void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID) 
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case VK_F8:
			break;
		case VK_F9:
			break;
		default:
			break;
		
		}
		break;
	default:
		break;
	}
}

LRESULT CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID)
	{
		case WM_SIZE:
		{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);
		break;
		}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
			OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
			break;
		case WM_KEYDOWN:
		case WM_KEYUP:
			OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
			break;
	}	
	return(0);
}
void CGameFramework::ProcessInput()
{
}

void CGameFramework::AnimateObjects()
{
}

void CGameFramework::WaitForGpuComplete()
{
	m_nFenceValue++;
	//CPU�潺�� ���� �����Ѵ�.
	const UINT64 nFence = m_nFenceValue;
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFence);
	//GPU�� �潺�� ���� �����ϴ� ������ ���� ť�� �߰��Ѵ�.
	if (m_pd3dFence->GetCompletedValue() < nFence)
	{
	//�潺�� ���� ���� ������ ������ ������ �潺�� ���� ���� ������ ���� �� ������ ��ٸ���.
		hResult = m_pd3dFence->SetEventOnCompletion(nFence, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::FrameAdvance()
{
	ProcessInput();

	AnimateObjects();

	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	//���� �Ҵ��ڿ� ���� ����Ʈ�� �����Ѵ�.

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;//�ڿ� �庮(�ڿ� ����� ��ȯ)�� �����մϴ�.
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;//�ٸ� �뵵 ���� ���� ���ҽ� ������ ��ȯ�� ��Ÿ���� ��ȯ �庮�Դϴ�. ȣ���ڴ� ���� ���ҽ��� ��� ���ĸ� �����ؾ� �մϴ�.
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;//"���� ����" �Ǵ� "���� ����"�� ���� D3D12_RESOURCE_BARRIER_FLAGS ���� ����� �����մϴ� .
	d3dResourceBarrier.Transition.pResource = 
		m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];//��ȯ�� ���� ���ҽ��� ��Ÿ���� ID3D12Resource ��ü�� ���� ������ �Դϴ�.
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;//��ȯ�� ���� ���� ���ҽ��� �ε����Դϴ�.,���ҽ��� ��� ���� ���ҽ��� ���ÿ� ��ȯ�մϴ�.
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);// ���ҽ��� ���� ���� �׼����� ����ȭ�ؾ� ���� ����̹��� �˸��ϴ�.

	m_pd3dCommandList->RSSetViewports(1, &m_d3dviewport);
	m_pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
	//����Ʈ�� ���� �簢���� �����Ѵ�.

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();// ���� ������ ��Ÿ���� CPU ������ �ڵ��� �����ɴϴ�.
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);//ü�� ���� �ε���* ���� Ÿ�� ���� ������

	//������ ���� Ÿ�ٿ� �ش��ϴ� �������� CPU�ּ�(�ڵ�)�� ����Ѵ�.
	
	float pfClearColor[4] = { 0.0f,0.125f,0.3f,1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, 
		pfClearColor, 0, NULL);//->�簢��??
	//���ϴ� �������� ���� Ÿ���� �����.

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//����-���ٽ� �������� CPU ���� �ּҸ� �޾��ش�.
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	//���ϴ� ������ ����-���ٽ�(��)�� �ʱ�ȭ ���ش�.

	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE,
		&d3dDsvCPUDescriptorHandle);
	//���� Ÿ�� ��(������)�� ����-���ٽ� ��(������)�� ��� -���� �ܰ�(OM)�� �����Ѵ�.

	//������ �ڵ�� ���⿡ �߰��ɰ��̴�.

	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	/*���� ���� Ÿ�ٿ� ���� �������� �����⸦ ��ٸ���. GPU�� ���� Ÿ��(����)�� �� �̻� ������� ������ ���� Ÿ��
�� ���´� ������Ʈ ����(D3D12_RESOURCE_STATE_PRESENT)�� �ٲ� ���̴�.*/

	hResult = m_pd3dCommandList->Close();
	//���� ����Ʈ�� ���� ���·� �����.

	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	//���� ����Ʈ�� ���� ť�� �߰��Ͽ� �����Ѵ�.

	WaitForGpuComplete();
	///GPU�� ��� ���� ����Ʈ�� ������ �� ���� ��ٸ���.
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;//� ü���� ���������̼��� ����ȭ�ϴ� �� ������ �Ǵ� ���翡 ���� ������ �����մϴ�.
	dxgiPresentParameters.DirtyRectsCount = 0;//������ �������� �� ���ۿ��� ������Ʈ�ϴ� ������Ʈ�� �簢���� ���Դϴ�. � ü���� �� ������ ����Ͽ� ���������̼��� ����ȭ�մϴ�
	dxgiPresentParameters.pDirtyRects = NULL;//������ �������� �� ���ۿ��� ������Ʈ�ϴ� ������Ʈ�� �簢�� ����Դϴ�.
	dxgiPresentParameters.pScrollRect = NULL;// ��ũ�ѵ� �簢���� ���� �������Դϴ�
	dxgiPresentParameters.pScrollOffset = NULL;//���� �簢��(���� ������)���� ��� �簢��(���� ������)���� �̵��ϴ� ��ũ�ѵ� ������ �����¿� ���� �������Դϴ�. 
	m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);//���÷��� ȭ�鿡 �������� ǥ���մϴ�.
	/*����ü���� ������Ʈ�Ѵ�. ������Ʈ�� �ϸ� ���� ���� Ÿ��(�ĸ����)�� ������ ������۷� �Ű����� ���� Ÿ�� ��
������ �ٲ� ���̴�.*/
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();//->���� ü���� ���� �� ������ �ε����� �����ͼ� �ε����� ����
}