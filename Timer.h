#pragma once
#include"stdafx.h"
const ULONG MAX_SAMPLE_COUNT = 50;//50ȸ�� ������ ó���ð��� �����Ͽ� ����Ѵ�.
class CGameTimer
{
public:
	CGameTimer();
	virtual ~CGameTimer();
	void Start() {};
	void Reset();
	void Tick(float fLockFPS = 0.0f);
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
	float GetTImeElapsed();

private:
	bool m_bHardWareHAsPerformanceCounter; //��ǻ�Ͱ� Performance Counter�� ������ �ִ� ��
	float m_fTimeScale; //Scale Counter�� ��
	float m_fTimeElapsed; //������ ������ ���� ������ �ð�
	__int64 m_nCurrentTime; //������ �ð�
	__int64 m_nLastTime;  //������ �������� �ð�
	__int64 m_nPerformaceFrequency; //��ǻ���� Performance Frequency ����ī���� �� 

	float m_fFrameTime[MAX_SAMPLE_COUNT];//������ �ð��� �����ϱ� ���� �迭
	ULONG m_nSampleCount;//������ ������ Ƚ��
	unsigned long m_nCurrentFrameRate;//������ ������ ����Ʈ
	unsigned long m_nFramesPerSecond;//�ʴ� ������ ��
	float m_fFPSTimeElapsed;//������ ����Ʈ ��� �ҿ� �ð�

	bool m_bStopped;
};



