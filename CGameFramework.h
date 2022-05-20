#pragma once
#include"stdafx.h"
#include"Timer.h"
#include"Scene.h"
#include"Camera.h"
class CGameFramework
{

private:
	HINSTANCE m_hinstance;//�츮�� ���� exe�� ��Ÿ���ִ� �����̴�.
	HWND m_hwnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

	IDXGIFactory4* m_pdxgiFactory;//DXGI ���丮 �������̽��� ���� �������̴�.//
	//IDXGIFactory�� ���(�׷���ī��)(adaptor),�����ġ(�����),(output),����̽��� �����ϰ� �ִ� �İ�ü�̴�.
	IDXGISwapChain3* m_pdxgiSwapChain;
	//���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D12Device* m_pd3dDevice;
	//Direct3D ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.

	bool m_bMsaa4xEnable = false;

	UINT m_nMsaa4xQualityLevels = 0;
	//MSAA(Multy Sampleing AA) ���� ���ø��� Ȱ��ȭ�ϰ� ���� ���ø� ������ �����Ѵ�. SSAA(SUPER SAMPLING AA)�� �ִ�.

	static const UINT m_nSwapChainBuffers = 2;
	//���� ü���� �ĸ� ������ �����̴�.

	UINT m_nSwapChainBufferIndex;
	//���� ���� ü���� �ĸ� ���� �ε����̴�.

	ID3D12Resource *m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];//���� Ÿ�ٹ��۴� ���� m_nRtvDescriptorIncrementSize���۸� ��Ÿ���� ���� �ƴ� 
	//�ĸ� ���۸� ��Ÿ���� ���̴�.�迭�� ���� ������ �ĸ� ���۰� ������ �غ� �� �� �־ �̴�.
	ID3D12DescriptorHeap *m_pd3dRtvDescriptorHeap;
	//��������, ���� ���α׷��� �ʿ��� ������ / ����� ���� ������ ���� ������ �Ѵ�.
	UINT m_nRtvDescriptorIncrementSize;
	//���� Ÿ�� ������ ������ ũ���̴�.

	ID3D12Resource *m_pd3dDepthStencilBuffer;//���� ���ٽ� ����->Z BUFFER ���ٽ� ->�� ���̴� ���� �����ϴ� ��
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;//���� ���α׷��� �ʿ��� ������/ ����� ���� ������ ���� ������ �Ѵ�.��
	UINT m_nDsvDescriptorIncrementSize;//����-���ٽ� ������ ������ ũ���̴�.

	ID3D12CommandQueue* m_pd3dCommandQueue;//��� ť,CPU�� Command List�� Direct3D API�� ���� GPU�� Command Queue�� �����Ѵ�.
	ID3D12CommandAllocator* m_pd3dCommandAllocator;//Command List���� ID3D12CommandAllocator�� �ϳ� �����ȴ�.Command List�� �߰��� Command���� �� Allocator�� �޸𸮿� ����ȴ�.
	ID3D12GraphicsCommandList* m_pd3dCommandList;//��� ����Ʈ �������̽� ������

	ID3D12PipelineState* m_pd3dPipelineState;
	//�׷��Ƚ� ���������� ���� ��ü�� ���� �������̽� �������̴�.

	ID3D12Fence* m_pd3dFence; //�̶� �ʿ��� ���� �ٷ� ��Ÿ��(Fence)�̴�.��Ÿ��(�潺)�� ID3D12Fence �������̽��� ��ǥ�Ǹ�, GPU�� CPU�� ����ȭ�� ���� �������� ���δ�.������ �潺 ��ü�� �����ϴ� �޼����̴�
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent;
	//�潺 �������̽� ������,�潺�� ��,�̺�Ʈ �ڵ��̴�.

	CScene *m_pScene;
	
	CCamera* m_pCamera = NULL;

	//������ ���� ������ ��ũ���� ����� Ÿ�̸��̴�.
	CGameTimer m_GameTimer;

	//������ ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ��̴�.
	_TCHAR m_pszFrameRate[50];
public:
	CGameFramework();
	~CGameFramework();

	bool onCreate(HINSTANCE hinstance, HWND hmainwnd);
	//�����ӿ�ũ��  �ʱ�ȭ�ϴ� �Լ��̴�(�� �����찡 �����Ǹ� ȣ��ȴ�.)
	void OnDestroy();
	//�����ӿ�ũ �ı�

	void ChangeSwapChainState();

	void CreateSwapChain();//����ü��
	void CreateRtvAndDsvDescriptorHeaps();//������ ��
	void CreateDirect3DDevice();//����̽� ����
	void CreateCommandQueueAndList();//��� ť,�Ҵ���,����Ʈ�� �����ϴ� �Լ�
	//���� ü��,����̽� ,������ ��,��� ť/�Ҵ���/����Ʈ�� �����ϴ� �Լ��̴�.

	void CreateRenderTargetViews();//���� Ÿ�� �並 �����ϴ� �Լ�
	void CreateDepthStencilView();// ���� - ���ٽ� �並 �����ϴ� �Լ��̴�.
	//���� Ÿ�� ��� ����-���ٽ� �並 �����ϴ� �Լ��̴�.

	void BuildObjects();
	void ReleaseObjects();
	//�������� �޽��� ���� ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�.

	//�����ӿ�ũ �ٽ�(����� �Է�,�ִϸ��̼�,������)�� �����ϴ� �Լ��̴�.
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void WaitForGpuComplete();//CPU�� GPu�� ����ȭ�ϴ� �Լ��̴�.
	//https://vitacpp.tistory.com/m/50

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	LRESULT OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);//LRESULT CALLBACK OnProcessingWindowMessage->CALLBACK�� �ȵǴ� ������ �𸣰���


	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�.

	void MoveToNextFrame();
};


