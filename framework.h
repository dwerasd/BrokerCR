// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <DarkCore/DPrint.h>
#include <DarkCore/DLog.h>
#include <DarkCore/DConfig.h>
#include <DarkCore/DMemory.h>
#include <DarkCore/DEvent.h>
#pragma comment(lib, "DarkCore")

#include <Defines/ClosedDays.h>

extern dk::C_CONFIG* g_pConfig;
extern dk::C_LOG* g_pLog;

inline constexpr bool _DEBUG_MODE_{ false };
inline constexpr bool _FORCE_APPLY_TEST_CODE_{ false };
inline constexpr wchar_t szTestCode[] = L"005930";	// 삼성전자
//inline constexpr wchar_t szTestCode[] = L"001470";	// 삼부토건
//inline constexpr wchar_t szTestCode[] = L"000660";	// SK하이닉스
//inline constexpr wchar_t szTestCode[] = L"000020";	// 동화약품
//inline constexpr wchar_t szTestCode[] = L"034020";	// 두산에너빌리티
//inline constexpr wchar_t szTestCode[] = L"215600";	// 신라젠
//inline constexpr wchar_t szTestCode[] = L"393210";	// 토마토시스템
//inline constexpr wchar_t szTestCode[] = L"025550";	// 한국선재