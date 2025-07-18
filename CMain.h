/*
 * MIT License
 * Copyright (c) 2025 dwerasd
 *
 * 사용된 라이브러리:
 * - ConcurrentQueue (Cameron Desrochers)
 *   라이센스 선택: Boost Software License 1.0
 *   원본 파일: readerwriterqueue.h
 *   원본 파일: blockingconcurrentqueue.h
*/
#pragma once


#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlctrls.h>
#include <atlcrack.h>
#include <atlddx.h>
#include <atlctrlw.h>
#include <atlscrl.h>
#include <atluser.h>
#include <atlgdi.h>

#include <DarkCore/DLock.h>
#include <DarkCore/DTimer.h>
#include <DarkCore/DMutex.h>
#include <DarkCore/DThread.h>
#include <DarkCore/DSingleton.h>
#include <DarkCore/DSharedMemory.h>
#include <DarkCore/DExecBat.h>

#include <DOpenSources/readerwriterqueue.h>
#include <DOpenSources/blockingconcurrentqueue.h>

#include <DataHelper/functions.h>
#include <DataHelper/CMariaDB.h>
#pragma comment(lib, "DataHelper")

#include "resource.h"

#include "CCreonPlus.h"



#if !defined(WM_TRAYICON)
#define WM_TRAYICON     (WM_USER + 100)
#endif

constexpr UINT_PTR _TID_MS_10_ = 10;
constexpr UINT_PTR _TID_MS_100_ = 100;
constexpr UINT_PTR _TID_MS_200_ = 200;
constexpr UINT_PTR _TID_MS_500_ = 500;
constexpr UINT_PTR _TID_MS_1000_ = 1000;

class C_MAIN
	: public dk::C_SINGLETON<C_MAIN>
	, public ATL::CDialogImpl<C_MAIN>
	//, public dk::C_THREAD
{
private:
	bool bShowWindow{ true };

	dk::C_EVENT_MANUAL_RESET m_ShutdownEvent;	// 요청 이벤트

	dk::C_INTERLOCK_FLAG nWorking;	            // 작업중 여부
	dk::C_INTERLOCK_FLAG nShutdownScheduled;	// 종료 예약 여부

	HINSTANCE hInst{ nullptr };

	dk::DPOINT ptMainWindow;
	dk::DSIZE sizeMainWindow;

	typedef std::unordered_map<UINT_PTR, UINT> UMAP_TIMER_IDS;
	UMAP_TIMER_IDS umapTimerIDs;

	void save_rect();
	void move_rect();

	void create_tray_icon(HINSTANCE _hInst);
	void delete_tray_icon();

	WORD remainingHours{ 0 };
	WORD remainingMinutes{ 0 };
	WORD remainingSeconds_int{ 0 };
	_SYSTEMTIME estimatedEndTime{};

	size_t nCurrentIndex{ 0 }, nMaxCount{ 0 };

	C_CREON_PLUS* pCreonPlus{ nullptr };
	C_MARIADB* pMariaDB{ nullptr };

	// 크레온 종목 정보
	std::vector<_STOCK_INFO_CREON> vStockCreonInfo;
	std::unordered_map<std::wstring, _STOCK_INFO_CREON*> umapCreonStockInfo;
	std::vector<_STOCK_INFO_CREON*> vUpdateStockInfos;
	// 사용할 종목 코드 목록
	std::vector<std::wstring> vStockDBCodes;

	//moodycamel::BlockingConcurrentQueue<std::wstring> qUpdateCode;
	//DWORD ThreadFunc(LPVOID);

protected:
	WTL::CAppModule _Module;
	WTL::CMessageLoop theLoop;

	LRESULT OnTrayMenu(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/);
	LRESULT OnTrayClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

public:
	enum { IDD = IDD_MAIN_DIALOG };
	C_MAIN();
	virtual ~C_MAIN();

	bool Init(HINSTANCE _h);
	int Display();
	void Destroy() noexcept;

	void CloseDialog(int nVal);

	void WTLInit(HINSTANCE _hInst)
	{
		// WTL 초기화
		this->_Module.Init(NULL, _hInst);
		// 메시지 루프에 추가
		this->_Module.AddMessageLoop(&theLoop);
		// 메인 다이얼로그 생성
		HWND Desktop = ::GetDesktopWindow();
		HWND hWnd = this->Create(Desktop);
		if (!hWnd)
		{
			DBGPRINT(L"Main dialog creation failed - %s", dk::GetLastErrorMessageW());
		}
		DBGPRINT(L"메인 다이얼로그 생성 완료");
		// 메인 다이얼로그 표시
		this->ShowWindow(SW_SHOWDEFAULT);
	}
	int WTLRun() { return theLoop.Run(); }

	BEGIN_MSG_MAP(C_MAIN)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)

		MESSAGE_HANDLER_EX(WM_TRAYICON, OnTrayMenu)
		COMMAND_ID_HANDLER(ID_TRAY_CLOSE, OnTrayClose)

		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)

		COMMAND_HANDLER(IDC_BTN_UPDATE, BN_CLICKED, OnBnClickedBtnUpdate)
		COMMAND_HANDLER(IDC_BTN_WMUP, BN_CLICKED, OnBnClickedBtnWMUpdate)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSysCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnBnClickedBtnUpdate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBtnWMUpdate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	_STOCK_INFO_CREON* get_creon_stock_info(LPCWSTR _pCode);
	inline _STOCK_INFO_CREON* get_creon_stock_info(std::wstring_view _pCode) { return this->get_creon_stock_info(_pCode.data()); }

	void creon_update_day(_STOCK_INFO_CREON* _pCreonStockInfo, bool _bAppend = true);
	inline void creon_update_day(std::wstring_view _pCode) { this->creon_update_day(_pCode.data()); }

	void creon_update_week(_STOCK_INFO_CREON* _pCreonStockInfo);
	inline void creon_update_week(std::wstring_view _pCode) { this->creon_update_week(_pCode.data()); }

	void creon_update_month(_STOCK_INFO_CREON* _pCreonStockInfo);
	inline void creon_update_month(std::wstring_view _pCode) { this->creon_update_month(_pCode.data()); }

	void creon_update_min(_STOCK_INFO_CREON* _pCreonStockInfo = nullptr, size_t _nIndex = 0, bool _bAppend = true);
	inline void creon_update_min(std::wstring_view _pCode, size_t _nIndex, bool _bAppend = true) { this->creon_update_min(_pCode.data(), _nIndex, _bAppend); }

	void creon_update_tick(_STOCK_INFO_CREON* _pCreonStockInfo = nullptr, WORD _nLength = 1);
	
	void creon_update(LPCWSTR _pCode = nullptr);
	inline void creon_update(std::wstring_view _pCode) { this->creon_update(_pCode.data()); }
	void creon_update_WM(LPCWSTR _pCode = nullptr);
	inline void creon_update_tick(std::wstring_view _pCode, WORD _nLength) { this->creon_update_tick(_pCode.data(), _nLength); }

	void trans_min(LPCWSTR _pCode, size_t _nIndex);
	void trans_mins(LPCWSTR _pCode = nullptr);
	
	void creon_chk_dup(_STOCK_INFO_CREON* _pCreonStockInfo);

	void update();

	//void push_update_code(LPCWSTR _pCode);
	//inline void push_update_code(std::wstring_view _pCode) { this->push_update_code(_pCode.data()); }
};
