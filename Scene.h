#pragma once
#include"stdafx.h"
#include"Timer.h"
#include"Shader.h"



class CScene
{
public:
	CScene();
	~CScene();
	//씬에서 키보드와 마우스 메시지를 처리한다
	bool onProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
	bool onProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, 
		LPARAM lParam);

	void BuildObjects(ID3D12Device* pd3dDevice,ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);

	void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	void ReleaseUploadBuffers();
	//그래픽 루트 시그너쳐를 생성한다.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

	protected:
	//씬은 셰이더들의 집합이다.셰이더들은 게임 객체들의 집합이다.
	CShader** m_ppShaders = NULL;
	int m_nShaders = 0;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;


	//루트 시그너쳐를 나타내는 인터페이스 포인터이다. 
	 ID3D12PipelineState *m_pd3dPipelineState = NULL;
	//파이프라인 상태를 나타내는 인터페이스 포인터이다.
	

	
	
};


