#include "framework.h"
#include "CMain.h"



LRESULT C_MAIN::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	static WORD nPrevSecond = 0;
	static dk::_DLOCAL_TIME lt;
	static wchar_t szTitle[128] = { 0 };	// 제목 표시줄

	static C_CLOSED_DAYS closedDays;
	static ULONG nToday{ 0 };	// 오늘 날짜
	
	//DBGPRINT(L"[%s] %d / %d, 종료 예상 시간: %02d시 %02d분 %02d초, 남은 시간: %d시간 %d분 %d초",
	//	pCurrentStockInfo->code.c_str(),
	//	i + 1,
	//	nUpdateStockCount,
	//	estimatedEndTime.wHour,
	//	estimatedEndTime.wMinute,
	//	estimatedEndTime.wSecond,
	//	remainingHours,
	//	remainingMinutes,
	//	remainingSeconds_int
	//);
	static size_t nPrevIndex = 0;
	try
	{
		switch (wParam)
		{
		case _TID_MS_100_:
			lt.refresh();
			//if (!this->IsIconic())	// 최소화 상태가 아니라면
			//{
			//	::swprintf(szTitle, L"크레온 - %04d-%02d-%02d %02d:%02d:%02d.%03d"
			//		, lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds
			//	);
			//	this->SetWindowTextW(szTitle);
			//}
			if (pCreonPlus && pCreonPlus->is_connected())
			{
				const long nRemainRequestCount = pCreonPlus->get_limit_remain_count();
				const long nRemainRequestTime = pCreonPlus->get_limit_request_remain_time();
				if (0 < nRemainRequestCount || 15000 == nRemainRequestTime)
				{
					pCreonPlus->signal_request_possible();
				}
				else if (nPrevIndex != nCurrentIndex)
				{
					::swprintf(szTitle, L"%d/%d - %02d시 %02d분 %02d초까지 남은 시간: %d시간 %d분 %d초"
						, nCurrentIndex
						, nMaxCount
						, estimatedEndTime.wHour
						, estimatedEndTime.wMinute
						, estimatedEndTime.wSecond
						, remainingHours
						, remainingMinutes
						, remainingSeconds_int
					);
					this->SetWindowTextW(szTitle);
					nPrevIndex = nCurrentIndex;
				}
			}
			{
				//ULONG nCurrentDate = _YMD_TO_ULONG_(lt.wYear, lt.wMonth, lt.wDay);
				//ULONG nCurrentTime = _HMS_TO_SEC_(lt.wHour, lt.wMinute, lt.wSecond);	// 현재 시간을 초로 변환한다.
				if (nPrevSecond != lt.wSecond)	// 초가 변경되었을 때만 실행된다.
				{	// 여기는 1초에 한번씩 실행된다
					if (this->nShutdownScheduled.is_set() && !this->nWorking.is_set())
					{	// 종료 예약이 되어있고, 작업중이 아니라면
						this->CloseDialog(0);
					}
					if (this->IsIconic())
					{	// 최소화 상태라면
						::swprintf(szTitle, L"크레온 - %04d-%02d-%02d %02d:%02d:%02d.%03d"
							, lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds
						);
						// 트레이의 툴팁을 갱신한다.
						::_NOTIFYICONDATAW nid = { sizeof(::_NOTIFYICONDATAW), this->m_hWnd, 0 };
						nid.uFlags = NIF_TIP;
						::wcscpy(nid.szTip, szTitle);
						::Shell_NotifyIconW(NIM_MODIFY, &nid);
						//DBGPRINT(L"툴팁 갱신: %s", szTitle);
					}
					nPrevSecond = lt.wSecond;
				}
			}
			break;
		default:
			DBGPRINT(L"알수없는 타이머 ID: %d", wParam);
			break;
		}
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, __LINE__);
	}
	return 0;
}
