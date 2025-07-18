#include "framework.h"
#include "CMain.h"


//#define _DBGPRINT_CMAIN_

namespace
{
	thread_local std::wstring wstrBuffer;
	thread_local wchar_t szDefaultPath[1024]{};
}

C_MAIN::C_MAIN()
{
	::tagINITCOMMONCONTROLSEX iccx
	{
		sizeof(::tagINITCOMMONCONTROLSEX)
		, ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES | ICC_BAR_CLASSES
	};
	::InitCommonControlsEx(&iccx);
#if defined(_DBGPRINT_CMAIN_)
	// 초기화 루틴 내 추가
	//if (!dk::IsAVX512Supported()) {
	//	DBGPRINT(L"AVX512 미지원 CPU");
	//}
	if (!dk::IsAVX2Supported()) {
		DBGPRINT(L"AVX2 미지원 CPU");
	}
	if (!dk::IsSSE2Supported()) {
		DBGPRINT(L"SSE2 미지원 CPU");
	}
	if (!dk::IsSSE4Supported()) {
		DBGPRINT(L"SSE4 미지원 CPU");
	}
	DBGPRINT(L"분봉개수: %d", _MAX_MIN_CANDLE_COUNT_);
#endif
}

C_MAIN::~C_MAIN()
{

}

void C_MAIN::save_rect()
{
	dk::DRECT rc;
	this->GetWindowRect(&rc);

	// 현재 상태가 최소화 상태인지 확인한다.
	if (!this->IsIconic())
	{
		::WritePrivateProfileStringW(L"location", L"x", std::to_tstring(rc.left).c_str(), g_pConfig->get_config_path());
		::WritePrivateProfileStringW(L"location", L"y", std::to_tstring(rc.top).c_str(), g_pConfig->get_config_path());
		::WritePrivateProfileStringW(L"location", L"w", std::to_tstring(rc.Width()).c_str(), g_pConfig->get_config_path());
		::WritePrivateProfileStringW(L"location", L"h", std::to_tstring(rc.Height()).c_str(), g_pConfig->get_config_path());
	}
}

void C_MAIN::move_rect()
{
	dk::DRECT rc;
	GetWindowRect(&rc);

	this->MoveWindow(
		g_pConfig->get_ini(L"location", L"x", (::GetSystemMetrics(SM_CXSCREEN) - rc.Width()) / 2)
		, g_pConfig->get_ini(L"location", L"y", (::GetSystemMetrics(SM_CYSCREEN) - rc.Height()) / 2)
		, g_pConfig->get_ini(L"location", L"w", rc.Width())
		, g_pConfig->get_ini(L"location", L"h", rc.Height())
	);
	//GetWindowRect(&rc);
	//DBGPRINT(_T("OnInitDialog() : %d, %d, %d, %d"), rc.left, rc.top, rc.Width(), rc.Height());
}

//static char szLine[32768] = { 0 }, szTemp[32768] = { 0 }, szData[32768] = { 0 };
bool C_MAIN::Init(HINSTANCE _h)
{
	bool bResult = false;
	dk::C_TIMER_QP 타이머;
	int nCurrentLine = __LINE__;
	try
	{
		do
		{
			DBGPRINT(L"C_MAIN::Init(start)");
			this->hInst = _h;

			nCurrentLine = __LINE__;
			// 로그
			if (!g_pLog) { g_pLog = new dk::C_LOG(); }
			if (g_pLog)
			{
				g_pLog->Init(L"log-BrokerCR");
				g_pLog->Write(L"로그 기록을 시작합니다(BrokerCR)");
			}

			nCurrentLine = __LINE__;
			if (!g_pConfig)
			{
				g_pConfig = new dk::C_CONFIG();
				if (!g_pConfig) { DBGPRINT(L"설정 생성 실패"); break; }
			}
			nCurrentLine = __LINE__;
			this->WTLInit(this->hInst);
			nCurrentLine = __LINE__;
			//::SetTimer(m_hWnd, 0, 1000, NULL);

			//this->umapTimerIDs[_TID_MS_10_] = 10;		// 타이머 아이디, 타이머 시간(밀리초)
			this->umapTimerIDs[_TID_MS_100_] = 100;		// 타이머 아이디, 타이머 시간(밀리초)
			//this->umapTimerIDs[_TID_MS_200_] = 200;		// 타이머 아이디, 타이머 시간(밀리초)
			//this->umapTimerIDs[_TID_MS_500_] = 500;		// 타이머 아이디, 타이머 시간(밀리초)
			//this->umapTimerIDs[_TID_MS_1000_] = 1000;	// 타이머 아이디, 타이머 시간(밀리초)
			//::SetTimer(m_hWnd, 0, 1000, NULL);
			for (auto& it : this->umapTimerIDs)
			{
				//UINT_PTR nTimerID = ::SetTimer(m_hWnd, it.first, it.second, NULL);
				// MFC 함수를 사용해서 타이머를 등록한다.
				const UINT_PTR nTimerID = this->SetTimer(it.first, it.second, NULL);
				DBGPRINT(L"[타이머] 등록 ID: %d, Interval: %d", nTimerID, it.second);
			}
			// 1. 먼저 MariaDB 초기화 및 연결
			this->pMariaDB = new C_MARIADB();
			if (this->pMariaDB)
			{
				if (!this->pMariaDB->Connect(g_pConfig->get_ini(L"database", L"address", L"")
					, static_cast<WORD>(g_pConfig->get_ini(L"database", L"port", 3306))
					, g_pConfig->get_ini(L"database", L"account", L"")
					, g_pConfig->get_ini(L"database", L"password", L"")
					, g_pConfig->get_ini(L"database", L"name", L"")
				))
				{
					DBGPRINT(L"DB 연결 실패");
					delete this->pMariaDB;
					this->pMariaDB = nullptr;
					break;
				}
				DBGPRINT(L"DB 연결 성공");
			}
			// 2. DB에서 기존 종목 코드 가져오기
			this->vStockDBCodes.clear();
			//this->pMariaDB->get_creon_noblock_stocks(this->vStockDBCodes);
			this->pMariaDB->get_creon_stocks(this->vStockDBCodes);
			const size_t nDBStockCount = this->vStockDBCodes.size();
			DBGPRINT(L"DB 기존 종목 수: %d", nDBStockCount);
			// 3. 크레온플러스 초기화
			this->pCreonPlus = new C_CREON_PLUS(
				g_pConfig->get_ini(L"account", L"creonplus_id", L"")
				, g_pConfig->get_ini(L"account", L"creonplus_pwd", L"")
				, g_pConfig->get_ini(L"account", L"creonplus_certpwd", L"")
			);
			if (this->pCreonPlus)
			{
				this->pCreonPlus->set_shutdown_handle(m_ShutdownEvent.GetEvent());
				if (!this->pCreonPlus->create())
				{
					break;
				}
			}
			// 4. 크레온에서 주식 정보 가져오기
			this->pCreonPlus->get_stock_infos(this->vStockCreonInfo);
			const size_t nCreonCount = this->vStockCreonInfo.size();
			DBGPRINT(L"%s - 크레온 종목 수: %d", __FUNCTIONW__, nCreonCount);

			// 5. 크레온 정보를 맵에 연결하고, DB에 없는 종목 삽입
			this->umapCreonStockInfo.clear();
			this->umapCreonStockInfo.reserve(nCreonCount);
			this->vUpdateStockInfos.clear();
			// 화이트리스트 정의
			constexpr wchar_t szWhiteList[][7] =
			{
				L"122630", L"252670", L"233740", L"251340"
			};
			// DB에 있는 종목 코드를 빠른 검색을 위해 set으로 변환
			std::unordered_set<std::wstring> dbCodesSet;
			for (const auto& code : this->vStockDBCodes)
			{
				dbCodesSet.insert(code);
			}

			for (size_t i = 0; i < nCreonCount; ++i)
			{
				const _STOCK_INFO_CREON& pCurrentStockInfo = this->vStockCreonInfo[i];
				// 맵에 모든 종목 정보 연결
				this->umapCreonStockInfo[pCurrentStockInfo.code.c_str()] = &this->vStockCreonInfo[i];
				// 코스피/코스닥 종목만 필터링
				if (CPUTILLib::CPC_MARKET_KOSPI != pCurrentStockInfo.nMarketKind &&
					CPUTILLib::CPC_MARKET_KOSDAQ != pCurrentStockInfo.nMarketKind)
				{
					continue;
				}
				// 추가 필터링 조건
				if (CPUTILLib::CPC_KSE_SECTION_KIND_ST != pCurrentStockInfo.nKseSectionKind ||
					pCurrentStockInfo.nSpac)
				{
					// ETF 화이트리스트 체크
					if (CPUTILLib::CPC_KSE_SECTION_KIND_ETF == pCurrentStockInfo.nKseSectionKind)
					{
						bool bWhiteList = false;
						for (size_t j = 0; j < _countof(szWhiteList); ++j)
						{
							if (0 == ::wcscmp(szWhiteList[j], pCurrentStockInfo.code.c_str()))
							{
								bWhiteList = true;
								break;
							}
						}
						if (!bWhiteList) { continue; }
					}
					else
					{
						continue;
					}
				}
				// 데이터베이스에 없는 종목만 INSERT
				if (dbCodesSet.find(pCurrentStockInfo.code.c_str()) == dbCodesSet.end()
					&& 0 == pCurrentStockInfo.nStatus
					)
				{
					this->pMariaDB->insert_creon_code(
						pCurrentStockInfo.code.c_str(),
						pCurrentStockInfo.creon_code.c_str(),
						pCurrentStockInfo.full_code.c_str(),
						pCurrentStockInfo.stock_name.c_str(),
						pCurrentStockInfo.nMarketKind
					);
					//FMTPRINT(L"종목코드: {}, 크레온코드: {}, 풀코드: {}, 종목명: {}, 옵션: {}"
					DBGPRINT(L"종목코드: %s, 크레온코드: %s, 풀코드: %s, 종목명: %s, 옵션: %d"
						, pCurrentStockInfo.code.c_str()
						, pCurrentStockInfo.creon_code.c_str()
						, pCurrentStockInfo.full_code.c_str()
						, pCurrentStockInfo.stock_name.c_str()
						, pCurrentStockInfo.nKseSectionKind
					);
				}
				// 업데이트할 종목 정보 저장
				this->vUpdateStockInfos.push_back(&this->vStockCreonInfo[i]);
			}
			DBGPRINT(L"필터 종목 개수: %d", this->vUpdateStockInfos.size());
			// 6. DB 종목 중 크레온에서 status가 0이 아닌 종목 확인 및 처리
			for (size_t i = 0; i < this->vStockDBCodes.size(); ++i)
			{
				const _STOCK_INFO_CREON* pCurrentStockInfo = this->get_creon_stock_info(this->vStockDBCodes[i]);

				if (nullptr == pCurrentStockInfo)
				{
					DBGPRINT(L"%s - 크레온 정보 없음", this->vStockDBCodes[i].c_str());
					continue;
				}
				if (!pCurrentStockInfo->nStatus)
				{
					//DBGPRINT(L"%s - 정상", this->vStockDBCodes[i].c_str());
					// 정상 종목으로 업데이트
					this->pMariaDB->update_code_block(this->vStockDBCodes[i].c_str(), 0);
					continue;
				}
				else
				{
					DBGPRINT(L"%s - 정상 아님 제거", this->vStockDBCodes[i].c_str());
					this->pMariaDB->update_code_block(this->vStockDBCodes[i].c_str());
					// 여기서 DB에서 제거하는 코드 추가 가능
					this->vStockDBCodes.erase(this->vStockDBCodes.begin() + i);
					--i;  // 인덱스 조정 필요
					continue;
				}

				// 크레온 코드와 풀코드 업데이트
				//this->pMariaDB->update_code_info(
				//	this->vStockDBCodes[i].c_str(),
				//	pCurrentStockInfo->creon_code.c_str(),
				//	pCurrentStockInfo->full_code.c_str()
				//);
			}
			DBGPRINT(L"유효한 DB 종목 수: %d", this->vStockDBCodes.size());
			bResult = true;
		} while (false);
	}
	catch (...)
	{
		DBGPRINT(L"예외발생: %s(%d) - error: %d, %s", __FILEW__, nCurrentLine, ::GetLastError(), dk::GetLastErrorMessageW());
	}

	DBGPRINT(L"C_MAIN::Init(end)");
	return(bResult);
}

int C_MAIN::Display()
{
	return this->WTLRun();
}

void C_MAIN::Destroy() noexcept
{
	int nCurrentLine = __LINE__;
	try
	{
		DBGPRINT(L"C_MAIN::Destroy(start)");
		// 스레드가 실행중이면 종료 플래그를 설정한다.
		//if (this->IsOnThread()) { this->set_exit_thread_flag(); }

		// 타이머를 해제한다.
		for (auto& it : this->umapTimerIDs)
		{
			//if (::KillTimer(m_hWnd, it.first)) { DBGPRINT(L"[타이머] 해제 ID: %d, Interval: %d", it.first, it.second); }
			// MFC 함수를 사용해서 타이머를 해제한다.
			if (this->KillTimer(it.first)) { DBGPRINT(L"[타이머] 해제 ID: %d, Interval: %d", it.first, it.second); }
		}
		this->umapTimerIDs.clear();
		// 트레이 아이콘을 삭제한다.
		DBGPRINT(L"트레이 아이콘 삭제");
		this->delete_tray_icon();
		// 크레온 플러스 객체를 해제한다.
		DBGPRINT(L"크레온 플러스 해제");
		if (this->pCreonPlus)
		{
			delete this->pCreonPlus;
			this->pCreonPlus = nullptr;
		}
		// 마리아DB 객체를 해제한다.
		DBGPRINT(L"마리아DB 해제");
		if (this->pMariaDB)
		{
			delete this->pMariaDB;
			this->pMariaDB = nullptr;
		}
		// 설정을 종료한다.
		DBGPRINT(L"설정 종료");
		if (g_pConfig) { delete g_pConfig; g_pConfig = nullptr; }
		// 로그를 종료한다.
		DBGPRINT(L"로그 종료");
		if (g_pLog) { delete g_pLog; g_pLog = nullptr; }
	}
	catch (...)
	{
		DBGPRINT(L"예외발생: %s(%d)", __FILEW__, nCurrentLine);
	}
	DBGPRINT(L"C_MAIN::Destroy(end)");
}


void C_MAIN::CloseDialog(int _nVal)
{
	DBGPRINT(L"CloseDialog(%d)", _nVal);
	// 타이머를 해제한다.
	for (auto& it : this->umapTimerIDs)
	{
		//if (::KillTimer(m_hWnd, it.first)) { DBGPRINT(L"[타이머] 해제 ID: %d, Interval: %d", it.first, it.second); }
		// MFC 함수를 사용해서 타이머를 해제한다.
		if (this->KillTimer(it.first)) { DBGPRINT(L"[타이머] 해제 ID: %d, Interval: %d", it.first, it.second); }
	}
	this->umapTimerIDs.clear();

	this->save_rect();
	this->delete_tray_icon();
	this->DestroyWindow();

	this->_Module.RemoveMessageLoop();
	this->_Module.Term();

	::PostQuitMessage(_nVal);
}

void C_MAIN::create_tray_icon(HINSTANCE _hInst)
{
	this->delete_tray_icon();
	::_NOTIFYICONDATAW nid
	{
		sizeof(::_NOTIFYICONDATAW)
		, this->m_hWnd
		, 0
		, NIF_ICON | NIF_MESSAGE | NIF_TIP
		, WM_TRAYICON
		, ::LoadIconW(_hInst, MAKEINTRESOURCEW(IDI_BROKERCR))
	};
	// 타이틀을 얻어온다. CString 은 사용할 수 없다.
	::GetWindowTextW(this->m_hWnd, nid.szTip, _countof(nid.szTip));
	// 트레이 아이콘 생성
	::Shell_NotifyIconW(NIM_ADD, &nid);
}

void C_MAIN::delete_tray_icon()
{
	::_NOTIFYICONDATAW nid
	{
		sizeof(::_NOTIFYICONDATAW)
		, this->m_hWnd
		, 0
	};
	// 트레이 아이콘 제거
	::Shell_NotifyIconW(NIM_DELETE, &nid);
}


LRESULT C_MAIN::OnTrayMenu(UINT /*uMsg*/, WPARAM /*_wParam*/, LPARAM _lParam)
{
	switch (LOWORD(_lParam))
	{
	case WM_LBUTTONDBLCLK:
		//DBGPRINT(L"트레이 더블클릭");
		{
			this->ShowWindow(SW_SHOWDEFAULT);
		}
		break;
	case WM_RBUTTONUP:
		//DBGPRINT(L"트레이 우클릭");
		{
			dk::DPOINT ptMouse;
			::GetCursorPos(&ptMouse);

			WTL::CMenu menu;
			menu.LoadMenu(IDR_MENU_TRAY);

			//CMenu* pMenu = menu.GetSubMenu(0);
			// MFC 가 아니라 WTL 을 사용하므로 이렇게 사용한다.
			WTL::CMenuHandle pMenu = menu.GetSubMenu(0);
			pMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptMouse.x, ptMouse.y, this->m_hWnd);

			// 커스텀 메시지를 보내서 메뉴를 닫음
			::PostMessageW(this->m_hWnd, WM_EXITMENULOOP, 0, 0);

			// 메뉴를 닫음
			pMenu.DestroyMenu();
		}
		break;
	}
	return(0);
}


LRESULT C_MAIN::OnTrayClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//DBGPRINT(L"C_MAIN::OnTrayClose()");
	this->CloseDialog(0);
	return(0);
}


LRESULT C_MAIN::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DBGPRINT(L"다이얼로그 초기화 - %s", __FUNCTIONW__);
	// center the dialog on the screen
	//CenterWindow();
	this->create_tray_icon(hInst);
	this->move_rect();

	// set icons
	HICON hIcon = (HICON)::LoadImageW(this->_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_BROKERCR), IMAGE_ICON
		, ::GetSystemMetrics(SM_CXICON)
		, ::GetSystemMetrics(SM_CYICON)
		, LR_DEFAULTCOLOR
	);
	CDialogImpl::SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImageW(this->_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_SMALL), IMAGE_ICON
		, ::GetSystemMetrics(SM_CXSMICON)
		, ::GetSystemMetrics(SM_CYSMICON)
		, LR_DEFAULTCOLOR
	);
	CDialogImpl::SetIcon(hIconSmall, FALSE);

#if defined(_USE_ODLE_HANDLER_)
	// register object for message filtering
	WTL::CMessageLoop* pLoop = this->_Module.GetMessageLoop();
	//pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);
#endif



	DBGPRINT(L"다이얼로그 초기화 완료 - %s", __FUNCTIONW__);
	return TRUE;
}

LRESULT C_MAIN::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DBGPRINT(L"%s", __FUNCTIONW__);
	this->nShutdownScheduled.set();
	this->m_ShutdownEvent.Set();

	return(0);
}

LRESULT C_MAIN::OnCancel(WORD /*wNotifyCode*/, WORD _wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DBGPRINT(L"Cancel 버튼 눌림: %d", _wID);
	bool bCloseToTray = g_pConfig->get_ini(L"base", L"close_to_tray", false);
	if (!bCloseToTray)
	{
		// 여기는 종료 버튼이 눌렸을 때 들어온다.
		if (this->nWorking.is_set())
		{
			// 종료를 예약할건지 메시지를 띄운다.
			if (IDYES != ::MessageBoxW(this->m_hWnd, L"작업이 끝나지 않았습니다. 종료를 예약하시겠습니까?", L"종료", MB_YESNO))
			{
				return 0;
			}
			else
			{
				// 종료를 예약한다.
				this->nShutdownScheduled.set();
			}
		}
		this->CloseDialog(_wID);
	}
	else
	{
		this->ShowWindow(SW_MINIMIZE);
	}
	return 0;
}


LRESULT C_MAIN::OnSysCommand(UINT /*uMsg*/, WPARAM _wParam, LPARAM /*lParam*/, BOOL& _bHandled)
{
	_wParam;
	//UINT uCmdType = (UINT)_wParam;

	//if ((uCmdType & 0xFFF0) == IDM_ABOUTBOX)
	//{
	//	CAboutDlg dlg;
	//	dlg.DoModal();
	//}
	//else
	_bHandled = FALSE;

	return 0;
}


void C_MAIN::update()
{
	if (this->pCreonPlus)
	{
		if (!_FORCE_APPLY_TEST_CODE_)
		{
			//this->creon_update(_DEBUG_MODE_ ? szTestCode : nullptr);
			this->creon_update(_DEBUG_MODE_ ? szTestCode : nullptr);
		}
		else
		{
			this->creon_update(szTestCode);
		}
	}
}

LRESULT C_MAIN::OnBnClickedBtnUpdate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DBGPRINT(L"업데이트 버튼 눌림");
	// 여기는 실행 버튼이 눌렸을 때 들어온다.
	int nCurrentLine = __LINE__;
	try
	{
		// 만약 업데이트 스레드가 실행중이 아니라면 업데이트 스레드를 시작한다.
		// this->IsOnThread() 를 사용한다
		//if (!this->IsOnThread())
		//{
		//	this->ThreadStart();
		//	// 스레드가 시작될때까지 대기한다.
		//	do { dk::Sleep(10); } while (!this->IsOnThread());
		//}
		this->update();

		if (this->pCreonPlus)
		{
			this->pCreonPlus->test_creon();
			// 분봉 변환
			//this->trans_mins();	// 완료함

			// 중복 데이터 체크


			//std::vector<_TICK_DATA> vCandleTick;
			//this->get_tick_data(vCandleTick, szCode);
			//const size_t nCount = vCandleTick.size();
			//DBGPRINT(L"%s - %d", __FUNCTIONW__, nCount);
			//
			//for (size_t i = 0; i < nCount; ++i)
			//{
			//	DBGPRINT(L"[%s] %d - %d / %d / %d / %d / %d / %d / %I64u / %d / %d", __FUNCTIONW__
			//		, vCandleTick[i].nDate
			//		, vCandleTick[i].nTime
			//		, vCandleTick[i].nOpen
			//		, vCandleTick[i].nHigh
			//		, vCandleTick[i].nLow
			//		, vCandleTick[i].nClose
			//		, vCandleTick[i].nVolume
			//		, vCandleTick[i].nTransferPayment
			//		, vCandleTick[i].nAccrueVolumeSell
			//		, vCandleTick[i].nAccrueVolumeBuy
			//	);
			//}

			//std::vector<_CANDLE_MIN> vCandleSec;
			//this->pCreonPlus->get_min_data(vCandleSec, pCurrentStockInfo->creon_code.c_str());
			//const size_t nCount = vCandleSec.size();
			//DBGPRINT(L"%s - %d", __FUNCTIONW__, nCount);

			//for (size_t i = 0; i < nCount; ++i)
			//{
			//	DBGPRINT(L"[%s] %08d %04d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %d"
			//		L" 매도량: %d, 매수량: %d, 순매수량: %d, 거래대금: %I64u"
			//		, pCurrentStockInfo->code.c_str()
			//		, vCandleSec[i].nDate
			//		, vCandleSec[i].nTime
			//		, vCandleSec[i].nOpen
			//		, vCandleSec[i].nHigh
			//		, vCandleSec[i].nLow
			//		, vCandleSec[i].nClose
			//		, vCandleSec[i].nVolume
			//		, vCandleSec[i].nAccrueVolumeSell
			//		, vCandleSec[i].nAccrueVolumeBuy
			//		, vCandleSec[i].nAccrueVolumeSell - vCandleSec[i].nAccrueVolumeBuy
			//		//, vCandleSec[i].nAccruePaymentSell
			//		//, vCandleSec[i].nAccruePaymentBuy
			//		, vCandleSec[i].nTransferPayment
			//	);
			//}

			//std::vector<_CANDLE_WEEK> vCandleWeek;
			//this->pCreonPlus->get_week_data(vCandleWeek, pCurrentStockInfo->creon_code.c_str());
			//const size_t nWeekCount = vCandleWeek.size();
			//DBGPRINT(L"%s - %d", __FUNCTIONW__, nWeekCount);
			//for (size_t i = 0; i < nWeekCount; ++i)
			//{
			//	DBGPRINT(L"[%s] - %08d - %d / %d / %d / %d / %I64u"
			//		, pCurrentStockInfo->code.c_str()
			//		, vCandleWeek[i].nDate
			//		, vCandleWeek[i].nOpen
			//		, vCandleWeek[i].nHigh
			//		, vCandleWeek[i].nLow
			//		, vCandleWeek[i].nClose
			//		, vCandleWeek[i].nVolume
			//	);
			//}
			//std::vector<_CANDLE_MONTH> vCandleMonth;
			//this->pCreonPlus->get_month_data(vCandleMonth, pCurrentStockInfo->creon_code.c_str());
			//const size_t nMonthCount = vCandleMonth.size();
			//DBGPRINT(L"%s - %d", __FUNCTIONW__, nMonthCount);
			//for (size_t i = 0; i < nMonthCount; ++i)
			//{
			//	DBGPRINT(L"[%s] %08d - %d / %d / %d / %d / %I64u"
			//		, pCurrentStockInfo->code.c_str()
			//		, vCandleMonth[i].nDate
			//		, vCandleMonth[i].nOpen
			//		, vCandleMonth[i].nHigh
			//		, vCandleMonth[i].nLow
			//		, vCandleMonth[i].nClose
			//		, vCandleMonth[i].nVolume
			//	);
			//}

		}
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
	DBGPRINT(L"업데이트 버튼 눌림 종료");
	this->CloseDialog(0);
	return 0;
}

LRESULT C_MAIN::OnBnClickedBtnWMUpdate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (this->pCreonPlus)
	{
		if (!_FORCE_APPLY_TEST_CODE_)
		{
			//this->creon_update(_DEBUG_MODE_ ? szTestCode : nullptr);
			this->creon_update_WM(_DEBUG_MODE_ ? szTestCode : nullptr);
		}
		else
		{
			this->creon_update_WM(szTestCode);
		}
	}
	DBGPRINT(L"WM 업데이트 버튼 눌림 종료");
	return 0;
}

_STOCK_INFO_CREON* C_MAIN::get_creon_stock_info(LPCWSTR _pCode)
{
	std::unordered_map<std::wstring, _STOCK_INFO_CREON*>::iterator it = this->umapCreonStockInfo.find(_pCode);
	if (this->umapCreonStockInfo.end() != it)
	{
		return it->second;
	}
	return(nullptr);
}

void C_MAIN::creon_update_day(_STOCK_INFO_CREON* _pCreonStockInfo, bool _bAppend)
{
	const std::wstring szFilePath = generate_path(_pCreonStockInfo->code.c_str(), L"day");

	// 시작 날짜 세팅
	ULONG nStartDate = 19700101;
	ULONG nLastestDate = 0;
	if (_bAppend)
	{	// 디비에 마지막 날짜 이후의 데이터를 기록한다.
		// 디비에 저장된 마지막 날짜를 얻어온다.
		//nLastestDate = this->pMariaDB->get_latest_date(pCurrentStockInfo->code.c_str(), "kr_stock_daily_base");
		// 파일에서 가장 최근 날짜를 얻어온다.
		_CANDLE_DAY candle_day;
		get_static_lastdata<_CANDLE_DAY>(szFilePath, candle_day);

		if (candle_day.nDate < 19700101) { nStartDate = 19700101; }
		else
		{
			nLastestDate = candle_day.nDate;
#if defined(_DBGPRINT_CMAIN_)
			if (_DEBUG_MODE_)
			{
				DBGPRINT(L"[%s] %08d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %d, 대금: %I64u / %I64u / %I64u"
					, _pCreonStockInfo->code.c_str()
					, candle_day.nDate
					, candle_day.nOpen
					, candle_day.nHigh
					, candle_day.nLow
					, candle_day.nClose
					, candle_day.nVolume
					, candle_day.nTransferPayment
					, candle_day.nListedStocks
				);
			}
#endif
			// 얻어올 날짜를 얻어온다.
			nStartDate = dk::get_next_day(candle_day.nDate);
		}
	}

	// 오늘 날짜를 얻어온다.
	const dk::_DLOCAL_TIME lt;
	ULONG nToday = lt.wYear * 10000 + lt.wMonth * 100 + lt.wDay;

	// 끝 날자 세팅
	ULONG nEndDate = nToday;
	if (lt.wHour < 18) // 18시 이전이라면 어제 날짜를 얻는다.
	{
		nEndDate = dk::get_prev_day(nToday);
	}

	if (nStartDate <= nEndDate)
	{
		//DBGPRINT(L"파일 경로: %s", szFilePath.c_str());
		//DBGPRINT(L"[%s] %d - %d", _pCreonStockInfo->code.c_str(), nStartDate, nEndDate);
		// 크레온에서 데이터를 얻어온다.
		std::vector<_CANDLE_DAY> vCandleDays;
		this->pCreonPlus->get_day_data(vCandleDays, _pCreonStockInfo->creon_code.c_str(), nStartDate, nEndDate);
		const size_t nDayCount = vCandleDays.size();
		if (2 < nDayCount || (1 == nDayCount && dk::IsOverDate(nLastestDate, vCandleDays[0].nDate)))
		{
			// 출력
			//for (size_t i = 0; i < nDayCount; ++i)
			//{
			//	DBGPRINT(L"[%s] %08d - %d / %d / %d / %d / %d / %I64u / %I64u / %I64u"
			//		, _pCreonStockInfo->code.c_str()
			//		, vCandleDays[i].nDate
			//		, vCandleDays[i].nOpen
			//		, vCandleDays[i].nHigh
			//		, vCandleDays[i].nLow
			//		, vCandleDays[i].nClose
			//		, vCandleDays[i].nVolume
			//		, vCandleDays[i].nTransferPayment
			//		, vCandleDays[i].nListedStocks
			//		, vCandleDays[i].nMarketCap
			//	);
			//}

			// 디비에 시가총액 정보를 업데이트 한다.
			this->pMariaDB->update_listedstocks(_pCreonStockInfo->code.c_str(), vCandleDays[0].nListedStocks);

			// 디비에 일봉 데이터를 업데이트 한다.
			//this->pMariaDB->update_daily_base(_pCreonStockInfo->code.c_str(), vCandleDays);


			// vCandleDays 를 역순으로 정렬한다.
			std::reverse(vCandleDays.begin(), vCandleDays.end());
			// 파일에 저장한다.
			if (!_DEBUG_MODE_)
				save_vector<_CANDLE_DAY>(szFilePath, vCandleDays);
#if defined(_DBGPRINT_CMAIN_)
			else
			{
				// 출력
				for (size_t i = 0; i < nDayCount; ++i)
				{
					DBGPRINT(L"[%s] %08d - 시: %d / 고: %d / 저: %d / 종: %d / 량: %d / 대금: %I64u / 시총: %I64u"
						, _pCreonStockInfo->code.c_str()
						, vCandleDays[i].nDate
						, vCandleDays[i].nOpen
						, vCandleDays[i].nHigh
						, vCandleDays[i].nLow
						, vCandleDays[i].nClose
						, vCandleDays[i].nVolume
						, vCandleDays[i].nTransferPayment
						, vCandleDays[i].nMarketCap
					);
				}
			}
			DBGPRINT(L"[%s] 일봉 %d 개 추가 완료", _pCreonStockInfo->code.c_str(), nDayCount);
#endif
			// 읽어서 출력해본다.
			// 우선 파일이 있는지 확인하고
			//if (1 == dk::FileExists(szFilePath))
			//{
			//	dk::C_HEAP_CTRL heapCtrl(szFilePath);
			//	if (0 == (heapCtrl.size() % sizeof(_CANDLE_DAY)))
			//	{	
			//		// 파일 크기가 _CANDLE_DAY 의 배수라면
			//		const size_t nCount = heapCtrl.size() / sizeof(_CANDLE_DAY);
			//		DBGPRINT(L"[%s] - %d", __FUNCTIONW__, nCount);
			//		// 메모리에서 읽어온다.
			//		_CANDLE_DAY* pCandleDay = (_CANDLE_DAY*)heapCtrl.get_mem();
			//		// 출력
			//		for (size_t i = 0; i < nCount; ++i)
			//		{
			//			DBGPRINT(L"[%s] %08d - %d / %d / %d / %d / %I64u / %I64u / %I64u"
			//				, pCurrentStockInfo->code.c_str()
			//				, pCandleDay[i].nDate
			//				, pCandleDay[i].nOpen
			//				, pCandleDay[i].nHigh
			//				, pCandleDay[i].nLow
			//				, pCandleDay[i].nClose
			//				, pCandleDay[i].nVolume
			//				, pCandleDay[i].nTransferPayment
			//				, pCandleDay[i].nMarketCap
			//			);
			//		}
			//	}
			//	else
			//	{
			//		DBGPRINT(L"[%s] - %d", __FUNCTIONW__, heapCtrl.size());
			//		// 파일 크기가 _CANDLE_DAY 의 배수가 아니라면
			//		// 에러를 출력한다.
			//		DBGPRINT(L"파일 크기 오류: %s", szFilePath);
			//	}
			//}
		}
	}
#if defined(_DBGPRINT_CMAIN_)
	else
	{
		DBGPRINT(L"[%s] 일봉 패스", _pCreonStockInfo->code.c_str());
	}
#endif
}

void C_MAIN::creon_update_week(_STOCK_INFO_CREON* _pCreonStockInfo)
{
	wstrBuffer.clear();

	const std::wstring szFilePath = generate_path(_pCreonStockInfo->code.c_str(), L"week");

	_CANDLE_WEEK candle_week;
	get_static_lastdata<_CANDLE_WEEK>(szFilePath, candle_week);
#if defined(_DBGPRINT_CMAIN_)
	if (_DEBUG_MODE_)
	{
		DBGPRINT(L"[%s] %08d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %I64u"
			, _pCreonStockInfo->code.c_str()
			, candle_week.nDate
			, candle_week.nOpen
			, candle_week.nHigh
			, candle_week.nLow
			, candle_week.nClose
			, candle_week.nVolume
		);
	}
#endif
	bool bFileExists = 1 == dk::FileExists(szFilePath);
	//DBGPRINT(L"파일 경로: %s", szFilePath.c_str());
	std::vector<_CANDLE_WEEK> vCandleWeek;
	const size_t nWeekCount = this->pCreonPlus->get_week_data(vCandleWeek, _pCreonStockInfo->creon_code.c_str(), !bFileExists);
	if (0 < nWeekCount)
	{
		if (1 == nWeekCount && candle_week.nVolume != vCandleWeek[0].nDate)
		{
			if (candle_week.nDate == vCandleWeek[0].nDate && candle_week.nVolume < vCandleWeek[0].nDate)
			{	// 날짜가 같고, 볼륨이 더 크다면 덮어쓰기 한다.
				wstrBuffer = L"업데이트";
				if (!_DEBUG_MODE_)
					update_static_lastdata<_CANDLE_WEEK>(szFilePath, vCandleWeek[0]);
#if defined(_DBGPRINT_CMAIN_)
				else
					DBGPRINT(L"[%s] 주봉 업데이트 %08d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %I64u"
						, _pCreonStockInfo->code.c_str()
						, vCandleWeek[0].nDate
						, vCandleWeek[0].nOpen
						, vCandleWeek[0].nHigh
						, vCandleWeek[0].nLow
						, vCandleWeek[0].nClose
						, vCandleWeek[0].nVolume
					);
#endif
			}
			else if (candle_week.nDate < vCandleWeek[0].nDate)
			{	// 새로운 주봉이 추가되었다면
				wstrBuffer = L"추가";
				if (!_DEBUG_MODE_)
					save_vector<_CANDLE_WEEK>(szFilePath, vCandleWeek);
#if defined(_DBGPRINT_CMAIN_)
				else
					DBGPRINT(L"[%s] 주봉 추가 %08d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %I64u"
						, _pCreonStockInfo->code.c_str()
						, vCandleWeek[0].nDate
						, vCandleWeek[0].nOpen
						, vCandleWeek[0].nHigh
						, vCandleWeek[0].nLow
						, vCandleWeek[0].nClose
						, vCandleWeek[0].nVolume
					);
#endif
			}

			else
			{
				wstrBuffer = L"보류";
#if defined(_DBGPRINT_CMAIN_)
				DBGPRINT(L"[%s] 주봉 확인 %08d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %I64u"
					, _pCreonStockInfo->code.c_str()
					, vCandleWeek[0].nDate
					, vCandleWeek[0].nOpen
					, vCandleWeek[0].nHigh
					, vCandleWeek[0].nLow
					, vCandleWeek[0].nClose
					, vCandleWeek[0].nVolume
				);
#endif
			}

		}
		else
		{	// 데이터가 2개 이상이라면
			wstrBuffer = L"업데이트";
			std::reverse(vCandleWeek.begin(), vCandleWeek.end());

			if (!_DEBUG_MODE_)
				save_vector<_CANDLE_WEEK>(szFilePath, vCandleWeek);
#if defined(_DBGPRINT_CMAIN_)
			else
				for (size_t i = 0; i < nWeekCount; ++i)
				{
					DBGPRINT(L"[%s] 주봉 추가2 %08d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %I64u"
						, _pCreonStockInfo->code.c_str()
						, vCandleWeek[i].nDate
						, vCandleWeek[i].nOpen
						, vCandleWeek[i].nHigh
						, vCandleWeek[i].nLow
						, vCandleWeek[i].nClose
						, vCandleWeek[i].nVolume
					);
				}
#endif
		}
#if defined(_DBGPRINT_CMAIN_)
		DBGPRINT(L"[%s] 주봉 %d개 %s 완료", _pCreonStockInfo->code.c_str(), nWeekCount, wstrBuffer.c_str());
#endif
	}
}

void C_MAIN::creon_update_month(_STOCK_INFO_CREON* _pCreonStockInfo)
{
	wstrBuffer.clear();

	const std::wstring szFilePath = generate_path(_pCreonStockInfo->code.c_str(), L"month");

	_CANDLE_MONTH candle_month;
	get_static_lastdata<_CANDLE_MONTH>(szFilePath, candle_month);
#if defined(_DBGPRINT_CMAIN_)
	if (_DEBUG_MODE_)
	{
		DBGPRINT(L"[%s] %08d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %I64u"
			, _pCreonStockInfo->code.c_str()
			, candle_month.nDate
			, candle_month.nOpen
			, candle_month.nHigh
			, candle_month.nLow
			, candle_month.nClose
			, candle_month.nVolume
		);
	}
#endif
	bool bFileExists = 1 == dk::FileExists(szFilePath);
	//DBGPRINT(L"파일 경로: %s", szFilePath.c_str());
	std::vector<_CANDLE_MONTH> vCandleMonth;
	const size_t nWeekCount = this->pCreonPlus->get_month_data(vCandleMonth, _pCreonStockInfo->creon_code.c_str(), !bFileExists);
	if (0 < nWeekCount)
	{
		if (1 == nWeekCount)
		{
			if (candle_month.nDate == vCandleMonth[0].nDate && candle_month.nVolume < vCandleMonth[0].nDate)
			{	// 날짜가 같고, 볼륨이 더 크다면 덮어쓰기 한다.
				wstrBuffer = L"업데이트";
				if (!_DEBUG_MODE_)
					update_static_lastdata<_CANDLE_MONTH>(szFilePath, vCandleMonth[0]);
#if defined(_DBGPRINT_CMAIN_)
				else
					DBGPRINT(L"[%s] 월봉 업데이트 %08d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %I64u"
						, _pCreonStockInfo->code.c_str()
						, vCandleMonth[0].nDate
						, vCandleMonth[0].nOpen
						, vCandleMonth[0].nHigh
						, vCandleMonth[0].nLow
						, vCandleMonth[0].nClose
						, vCandleMonth[0].nVolume
					);
#endif
			}
			else if (candle_month.nDate < vCandleMonth[0].nDate)
			{	// 새로운 월봉이 추가되었다면
				wstrBuffer = L"추가";
				if (!_DEBUG_MODE_)
					save_vector<_CANDLE_MONTH>(szFilePath, vCandleMonth);
#if defined(_DBGPRINT_CMAIN_)
				else
					// 출력
					DBGPRINT(L"[%s] 월봉 추가 %08d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %I64u"
						, _pCreonStockInfo->code.c_str()
						, vCandleMonth[0].nDate
						, vCandleMonth[0].nOpen
						, vCandleMonth[0].nHigh
						, vCandleMonth[0].nLow
						, vCandleMonth[0].nClose
						, vCandleMonth[0].nVolume
					);
#endif
			}
			else if (candle_month.nDate > vCandleMonth[0].nDate)
			{	// 월봉 확인을 한다.
				wstrBuffer = L"보류";
#if defined(_DBGPRINT_CMAIN_)
				DBGPRINT(L"[%s] 월봉 확인 %08d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %I64u"
					, _pCreonStockInfo->code.c_str()
					, vCandleMonth[0].nDate
					, vCandleMonth[0].nOpen
					, vCandleMonth[0].nHigh
					, vCandleMonth[0].nLow
					, vCandleMonth[0].nClose
					, vCandleMonth[0].nVolume
				);
#endif
			}
		}
		else
		{
			wstrBuffer = L"업데이트";
			std::reverse(vCandleMonth.begin(), vCandleMonth.end());

			if (!_DEBUG_MODE_)
				save_vector<_CANDLE_MONTH>(szFilePath, vCandleMonth);
#if defined(_DBGPRINT_CMAIN_)
			else
				for (size_t i = 0; i < nWeekCount; ++i)
				{
					DBGPRINT(L"[%s] 월봉 추가2 %08d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %I64u"
						, _pCreonStockInfo->code.c_str()
						, vCandleMonth[i].nDate
						, vCandleMonth[i].nOpen
						, vCandleMonth[i].nHigh
						, vCandleMonth[i].nLow
						, vCandleMonth[i].nClose
						, vCandleMonth[i].nVolume
					);
				}
#endif
		}
#if defined(_DBGPRINT_CMAIN_)
		DBGPRINT(L"[%s] 월봉 %d개 %s 완료", _pCreonStockInfo->code.c_str(), nWeekCount, wstrBuffer.c_str());
#endif
	}
}

void C_MAIN::creon_update_min(_STOCK_INFO_CREON* _pCreonStockInfo, size_t _nIndex, bool _bAppend)
{
	// 파일 경로 세팅
	const std::wstring szFilePath = generate_path(_pCreonStockInfo->code.c_str(), L"min", _분봉의길이들_[_nIndex]);

	// 시작 날짜 세팅
	ULONG nStartDate = 19700101;
	ULONG nLastestDate = 0;
	if (_bAppend)
	{
		// 파일에서 가장 최근 날짜를 얻어온다.
		_CANDLE_MIN candle_min;
		get_static_lastdata<_CANDLE_MIN>(szFilePath, candle_min);
		ULONG nTime;
		candle_min.get_datetime(&nLastestDate, &nTime);
#if defined(_DBGPRINT_CMAIN_)
		if (_DEBUG_MODE_)
		{
			DBGPRINT(L"[%s] %08d - %04d, 시: %d, 고: %d, 저: %d, 종: %d, 량: %d, 대금: %I64u"
				, _pCreonStockInfo->code.c_str()
				, nLastestDate
				, nTime
				, candle_min.nOpen
				, candle_min.nHigh
				, candle_min.nLow
				, candle_min.nClose
				, candle_min.nVolume
				, candle_min.nTransferPayment
			);
		}
#endif
		// 얻어올 날짜를 얻어온다.
		nStartDate = dk::get_next_day(nLastestDate);
	}
	const dk::_DLOCAL_TIME lt;
	ULONG nToday = (lt.wYear * 10000 + lt.wMonth * 100 + lt.wDay);
	// 날짜의 차이를 구한다.
	ULONG nCandleCount = dk::get_days_between(nToday, nStartDate);
	// 만약 차이가 없다면 오후 8시 5분이 지났는지 확인한다.
	if (!nCandleCount)
	{
		const ULONG nCurrentDate = _YMD_TO_ULONG_(lt.wYear, lt.wMonth, lt.wDay);
		C_CLOSED_DAYS closedDays;
		bool bClosedDay = false;
		if (!lt.wDayOfWeek || 6 == lt.wDayOfWeek
			|| closedDays.IsClosedDay(nCurrentDate)	// 휴장일이라면 실행하지 않는다.
			)
		{	// 일요일 또는 토요일이라면 상관 없다.
			bClosedDay = true;
		}
		// 우선 날짜가 동일한지 확인한다.
		if (nToday == nStartDate && !bClosedDay)
		{
			// 오전 5시 55분부터 오후 8시 5분 사이인지 확인
			bool isAfter555 = lt.wHour > 5 || (lt.wHour == 5 && lt.wMinute >= 55);		// 오후 5시 55분 이후
			bool isBefore2005 = lt.wHour < 20 || (lt.wHour == 20 && lt.wMinute <= 5);	// 오후 8시 5분 이전
			if (isAfter555 && isBefore2005)
			{
				DBGPRINT(L"[%s] %d분봉 업데이트 보류, 사유: 오전 5시 55분 ~ 오후 8시 5분 사이", _pCreonStockInfo->code.c_str(), _분봉의길이들_[_nIndex]);
				return;
			}
		}
		++nCandleCount;
	}
	nCandleCount *= _분봉의개수_[_nIndex];
	if (0 < nCandleCount)
	{
#if defined(_DBGPRINT_CMAIN_)
		if (_DEBUG_MODE_)
		{
			DBGPRINT(L"[%s] %d분봉 경로: %s, 얻어올 데이터 개수: %d", _pCreonStockInfo->code.c_str(), _분봉의길이들_[_nIndex], szFilePath.c_str(), nCandleCount);
		}
#endif
		// 크레온에서 데이터를 얻어온다.
		std::vector<_CANDLE_MIN> vCandleMins;
		this->pCreonPlus->get_min_data(vCandleMins, _pCreonStockInfo->creon_code.c_str(), _분봉의길이들_[_nIndex], nCandleCount);

		if (!vCandleMins.empty())
		{
			//DBGPRINT(L"%s - %d", __FUNCTIONW__, vCandleMins.size());

			std::vector<_CANDLE_MIN> filteredCandles;
			filteredCandles.reserve(vCandleMins.size()); // 메모리 최적화
			std::copy_if(vCandleMins.begin(), vCandleMins.end(), std::back_inserter(filteredCandles),
				[&](const _CANDLE_MIN& candle) {
					ULONG nDate, nTime;
					candle.get_datetime(&nDate, &nTime); // _CANDLE_MIN의 get_datetime 사용
					return nDate > nLastestDate; // nLastestDate보다 이후 날짜만 선택
				});

			vCandleMins.clear();
			// 이제부터 filteredCandles 벡터를 사용합니다.
			const size_t nMinCount = filteredCandles.size();
			if (0 < nMinCount)
			{
				//DBGPRINT(L"[%s] - %d 개, 역순으로 저장한다", __FUNCTIONW__, nMinCount);

				// filteredCandles 를 역순으로 정렬한다.
				std::reverse(filteredCandles.begin(), filteredCandles.end());

				// 파일에 저장한다.
				if (!_DEBUG_MODE_)
					save_vector<_CANDLE_MIN>(szFilePath, filteredCandles);
#if defined(_DBGPRINT_CMAIN_)
				else
				{
					// 출력
					ULONG nDate, nTime;
					for (size_t i = 0; i < nMinCount; ++i)
					{
						filteredCandles[i].get_datetime(&nDate, &nTime);
						DBGPRINT(L"[%s] %08d - %04d, 시: %d, 고: %d, 저: %d, 종: %d, 량: %d, 대금: %I64u"
							, _pCreonStockInfo->code.c_str()
							, nDate
							, nTime
							, filteredCandles[i].nOpen
							, filteredCandles[i].nHigh
							, filteredCandles[i].nLow
							, filteredCandles[i].nClose
							, filteredCandles[i].nVolume
							, filteredCandles[i].nTransferPayment
						);
					}
				}
				DBGPRINT(L"[%s] %d분봉 %d 개 추가 완료", _pCreonStockInfo->code.c_str(), _분봉의길이들_[_nIndex], nMinCount);
#endif
			}
#if defined(_DBGPRINT_CMAIN_)
			else
			{
				DBGPRINT(L"[%s] %d분봉 업데이트 보류, 사유: 최신 데이터 없음", _pCreonStockInfo->code.c_str(), _분봉의길이들_[_nIndex]);
			}
#endif
			// 읽어서 출력해본다.
			// 우선 파일이 있는지 확인하고
			//if (1 == dk::FileExists(szFilePath))
			//{
			//	dk::C_HEAP_CTRL heapCtrl(szFilePath);
			//	if (0 == (heapCtrl.size() % sizeof(_CANDLE_MIN)))
			//	{	
			//		// 파일 크기가 _CANDLE_MIN 의 배수라면
			//		const size_t nCount = heapCtrl.size() / sizeof(_CANDLE_MIN);
			//		DBGPRINT(L"[%s] - %d", __FUNCTIONW__, nCount);
			//		// 메모리에서 읽어온다.
			//		_CANDLE_MIN* pCandleSec = (_CANDLE_MIN*)heapCtrl.get_mem();
			//		// 출력
			//		for (size_t i = 0; i < nCount; ++i)
			//		{
			//			ULONG nDate, nTime;
			//			pCandleSec[i].get_datetime(&nDate, &nTime);

			//			DBGPRINT(L"[%s] %08d - %04d, %d / %d / %d / %d / %d / %I64u"
			//				, _pCreonStockInfo->code.c_str()
			//				, nDate
			//				, nTime
			//				, pCandleSec[i].nOpen
			//				, pCandleSec[i].nHigh
			//				, pCandleSec[i].nLow
			//				, pCandleSec[i].nClose
			//				, pCandleSec[i].nVolume
			//				, pCandleSec[i].nTransferPayment
			//			);
			//		}
			//	}
			//	else
			//	{
			//		DBGPRINT(L"[%s] - %d", __FUNCTIONW__, heapCtrl.size());
			//		// 파일 크기가 _CANDLE_MIN 의 배수가 아니라면
			//		// 에러를 출력한다.
			//		DBGPRINT(L"파일 크기 오류: %s", szFilePath);
			//	}
			//}
		}
	}
#if defined(_DBGPRINT_CMAIN_)
	else
	{
		DBGPRINT(L"[%s] %d분봉 업데이트 보류, %08d, %08d, %d", _pCreonStockInfo->code.c_str(), _분봉의길이들_[_nIndex], nStartDate, nToday, nCandleCount);
	}
#endif
}

void C_MAIN::creon_update_tick(_STOCK_INFO_CREON* _pCreonStockInfo, WORD _nLength)
{
	const std::wstring szFilePath = generate_path(_pCreonStockInfo->code.c_str(), L"tick", _nLength);

	// 시작 날짜 세팅
	ULONG nStartDate = 19700101;
	ULONG nLastestDate = 0;
	if (1 == dk::FileExists(szFilePath))
	{
		// 파일에서 가장 최근 날짜를 얻어온다.
		_TICK_DATA candle_tick;
		get_static_lastdata<_TICK_DATA>(szFilePath, candle_tick);
		ULONG nTime;
		candle_tick.get_datetime(&nLastestDate, &nTime);
#if defined(_DBGPRINT_CMAIN_)
		if (_DEBUG_MODE_)
		{
			DBGPRINT(L"[%s] %08d - %04d, %d / %I64u"
				, _pCreonStockInfo->code.c_str()
				, nLastestDate
				, nTime
				, candle_tick.nClose
				, candle_tick.nVolume
			);
		}
#endif
		// 얻어올 날짜를 얻어온다.
		nStartDate = dk::get_next_day(nLastestDate);
	}

	const dk::_DLOCAL_TIME lt;
	ULONG nToday = (lt.wYear * 10000 + lt.wMonth * 100 + lt.wDay);
	// 날짜의 차이를 구한다.
	ULONG nDiffDay = dk::get_days_between(nToday, nStartDate);
	// 만약 차이가 없다면 오후 8시 5분이 지났는지 확인한다.
	if (!nDiffDay)
	{
		// 우선 날짜가 동일한지 확인한다.
		if (nToday == nStartDate)
		{
			// 우선 오전 5시 55분 이후인지 확인한다.
			if (lt.wHour > 5 || (lt.wHour == 5 && lt.wMinute > 55))
			{
				DBGPRINT(L"[%s] %d틱 업데이트 보류, 사유: 오전 5시 55분 이후", _pCreonStockInfo->code.c_str(), _nLength);
				return;
			}
			// 오후 8시 5분 이전인지 확인한다.
			if (lt.wHour > 20 || (lt.wHour == 20 && lt.wMinute > 5))
			{
				DBGPRINT(L"[%s] %d틱 업데이트 보류, 사유: 오후 8시 5분 이후", _pCreonStockInfo->code.c_str(), _nLength);
				return;
			}
		}
		++nDiffDay;
	}
	if (0 < nDiffDay)
	{
#if defined(_DBGPRINT_CMAIN_)
		DBGPRINT(L"파일 경로: %s", szFilePath.c_str());
#endif
		// 크레온에서 데이터를 얻어온다.
		std::vector<_TICK_DATA> vCandleTicks;
		this->pCreonPlus->get_tick_data(vCandleTicks, _pCreonStockInfo->creon_code.c_str(), nDiffDay);

		if (!vCandleTicks.empty())
		{
			//DBGPRINT(L"%s - %d", __FUNCTIONW__, vCandleMins.size());

			std::vector<_TICK_DATA> filteredCandles;
			filteredCandles.reserve(vCandleTicks.size()); // 메모리 최적화
			std::copy_if(vCandleTicks.begin(), vCandleTicks.end(), std::back_inserter(filteredCandles),
				[&](const _TICK_DATA& candle) {
					ULONG nDate, nTime;
					candle.get_datetime(&nDate, &nTime); // _CANDLE_MIN의 get_datetime 사용
					return nDate > nLastestDate; // nLastestDate보다 이후 날짜만 선택
				});

			vCandleTicks.clear();
			// 이제부터 filteredCandles 벡터를 사용합니다.
			const size_t nMinCount = filteredCandles.size();
			if (0 < nMinCount)
			{
#if defined(_DBGPRINT_CMAIN_)
				if (_DEBUG_MODE_)
				{
					DBGPRINT(L"[%s] - %d 개, 역순으로 저장한다", _pCreonStockInfo->code.c_str(), nMinCount);
				}
#endif
				// filteredCandles 를 역순으로 정렬한다.
				std::reverse(filteredCandles.begin(), filteredCandles.end());
				{
					ULONG nSequence = 0, nCurrentDate = 0;
					for (size_t i = 0; i < nMinCount; ++i)
					{
						ULONG nDate, nTime;
						filteredCandles[i].get_datetime(&nDate, &nTime);
						// 만약 날짜가 바뀌면 시퀀스를 초기화 한다.
						if (nDate != nCurrentDate)
						{
							nCurrentDate = nDate;
							nSequence = 0;
							//DBGPRINT(L"[%s] %08d - %04d, 시퀀스 초기화", _pCreonStockInfo->code.c_str(), nDate, nTime / 10000);
						}
						++nSequence;
						filteredCandles[i].nSequence = nSequence;

						// 만약 매도량 또는 매수량이 0 이라면 0 이 아닌 수량을 가진 쪽으로 nbuySell 를 초기화 한다.
						if (0 == filteredCandles[i].nAccrueVolumeBuy || 0 == filteredCandles[i].nAccrueVolumeSell)
						{
							filteredCandles[i].nbuySell = filteredCandles[i].nAccrueVolumeBuy ? 1 : 2;	// 1: 매수, 2: 매도
						}
						else if (i > 1)
						{
							// 이전의 매수량, 매도량을 비교해서 nbuySell 을 결정한다.
							if (filteredCandles[i].nAccrueVolumeBuy > filteredCandles[i-1].nAccrueVolumeBuy)
							{
								filteredCandles[i].nbuySell = 1;	// 1: 매수
							}
							else if (filteredCandles[i].nAccrueVolumeSell > filteredCandles[i-1].nAccrueVolumeSell)
							{
								filteredCandles[i].nbuySell = 2;	// 2: 매도
							}
						}
#if defined(_DBGPRINT_CMAIN_)
						if (_DEBUG_MODE_)
						{
							DBGPRINT(L"[%s] %08d - %04d, 순: %d, 가격: %d, %s: %d, 대금: %d, 누적매도: %d, 누적매수: %d, 거래량: %d"
								, _pCreonStockInfo->code.c_str()
								, nDate
								, nTime / 10000
								, filteredCandles[i].nSequence
								, filteredCandles[i].nClose
								, (1 == filteredCandles[i].nbuySell) ? L"매수체결" : L"매도체결"
								, filteredCandles[i].nVolume
								, filteredCandles[i].nTransferPayment
								, filteredCandles[i].nAccrueVolumeSell
								, filteredCandles[i].nAccrueVolumeBuy
								, filteredCandles[i].nAccrueVolume
							);
						}
#endif
					}
				}
				// 파일에 저장한다.
				if (!_DEBUG_MODE_)
				{
					save_vector<_TICK_DATA>(szFilePath, filteredCandles);
				}
#if defined(_DBGPRINT_CMAIN_)
				DBGPRINT(L"[%s] %d틱, %d 개 추가 완료", _pCreonStockInfo->code.c_str(), _nLength, nMinCount);
#endif
			}

			// 읽어서 출력해본다.
			// 우선 파일이 있는지 확인하고
			//if (1 == dk::FileExists(szFilePath))
			//{
			//	dk::C_HEAP_CTRL heapCtrl(szFilePath);
			//	if (0 == (heapCtrl.size() % sizeof(_CANDLE_MIN)))
			//	{	
			//		// 파일 크기가 _CANDLE_MIN 의 배수라면
			//		const size_t nCount = heapCtrl.size() / sizeof(_CANDLE_MIN);
			//		DBGPRINT(L"[%s] - %d", __FUNCTIONW__, nCount);
			//		// 메모리에서 읽어온다.
			//		_CANDLE_MIN* pCandleSec = (_CANDLE_MIN*)heapCtrl.get_mem();
			//		// 출력
			//		for (size_t i = 0; i < nCount; ++i)
			//		{
			//			ULONG nDate, nTime;
			//			pCandleSec[i].get_datetime(&nDate, &nTime);

			//			DBGPRINT(L"[%s] %08d - %04d, %d / %d / %d / %d / %d / %I64u"
			//				, pCurrentStockInfo->code.c_str()
			//				, nDate
			//				, nTime
			//				, pCandleSec[i].nOpen
			//				, pCandleSec[i].nHigh
			//				, pCandleSec[i].nLow
			//				, pCandleSec[i].nClose
			//				, pCandleSec[i].nVolume
			//				, pCandleSec[i].nTransferPayment
			//			);
			//		}
			//	}
			//	else
			//	{
			//		DBGPRINT(L"[%s] - %d", __FUNCTIONW__, heapCtrl.size());
			//		// 파일 크기가 _CANDLE_MIN 의 배수가 아니라면
			//		// 에러를 출력한다.
			//		DBGPRINT(L"파일 크기 오류: %s", szFilePath);
			//	}
			//}
		}
	}
#if defined(_DBGPRINT_CMAIN_)
	else
	{
		DBGPRINT(L"[%s] %d틱 패스", _pCreonStockInfo->code.c_str(), _nLength);
	}
#endif
}

void C_MAIN::creon_update(LPCWSTR _pCode)
{
	if (!_pCode || !::wcslen(_pCode))
	{
		//const size_t nDBStockCount = this->vStockDBCodes.size();
		dk::C_TIMER_QP timer;
		timer.Start();
		const size_t nUpdateStockCount = nMaxCount = this->vUpdateStockInfos.size();
		for (size_t i = 0; i < nUpdateStockCount; ++i)
		{
			_STOCK_INFO_CREON* pCurrentStockInfo = this->vUpdateStockInfos[i];
			// 폴더 체크
			check_path(pCurrentStockInfo->code.c_str());
			{	// 업데이트
				this->creon_update_day(pCurrentStockInfo);
				this->creon_update_week(pCurrentStockInfo);
				this->creon_update_month(pCurrentStockInfo);
				// 분봉 업데이트
				for (size_t j = 0; j < _MAX_MIN_CANDLE_COUNT_; ++j)
				{
					this->creon_update_min(pCurrentStockInfo, j);
				}
				//this->creon_update_tick(pCurrentStockInfo);
			}
			{	// 중복체크
				//this->creon_chk_dup(pCurrentStockInfo);
			}
			// 남은 시간을 출력한다 - [%s] %d / %d, 종료 예상 시간: %d시 %d분 %d초, 남은 시간: %d시간 %d분 %d초, 종목코드, 진행중인 종목, 전체 종목 수, 종료 예상 시간, 남은 시간
			// 현재까지 경과 시간 계산
			const double elapsedSeconds = timer.Elapse();

			// 현재 진행률 및 평균 처리 시간 계산
			const double processedCount = static_cast<double>(i + 1);
			const double avgTimePerStock = elapsedSeconds / processedCount;

			// 남은 종목 수와 예상 소요 시간 계산
			const size_t remainingStocks = nUpdateStockCount - (i + 1);
			const double remainingSeconds = avgTimePerStock * remainingStocks;

			// 남은 시간을 시, 분, 초로 변환
			remainingHours = static_cast<WORD>(remainingSeconds) / 3600;
			remainingMinutes = (static_cast<WORD>(remainingSeconds) % 3600) / 60;
			remainingSeconds_int = static_cast<WORD>(remainingSeconds) % 60;

			// 현재 시간 가져오기
			const dk::_DLOCAL_TIME lt;

			// 현재 시간의 FILETIME을 가져옴
			dk::_DFILETIME fileTime(&lt);
			fileTime += static_cast<ULONGLONG>(remainingSeconds * 10000000);	// 남은 시간 추가
			// 예상 종료 시간
			estimatedEndTime = fileTime.get_system_time();
			nCurrentIndex = i + 1;	// 현재 진행 중인 인덱스
			
			if (this->nShutdownScheduled.is_set()) { break; }	// 만약 종료 플래그가 있다면 중단한다.
#if defined(_DBGPRINT_CMAIN_)
			// 진행 상황, 예상 종료 시간, 남은 시간 출력
			DBGPRINT(L"[%s] %d / %d, 종료 예상 시간: %02d시 %02d분 %02d초, 남은 시간: %d시간 %d분 %d초",
				pCurrentStockInfo->code.c_str(),
				i + 1,
				nUpdateStockCount,
				estimatedEndTime.wHour,
				estimatedEndTime.wMinute,
				estimatedEndTime.wSecond,
				remainingHours,
				remainingMinutes,
				remainingSeconds_int
			);
#endif
		}
	}
	else
	{
		_STOCK_INFO_CREON* pCurrentStockInfo = this->get_creon_stock_info(_pCode);
		if (pCurrentStockInfo)
		{
			check_path(pCurrentStockInfo->code.c_str());
			this->creon_update_day(pCurrentStockInfo);
			this->creon_update_week(pCurrentStockInfo);
			this->creon_update_month(pCurrentStockInfo);
			for (size_t j = 0; j < _MAX_MIN_CANDLE_COUNT_; ++j)
			{
				this->creon_update_min(pCurrentStockInfo, j);
			}
			//this->creon_update_tick(pCurrentStockInfo);
			//this->creon_chk_dup(pCurrentStockInfo);
			
		}
		else
		{
			DBGPRINT(L"[%s] %s - 크레온 정보 없음", __FUNCTIONW__, _pCode);
		}
	}
}

void C_MAIN::creon_update_WM(LPCWSTR _pCode)
{
	if (!_pCode || !::wcslen(_pCode))
	{
		//const size_t nDBStockCount = this->vStockDBCodes.size();
		dk::C_TIMER_QP timer;
		timer.Start();
		const size_t nUpdateStockCount = nMaxCount = this->vUpdateStockInfos.size();
		for (size_t i = 0; i < nUpdateStockCount; ++i)
		{
			_STOCK_INFO_CREON* pCurrentStockInfo = this->vUpdateStockInfos[i];
			this->creon_update_week(pCurrentStockInfo);
			this->creon_update_month(pCurrentStockInfo);

			// 남은 시간을 출력한다 - [%s] %d / %d, 종료 예상 시간: %d시 %d분 %d초, 남은 시간: %d시간 %d분 %d초, 종목코드, 진행중인 종목, 전체 종목 수, 종료 예상 시간, 남은 시간
			// 현재까지 경과 시간 계산
			const double elapsedSeconds = timer.Elapse();

			// 현재 진행률 및 평균 처리 시간 계산
			const double processedCount = static_cast<double>(i + 1);
			const double avgTimePerStock = elapsedSeconds / processedCount;

			// 남은 종목 수와 예상 소요 시간 계산
			const size_t remainingStocks = nUpdateStockCount - (i + 1);
			const double remainingSeconds = avgTimePerStock * remainingStocks;

			// 남은 시간을 시, 분, 초로 변환
			remainingHours = static_cast<WORD>(remainingSeconds) / 3600;
			remainingMinutes = (static_cast<WORD>(remainingSeconds) % 3600) / 60;
			remainingSeconds_int = static_cast<WORD>(remainingSeconds) % 60;

			// 현재 시간 가져오기
			const dk::_DLOCAL_TIME lt;

			// 현재 시간의 FILETIME을 가져옴
			dk::_DFILETIME fileTime(&lt);
			fileTime += static_cast<ULONGLONG>(remainingSeconds * 10000000);	// 남은 시간 추가
			// 예상 종료 시간
			estimatedEndTime = fileTime.get_system_time();
			nCurrentIndex = i + 1;	// 현재 진행 중인 인덱스
#if defined(_DBGPRINT_CMAIN_)
			// 진행 상황, 예상 종료 시간, 남은 시간 출력
			DBGPRINT(L"[%s] %d / %d, 종료 예상 시간: %02d시 %02d분 %02d초, 남은 시간: %d시간 %d분 %d초",
				pCurrentStockInfo->code.c_str(),
				i + 1,
				nUpdateStockCount,
				estimatedEndTime.wHour,
				estimatedEndTime.wMinute,
				estimatedEndTime.wSecond,
				remainingHours,
				remainingMinutes,
				remainingSeconds_int
			);
#endif
		}
	}
	else
	{
		_STOCK_INFO_CREON* pCurrentStockInfo = this->get_creon_stock_info(_pCode);
		if (pCurrentStockInfo)
		{
			this->creon_update_week(pCurrentStockInfo);
			this->creon_update_month(pCurrentStockInfo);
		}
		else
		{
			DBGPRINT(L"[%s] %s - 크레온 정보 없음", __FUNCTIONW__, _pCode);
		}
	}
}

void C_MAIN::trans_min(LPCWSTR _pCode, size_t _nIndex)
{
	try
	{
		//const size_t nCount = _countof(ori_filename);
		_STOCK_INFO_CREON* pCurrentStockInfo = this->get_creon_stock_info(_pCode);
		if (nullptr == pCurrentStockInfo)
		{
			DBGPRINT(L"[%s] %s - 크레온 정보 없음", __FUNCTIONW__, _pCode);
			return;
		}
		const std::wstring szFilePath = generate_path(pCurrentStockInfo->code.c_str(), L"min", _분봉의길이들_[_nIndex]);
		// 파일이 이미 존재하면 패스
		if (1 == dk::FileExists(szFilePath))
		{
			DBGPRINT(L"변환된 파일이 존재합니다: %s", szFilePath.c_str());
			return;
		}
		// csv 경로 
		wchar_t szCsvPath[MAX_PATH] = { 0 };
		::swprintf_s(szCsvPath, _countof(szCsvPath), L"%s\\%s\\%s_%s.csv", szDefaultPath, pCurrentStockInfo->code.c_str(), pCurrentStockInfo->code.c_str()
			, ori_filename[_nIndex]
		);
		DBGPRINT(L"%s -> %s", szCsvPath, szFilePath.c_str());
		// 모든 내용을 변환해야함.
		// 파일이 있는지 확인하고
		std::vector<_CANDLE_MIN> vCandleSec;
		dk::C_HEAP_CTRL heapCtrl;
		if (1 == dk::FileExists(szCsvPath))
		{
			char szLine[1024] = { 0, };
			heapCtrl.load_file(szCsvPath);
			const LONGLONG nFileSize = heapCtrl.heap_size();
			if (0 < nFileSize)
			{
				dk::C_HEAP_CTRL heapTemp(static_cast<size_t>(nFileSize));

				//DBGPRINT(L"경로: %s, 파일 크기: %d", szCsvPath, nFileSize);
				// 우선 널바이트가 있다면 제거한다.
				LPBYTE pMemory = heapCtrl.get_mem();
				//LPBYTE pTempMemory = heapTemp.get_mem();
				for (size_t readPos = 0; readPos < nFileSize; ++readPos)
				{
					if (pMemory[readPos] != 0x00)  // NULL이 아닌 경우에만 복사
					{
						heapTemp.write_byte(pMemory[readPos]);
					}
				}
				heapCtrl.close();

				//std::wstring strNewFilePath = szCsvPath;
				//strNewFilePath += L".bak";
				//heapTemp.save_file(strNewFilePath);

				heapCtrl.alloc(heapTemp.used_size());
				heapCtrl.append_heap(heapTemp.get_mem(), heapTemp.used_size());
				//DBGPRINT(L"파일 크기: %d -> %d", heapTemp.used_size(), heapCtrl.size());

				heapCtrl.set_end_pos();	// 파일 끝으로 이동
				do
				{
					ZeroMemory(szLine, sizeof(szLine));
					// 역순으로 한줄씩 읽어들인다.
					LONGLONG nLineLength = heapCtrl.get_rline(szLine, _countof(szLine));	// 
					if (0 < nLineLength)
					{
						_CANDLE_MIN candle{};

						LPSTR _pBuf = szLine;
						//DBGPRINT(L"%d, %s", nLineLength, szLine);
						//OutputDebugStringA(szLine);

						LPSTR pContext = nullptr;
						LPSTR pToken = ::strtok_s(_pBuf, ",", &pContext);
						if (!pToken) { throw; }
						const ULONG nDate = ::atoi(pToken);
						pToken = ::strtok_s(nullptr, ",", &pContext);
						if (!pToken) { throw; }
						const ULONG nTime = ::atoi(pToken);
						if (0 < nTime)
						{
							candle.set_ts(nDate, nTime);
						}
						pToken = ::strtok_s(nullptr, ",", &pContext);
						if (!pToken) { throw; }
						candle.nOpen = ::atoi(pToken);
						pToken = ::strtok_s(nullptr, ",", &pContext);
						if (!pToken) { throw; }
						candle.nHigh = ::atoi(pToken);
						pToken = ::strtok_s(nullptr, ",", &pContext);
						if (!pToken) { throw; }
						candle.nLow = ::atoi(pToken);
						pToken = ::strtok_s(nullptr, ",", &pContext);
						if (!pToken) { throw; }
						candle.nClose = ::atoi(pToken);
						pToken = ::strtok_s(nullptr, ",", &pContext);
						if (!pToken) { throw; }
						candle.nVolume = ::atoi(pToken);

						vCandleSec.push_back(candle);
					}
					else
					{
						break;
					}
				} while (true);
				heapCtrl.close();
			}
		}
		else
		{
			DBGPRINT(L"파일 없음: %s", szCsvPath);
		}

		//if (HasDuplicateTimestampsNoModify(vCandleSec))
		//{
		//	DBGPRINT(L"중복된 타임스탬프가 있습니다, %s", szFilePath);
		//	::ExitProcess(0);
		//}
		DBGPRINT(L"중복체크: %d", vCandleSec.size());
		std::vector<_CANDLE_MIN> uniqueCandles = RemoveDuplicateTimestamps(vCandleSec);
		DBGPRINT(L"중복제거: %d", uniqueCandles.size());
		// 중복 제거한 uniqueCandles 를 bak 에 저장한다.
		//std::wstring strNewFilePath = szFilePath;
		//strNewFilePath += L".csv";
		
		std::vector<std::string> vCandleSecBak;
		for (const auto& candle : uniqueCandles)
		{
			char szLine[1024] = { 0, };
			ULONG nDate, nTime;
			candle.get_datetime(&nDate, &nTime);
			::sprintf(szLine, "%08d,%04d,%d,%d,%d,%d,%d\r\n"
				, nDate
				, nTime / 100000
				, candle.nOpen
				, candle.nHigh
				, candle.nLow
				, candle.nClose
				, candle.nVolume
			);
			vCandleSecBak.push_back(szLine);
		}
		{
			dk::C_HEAP_CTRL heapTemp;
			heapTemp.alloc(static_cast<size_t>(vCandleSecBak.size() * 1024));
			//heapTemp.append_heap(vCandleSecBak.data(), vCandleSecBak.size());
			for (const auto& line : vCandleSecBak)
			{
				heapTemp.append_heap(line.c_str(), line.size());
			}
			heapTemp.save_file(szCsvPath);
		}
		// vCandleSec 를 역순으로 정렬한다.
		//std::reverse(uniqueCandles.begin(), uniqueCandles.end());

		// 저장
		DBGPRINT(L"파일 저장: %s", szFilePath.c_str());
		append_file<_CANDLE_MIN>(uniqueCandles.data(), uniqueCandles.size(), szFilePath);
		{
			// 다시 파일을 읽어서 출력해본다.
			//heapCtrl.load_file(szNewFilePath);
			//if (0 == (heapCtrl.size() % sizeof(_CANDLE_MIN)))
			//{
			//	// 파일 크기가 _CANDLE_MIN 의 배수라면
			//	const size_t nCount = heapCtrl.size() / sizeof(_CANDLE_MIN);
			//	DBGPRINT(L"[%s] - %d", __FUNCTIONW__, nCount);
			//	// 메모리에서 읽어온다.
			//	_CANDLE_MIN* pCandleSec = (_CANDLE_MIN*)heapCtrl.get_mem();
			//	// 출력
			//	for (size_t i = 0; i < nCount; ++i)
			//	{
			//		ULONG nDate, nTime;
			//		pCandleSec[i].get_datetime(nDate, nTime);
			//		DBGPRINT(L"[%s] %08d %04d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %d"
			//			, pCurrentStockInfo->code.c_str()
			//			, nDate
			//			, nTime
			//			, pCandleSec[i].nOpen
			//			, pCandleSec[i].nHigh
			//			, pCandleSec[i].nLow
			//			, pCandleSec[i].nClose
			//			, pCandleSec[i].nVolume
			//		);
			//	}
			//}
			//else
			//{
			//	DBGPRINT(L"파일 크기 오류: %s", szNewFilePath);
			//}
			//// 출력
			//const size_t nCount = vCandleSec.size();
			//DBGPRINT(L"[%s] - %d", pCurrentStockInfo->code.c_str(), nCount);
			//for (size_t i = 0; i < nCount; ++i)
			//{
			//	ULONG nDate, nTime;
			//	vCandleSec[i].get_datetime(nDate, nTime);
			//	DBGPRINT(L"[%s] %08d %04d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %d"
			//		, pCurrentStockInfo->code.c_str()
			//		, nDate
			//		, nTime
			//		, vCandleSec[i].nOpen
			//		, vCandleSec[i].nHigh
			//		, vCandleSec[i].nLow
			//		, vCandleSec[i].nClose
			//		, vCandleSec[i].nVolume
			//	);
			//}
		}
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, __LINE__);
	}
}

void C_MAIN::trans_mins(LPCWSTR _pCode)
{
	// 폴더 내의 폴더명들로 종목 코드를 수집한다.
	std::vector<std::wstring> vFolderNames;
	dk::GetFolderListDirectoryW(vFolderNames, szDefaultPath);

	// 출력
	const size_t nCount = vFolderNames.size();
	DBGPRINT(L"폴더 개수: %d", nCount);

	if (!_pCode)
	{
		const size_t nUpdateStockCount = vUpdateStockInfos.size();
		for (size_t i = 0; i < nUpdateStockCount; ++i)
		{
			for (size_t j = 0; j < _MAX_MIN_CANDLE_COUNT_; ++j)
			{
				this->trans_min(this->vUpdateStockInfos[i]->code.c_str(), j);
			}
		}
	}
	else
	{
		this->trans_min(_pCode, 0);
	}
}

void C_MAIN::creon_chk_dup(_STOCK_INFO_CREON* _pCreonStockInfo)
{
	int nCurrentLine = __LINE__;
	try
	{
		// 일봉 체크
		std::wstring szFilePath = generate_path(_pCreonStockInfo->code.c_str(), L"day");
		check_duplicate_dwm<_CANDLE_DAY>(szFilePath);
		// 주봉 체크
		szFilePath = generate_path(_pCreonStockInfo->code.c_str(), L"week");
		check_duplicate_dwm<_CANDLE_WEEK>(szFilePath);
		// 월봉 체크
		szFilePath = generate_path(_pCreonStockInfo->code.c_str(), L"month");
		check_duplicate_dwm<_CANDLE_MONTH>(szFilePath);
		// 분봉 체크
		for (size_t i = 0; i < _MAX_MIN_CANDLE_COUNT_; ++i)
		{
			szFilePath = generate_path(_pCreonStockInfo->code.c_str(), L"min", _분봉의길이들_[i]);
			check_duplicate_min<_CANDLE_MIN>(szFilePath);
		}
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
}

//void C_MAIN::push_update_code(LPCWSTR _pCode)
//{
//	// 문자열을 한 번만 생성하여 큐에 추가
//	if (_pCode && *_pCode)
//	{
//		// 문자열 직접 생성 및 이동 의미론 활용
//		this->qUpdateCode.enqueue(std::wstring(_pCode));
//	}
//	else
//	{
//		// 빈 문자열 처리
//		static const std::wstring emptyStr;
//		this->qUpdateCode.enqueue(emptyStr);
//	}
//}
//
//DWORD C_MAIN::ThreadFunc(LPVOID _p)
//{
//	DBGPRINT(L"업데이트 스레드 시작");
//	_p; // 미사용 매개변수
//	// 예외를 저장할 큐 생성
//	moodycamel::ConcurrentQueue<std::wstring> exceptionMessages;
//	std::wstring strCode;
//	do
//	{
//		strCode.clear();
//		if (this->qUpdateCode.wait_dequeue_timed(strCode, std::chrono::milliseconds(100)))
//		{
//			if (!strCode.empty())
//			{
//				DBGPRINT(L"[%s] 업데이트 요청 ", strCode.c_str());
//				// 예외 처리 추가
//				try
//				{
//					// 크레온 업데이트
//					this->creon_update(strCode);
//				}
//				catch (const std::exception& e)
//				{
//					// 예외 정보를 큐에 저장하고 계속 진행
//					std::wstring errorMsg = L"종목 코드 [" + strCode + L"] 처리 중 예외 발생: " + std::wstring(e.what(), e.what() + strlen(e.what()));
//					exceptionMessages.enqueue(errorMsg);
//
//					//DBGPRINT(L"[%s] 예외: %S", __FUNCTIONW__, e.what());
//				}
//				catch (...)
//				{
//					// 알 수 없는 예외 처리
//					std::wstring errorMsg = L"종목 코드 [" + strCode + L"] 처리 중 알 수 없는 예외 발생";
//					exceptionMessages.enqueue(errorMsg);
//					//DBGPRINT(L"[%s] 알 수 없는 예외 발생", __FUNCTIONW__);
//				}
//			}
//			else
//			{
//				DBGPRINT(L"전체 종목 업데이트 요청");
//				this->creon_update();
//			}
//		}
//	} while (!this->bStopThread);
//
//	// 수집된 모든 예외 기록
//	std::wstring exMsg;
//	while (exceptionMessages.try_dequeue(exMsg))
//	{
//		DBGPRINT(L"[%s] 저장된 예외: %s", __FUNCTIONW__, exMsg.c_str());
//	}
//
//	DBGPRINT(L"[%s] %s", __FUNCTIONW__, L"종료");
//	return(0);
//}