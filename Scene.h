#pragma once
#include"stdafx.h"
#include"Timer.h"
#include"Shader.h"



class CScene
{
public:
	CScene();
	~CScene();
	//������ Ű����� ���콺 �޽����� ó���Ѵ�
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
	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

	protected:
	//���� ���̴����� �����̴�.���̴����� ���� ��ü���� �����̴�.
	CShader** m_ppShaders = NULL;
	int m_nShaders = 0;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;


	//��Ʈ �ñ׳��ĸ� ��Ÿ���� �������̽� �������̴�. 
	 ID3D12PipelineState *m_pd3dPipelineState = NULL;
	//���������� ���¸� ��Ÿ���� �������̽� �������̴�.
	

	
	
};


