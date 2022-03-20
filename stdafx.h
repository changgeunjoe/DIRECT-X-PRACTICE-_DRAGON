// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once


#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
#include<wrl.h>//com언어 
#include<shellapi.h>
// C 런타임 헤더 파일입니다.

#include<d3d12.h>//다이렉트 12 API//https://docs.microsoft.com/ko-kr/windows/win32/direct3d12/directx-12-programming-environment-set-up //다양한 라이브러리 설치가능 
#include<dxgi1_4.h>///DXGI->다렉 텍스쳐 형식
#include<d3dcompiler.h>//HLSL 코드 또는 효과 파일을 주어진 대상에 대한 바이트 코드로 컴파일합니다.
#include<DirectXMath.h>//DirectXMath API는 일반적인 선형 대 수 및 DirectX 응용 프로그램에 공통적인 그래픽 수학 작업에 대해 SIMD 친화적인 c + + 형식 및 함수를 제공 합니다. 라이브러리는 Visual C++ 컴파일러의 SSE, AVX 및 arm-NEON 내장 기능을 통해 arm/ARM64에 대 한 Windows 32 비트 (x86), Windows 64 비트 (x64) 및 Windows에 대해 최적화 된 버전을 제공 합니다.
#include<DirectXPackedVector.h>//DirectXMath라이브러리를 사용하기 위해 추가한다 //https://lesslate.github.io/directx/DirectXMath-%EB%B2%A1%ED%84%B0-%EB%9D%BC%EC%9D%B4%EB%B8%8C%EB%9F%AC%EB%A6%AC/
#include<DirectXColors.h>//DirectXMath라이브러리를 사용하기 위해 추가한다 https://docs.microsoft.com/ko-kr/windows/win32/dxmath/pg-xnamath-getting-started
#include<DirectXCollision.h>//DirectXMath라이브러리를 사용하기 위해 추가한다 https://docs.microsoft.com/ko-kr/windows/win32/dxmath/pg-xnamath-getting-started
#include <DXGIDebug.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include<math.h>
#include<time.h>

#include <string>
#include <wrl.h>
#include <shellapi.h>
#include <timeapi.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib") 

#define _WITH_SWAPCHAIN_FULLSCREEN_STAT

using namespace DirectX;
using namespace DirectX::PackedVector;
using Microsoft::WRL::ComPtr;

#define FRAME_BUFFER_WIDTH 800
#define FRAME_BUFFER_HEIGHT 600
