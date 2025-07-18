#include "framework.h"
#include "CCreonPlus.h"
//#include "CMain.h"


C_CREON_PLUS::C_CREON_PLUS()
{

}

C_CREON_PLUS::C_CREON_PLUS(LPCWSTR _pCreonID, LPCWSTR _pCreonPW, LPCWSTR _pCertPW)
{
	this->init(_pCreonID, _pCreonPW, _pCertPW);
}

C_CREON_PLUS::C_CREON_PLUS(std::wstring_view _pCreonID, std::wstring_view _pCreonPW, std::wstring_view _pCertPW)
{
	this->init(_pCreonID.data(), _pCreonPW.data(), _pCertPW.data());
}

C_CREON_PLUS::~C_CREON_PLUS()
{
	int nCurrentLine = __LINE__;
	try
	{
		this->destroy();
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
}

void C_CREON_PLUS::init(LPCWSTR _pCreonID, LPCWSTR _pCreonPW, LPCWSTR _pCertPW)
{
	int nCurrentLine = __LINE__;
	try
	{
		this->크레온계정 = _pCreonID;
		this->크레온비밀번호 = _pCreonPW;
		this->인증서비밀번호 = _pCertPW;

		this->umapCreonObjects.clear();
		// 연결, 요청제한, 서버 종류 등
		this->umapCreonObjects[L"CpCybos"] = { __uuidof(CPUTILLib::CpCybos), __uuidof(CPUTILLib::ICpCybos) };
		// 종목명, 풀코드 등
		this->umapCreonObjects[L"CpStockCode"] = { __uuidof(CPUTILLib::CpStockCode), __uuidof(CPUTILLib::ICpStockCode) };
		// 소속부, 감리구분, 상태 등
		this->umapCreonObjects[L"CpCodeMgr"] = { __uuidof(CPUTILLib::CpCodeMgr), __uuidof(CPUTILLib::ICpCodeMgr) };
		// [주식 시간대별체결] Dscbo1.StockBid (최대 80개)
		this->umapCreonObjects[L"StockBid"] = { __uuidof(DSCBO1Lib::StockBid), __uuidof(DSCBO1Lib::IDib) };
		// 주식, 지수, 선물 / 옵션등의여러종목의필요항목들을한번에수신합니다.
		this->umapCreonObjects[L"MarketEye"] = { __uuidof(CPSYSDIBLib::MarketEye), __uuidof(CPSYSDIBLib::ISysDib) };
		// [주식, 업종, ELW 차트] CpSysDib.StockChart
		this->umapCreonObjects[L"StockChart"] = { __uuidof(CPSYSDIBLib::StockChart), __uuidof(CPSYSDIBLib::ISysDib) };
		// [종목별 투자자] CpSysDib.InvestorsbyStock
		this->umapCreonObjects[L"InvestorsbyStock"] = { __uuidof(CPSYSDIBLib::InvestorsbyStock), __uuidof(CPSYSDIBLib::ISysDib) };
		// [종목별 프로그램매매] CpSysDib.CpSvr8113
		this->umapCreonObjects[L"CpSvr8113"] = { __uuidof(CPSYSDIBLib::CpSvr8113), __uuidof(CPSYSDIBLib::ISysDib) };
		// [해외 지수, 환율,원자재 차트] CpSysDib.CpSvrNew8300
		this->umapCreonObjects[L"CpSvrNew8300"] = { __uuidof(CPSYSDIBLib::CpSvrNew8300), __uuidof(CPSYSDIBLib::ISysDib) };
		// [주식 예상체결] DsCbo1.StockExpectCur
		this->umapCreonObjects[L"StockExpectCur"] = { __uuidof(DSCBO1Lib::StockExpectCur), __uuidof(DSCBO1Lib::IDib) };
		// [업종 일자별] DsCbo1.UpjongDaily
		this->umapCreonObjects[L"UpjongDaily"] = { __uuidof(DSCBO1Lib::UpjongDaily), __uuidof(DSCBO1Lib::IDib) };
		// [거래량/거래대금 상위종목] CpSysDib.CpSvr7049
		this->umapCreonObjects[L"CpSvr7049"] = { __uuidof(CPSYSDIBLib::CpSvr7049), __uuidof(CPSYSDIBLib::ISysDib) };
		// [주식선물 6주간] CpSysDib.FutStockWeek
		this->umapCreonObjects[L"FutStockWeek"] = { __uuidof(CPSYSDIBLib::FutStockWeek), __uuidof(CPSYSDIBLib::ISysDib) };
		// [주식선물 종목 실시간 현재가] CpSysDib.FutStockCurS
		this->umapCreonObjects[L"FutStockCurS"] = { __uuidof(CPSYSDIBLib::FutStockCurS), __uuidof(CPSYSDIBLib::ISysDib) };
		// [주식선물 종목 현재가] CpSysDib.FutStockMst
		this->umapCreonObjects[L"FutStockMst"] = { __uuidof(CPSYSDIBLib::FutStockMst), __uuidof(CPSYSDIBLib::ISysDib) };
		// [업종별 투자자 매매현황 실시간] CpSysDib.CpSvrNew7221S
		this->umapCreonObjects[L"CpSvrNew7221S"] = { __uuidof(CPSYSDIBLib::CpSvrNew7221S), __uuidof(CPSYSDIBLib::ISysDib) };
		// [파생상품 예상체결가] DsCbo1.CpSvr9027
		this->umapCreonObjects[L"CpSvr9027"] = { __uuidof(DSCBO1Lib::CpSvr9027), __uuidof(DSCBO1Lib::IDib) };
		// [체결기준 주식 당일매매손익] cptrade.CpTd6032
		this->umapCreonObjects[L"CpTd6032"] = { __uuidof(CPTRADELib::CpTd6032), __uuidof(CPTRADELib::ICpTdDib) };
		// [종목별 프로그램매매 추이 리스트(일자별)] DsCbo1.CpSvrNew8119Day
		this->umapCreonObjects[L"CpSvrNew8119Day"] = { __uuidof(DSCBO1Lib::CpSvrNew8119Day), __uuidof(DSCBO1Lib::IDib) };
		// [종목별 프로그램매매 추이 (실시간)] CpSysDib.CpSvr8119S
		this->umapCreonObjects[L"CpSvr8119S"] = { __uuidof(CPSYSDIBLib::CpSvr8119S), __uuidof(CPSYSDIBLib::ISysDib) };
		// [종목별 프로그램매매 추이 차트(시간대별)] DsCbo1.CpSvrNew8119Chart
		this->umapCreonObjects[L"CpSvrNew8119Chart"] = { __uuidof(DSCBO1Lib::CpSvrNew8119Chart), __uuidof(DSCBO1Lib::IDib) };
		// [종목별 프로그램매매 추이 리스트(시간대별)] DsCbo1.CpSvrNew8119
		this->umapCreonObjects[L"CpSvrNew8119"] = { __uuidof(DSCBO1Lib::CpSvrNew8119), __uuidof(DSCBO1Lib::IDib) };
		// [종목 시간별 잠정데이터] CpSysDib.CpSvr7210T
		this->umapCreonObjects[L"CpSvr7210T"] = { __uuidof(CPSYSDIBLib::CpSvr7210T), __uuidof(CPSYSDIBLib::ISysDib) };
		// [종목별 투자자 매매동향(잠정)데이터] CpSysDib.CpSvr7210d
		this->umapCreonObjects[L"CpSvr7210d"] = { __uuidof(CPSYSDIBLib::CpSvr7210d), __uuidof(CPSYSDIBLib::ISysDib) };
		// [ETF(NAV), ETN(IIV) 시간대별 실시간 데이터] CpSysDib.CpSvrNew7244S
		this->umapCreonObjects[L"CpSvrNew7244S"] = { __uuidof(CPSYSDIBLib::CpSvrNew7244S), __uuidof(CPSYSDIBLib::ISysDib) };
		// [회원사별 종목 매매동향] Dscbo1.CpSvr8412
		this->umapCreonObjects[L"CpSvr8412"] = { __uuidof(DSCBO1Lib::CpSvr8412), __uuidof(DSCBO1Lib::IDib) };
		// [시장조치사항] CpSysDib.CpSvr9619S
		this->umapCreonObjects[L"CpSvr9619S"] = { __uuidof(CPSYSDIBLib::CpSvr9619S), __uuidof(CPSYSDIBLib::ISysDib) };
		// [매매입체분석] CpSysDib.CpSvr7254
		this->umapCreonObjects[L"CpSvr7254"] = { __uuidof(CPSYSDIBLib::CpSvr7254), __uuidof(CPSYSDIBLib::ISysDib) };
		// [종목검색 실시간 신호] CpSysDib.CssAlert
		this->umapCreonObjects[L"CssAlert"] = { __uuidof(CPSYSDIBLib::CssAlert), __uuidof(CPSYSDIBLib::ISysDib) };
		// [종목검색 전략 감시 시작/중지] CpSysDib.CssWatchStgControl
		this->umapCreonObjects[L"CssWatchStgControl"] = { __uuidof(CPSYSDIBLib::CssWatchStgControl), __uuidof(CPSYSDIBLib::ISysDib) };
		// [종목검색 전략 감시 일련번호] CpSysDib.CssWatchStgSubscribe
		this->umapCreonObjects[L"CssWatchStgSubscribe"] = { __uuidof(CPSYSDIBLib::CssWatchStgSubscribe), __uuidof(CPSYSDIBLib::ISysDib) };
		// [종목검색 검색 종목 리스트] CpSysDib.CssStgFind
		this->umapCreonObjects[L"CssStgFind"] = { __uuidof(CPSYSDIBLib::CssStgFind), __uuidof(CPSYSDIBLib::ISysDib) };
		// [종목검색 전략명, 전략ID 리스트] CpSysDib.CssStgList
		this->umapCreonObjects[L"CssStgList"] = { __uuidof(CPSYSDIBLib::CssStgList), __uuidof(CPSYSDIBLib::ISysDib) };
		// [ETN(IIV) 일자별] Dscbo1.CpSvr7719
		this->umapCreonObjects[L"CpSvr7719"] = { __uuidof(DSCBO1Lib::CpSvr7719), __uuidof(DSCBO1Lib::IDib) };
		// [ETN(IIV) 시간대별] Dscbo1.CpSvr7718
		this->umapCreonObjects[L"CpSvr7718"] = { __uuidof(DSCBO1Lib::CpSvr7718), __uuidof(DSCBO1Lib::IDib) };
		// [ETF(NAV) 일자별] Dscbo1.CpSvr7246
		this->umapCreonObjects[L"CpSvr7246"] = { __uuidof(DSCBO1Lib::CpSvr7246), __uuidof(DSCBO1Lib::IDib) };

		// 거래 관련
		this->umapCreonObjects[L"CpTdUtil"] = { __uuidof(CPTRADELib::CpTdUtil), __uuidof(CPTRADELib::ICpTdUtil) };

		this->set_va_tick();
		this->set_va_sec();
		this->set_va_day();
		this->set_va_week();
		this->set_va_month();
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
}

void C_CREON_PLUS::init(std::wstring_view _pCreonID, std::wstring_view _pCreonPW, std::wstring_view _pCertPW)
{
	this->init(_pCreonID.data(), _pCreonPW.data(), _pCertPW.data());
}

bool C_CREON_PLUS::create()
{
	this->create_creon_plus();
	return this->connect_creon_plus();
}

void C_CREON_PLUS::destroy()
{
	int nCurrentLine = __LINE__;
	try
	{
		if (this->is_connected())
		{
			// umapCreonObjects 를 모두 해제한다.
			for (auto& [key, value] : this->umapCreonObjects)
			{
				if (value.pObject != nullptr)
				{
					// LPVOID를 IUnknown*로 캐스팅하여 Release 호출
					IUnknown* pUnknown = static_cast<IUnknown*>(value.pObject);
					pUnknown->Release();
					value.pObject = nullptr;
					//DBGPRINT(L"%s - %s 해제 완료", __FUNCTIONW__, key.c_str());
				}
			}
		}
		else
		{
			DBGPRINT(L"%s - 크레온플러스가 연결되어 있지 않음", __FUNCTIONW__);
		}
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
}

void C_CREON_PLUS::test_creon()
{
	int nCurrentLine = __LINE__;
	try
	{
		//this->umap_tf_params.reserve(6);
		//this->umap_tf_params.emplace('m', L"m");
		//this->umap_tf_params.emplace('T', L"t");
		//this->umap_tf_params.emplace('S', L"s");
		//this->umap_tf_params.emplace('D', L"day");
		//this->umap_tf_params.emplace('W', L"week");
		//this->umap_tf_params.emplace('M', L"month");
		//nCurrentLine = __LINE__;
		// 필터 이름 설정
		//pMain->vFilterName.reserve(6);
		//pMain->vFilterName.emplace_back(L"우B");
		//pMain->vFilterName.emplace_back(L"3우C");
		//pMain->vFilterName.emplace_back(L"(전환)");
		//pMain->vFilterName.emplace_back(L"G3우");
		//pMain->vFilterName.emplace_back(L"1우");
		//pMain->vFilterName.emplace_back(L"홀딩스");
		//
		//pMain->nCountStockItems = this->get_stock_infos(pMain->vStockItems);	// connect_creon_plus 를 해서 사용할 수 있는 상태다.
		//nCurrentLine = __LINE__;
		//// 편의를 위해 unorder_map 에 포인터 링크
		//DBGPRINT(L"%s - 맵 초기화: %d", __FUNCTIONW__, pMain->nCountStockItems);
		//pMain->umapStockItems.clear();
		//pMain->umapStockItems.reserve(pMain->nCountStockItems);
		//nCurrentLine = __LINE__;
		//DBGPRINT(L"%s - 종목들 맵에 연동", __FUNCTIONW__);
		//for (auto& stock : pMain->vStockItems)
		//{
		//	pMain->umapStockItems.emplace(stock.szCreonCode, &stock);
		//}
		//nCurrentLine = __LINE__;
		//DBGPRINT(L"%s - 현물 초기화", __FUNCTIONW__);
		//pMain->vRealItems.clear();
		//pMain->vRealItems.reserve(pMain->nCountStockItems);
		//nCurrentLine = __LINE__;
		//DBGPRINT(L"%s - 필터 초기화", __FUNCTIONW__);
		//pMain->vValidItems.clear();
		//pMain->vValidItems.reserve(pMain->nCountStockItems);
		//nCurrentLine = __LINE__;
		//DBGPRINT(L"%s - 업데이트 초기화", __FUNCTIONW__);
		//pMain->vUpdateCodes.clear();
		//pMain->vUpdateCodes.reserve(pMain->nCountStockItems);
		//nCurrentLine = __LINE__;
		//DBGPRINT(L"%s - 코스피200 가져오기", __FUNCTIONW__);
		//this->get_kospi200_list(pMain->vKospi200s);
		//nCurrentLine = __LINE__;
		//DBGPRINT(L"%s - 코스닥150 가져오기", __FUNCTIONW__);
		//this->get_kosdaq150_list(pMain->vKosdaq150s);
		//nCurrentLine = __LINE__;
		//// 코스피200, 코스닥150 에 포함된 종목은 표시
		//DBGPRINT(L"%s - 코스피200 표시", __FUNCTIONW__);
		//for (auto& stock : pMain->vKospi200s)
		//{
		//	UMAP_STOCKS::iterator itr = pMain->umapStockItems.find(stock);
		//	if (itr != pMain->umapStockItems.end())
		//	{
		//		itr->second->nIsTopIncluded = true;
		//	}
		//	//else { { DBGPRINT(L"%s - umapStockItems 에 코스피200 이 없음 = %s"), stock.c_str()); } }
		//}
		//nCurrentLine = __LINE__;
		//DBGPRINT(L"%s - 코스닥150 표시", __FUNCTIONW__);
		//for (auto& stock : pMain->vKosdaq150s)
		//{
		//	UMAP_STOCKS::iterator itr = pMain->umapStockItems.find(stock);
		//	if (itr != pMain->umapStockItems.end())
		//	{
		//		itr->second->nIsTopIncluded = true;
		//	}
		//	//else { { DBGPRINT(L"%s - umapStockItems 에 코스닥150 이 없음 = %s"), stock.c_str()); } }
		//}
		//nCurrentLine = __LINE__;
		//DBGPRINT(L"%s - 종목 필터링", __FUNCTIONW__);
		//for (auto& itr : pMain->vStockItems)
		//{	// 여기에서 거를 종목 거르고 vValidItems 를 채운다.
		//	_STOCK_INFO_CREON* pCurrentStock = &itr;
		//	if ((CPUTILLib::CPC_MARKET_KOSPI == pCurrentStock->nMarketKind || CPUTILLib::CPC_MARKET_KOSDAQ == pCurrentStock->nMarketKind))
		//	{
		//		if (CPUTILLib::CPC_KSE_SECTION_KIND_ELW == pCurrentStock->nKseSectionKind)
		//		{
		//			pMain->vELWItems.emplace_back(pCurrentStock);
		//			continue;
		//		}
		//		if (CPUTILLib::CPC_KSE_SECTION_KIND_ETF == pCurrentStock->nKseSectionKind)
		//		{
		//			pMain->vETFItems.emplace_back(pCurrentStock);
		//			continue;
		//		}
		//		//if (CPUTILLib::CPC_KSE_SECTION_KIND_ETN == pCurrentStock->nKseSectionKind)
		//		//{
		//		//	vETNItems.emplace_back(pCurrentStock);
		//		//	continue;
		//		//}
		//		if (CPUTILLib::CPC_KSE_SECTION_KIND_SR < pCurrentStock->nKseSectionKind)
		//		{
		//			continue;
		//		}
		//		//DBGPRINT(L"%s"), pCurrentStock->code.c_str());
		//		if (0 == pCurrentStock->nStatus					// 거래중지나 거래중단을 거르고
		//			&& 0 == pCurrentStock->nSupervisionType		// 관리종목 거르고
		//			&& 0 == pCurrentStock->nControlType			// 정상 외 거르고
		//			&& 0 == pCurrentStock->nSpac				// 스팩 거르고
		//			&& 0 < pCurrentStock->nPreClose				// nPreClose 없는건 아예 없는 STOCK_INFO.
		//			//&& 1 == pCurrentStock->bIsBigListingStock	// 삼성전자가 유일해서 뺀다.
		//			)	// 거래할 종목만 메모리에 적재한다.
		//		{
		//			// 종목코드의 끝에 K 가 붙으면 제외
		//			//if (pCurrentStock->szCode[5] != '0')		// 우선주 제외
		//			//{
		//			//	continue;
		//			//}
		//			//if (pMain->filter_name(pCurrentStock->szName))
		//			//{
		//			//	continue;
		//			//}

		//			// 종목코드가 전부 숫자가 아니면 제외
		//			//bool bAllNumber = true;
		//			//for (size_t i = 0; i < _countof(pCurrentStock->szCode); i++)
		//			//{
		//			//	if (pCurrentStock->szCode[i] < '0' && pCurrentStock->szCode[i] > '9')	// 만약 숫자가 아니면
		//			//	{
		//			//		DBGPRINT(L"숫자아님 제외: %s / %s"), pCurrentStock->szCode, pCurrentStock->szName);
		//			//		bAllNumber = false;
		//			//		break;
		//			//	}
		//			//}
		//			//if (bAllNumber)
		//			{
		//				pMain->vValidItems.emplace_back(pCurrentStock);
		//				pMain->vUpdateCodes.emplace_back(pCurrentStock->szCreonCode);
		//			}
		//		}
		//		pMain->vRealItems.emplace_back(pCurrentStock);
		//	}
		//}
		//nCurrentLine = __LINE__;
		////if (bDbgPrint)
		//{
		//	DBGPRINT(L"%s - 종목 수: %d", __FUNCTIONW__, pMain->vStockItems.size());
		//	DBGPRINT(L"%s - ELW: %d", __FUNCTIONW__, pMain->vELWItems.size());
		//	DBGPRINT(L"%s - ETF: %d", __FUNCTIONW__, pMain->vETFItems.size());
		//	//DBGPRINT(L"%s - ETN: %d", __FUNCTIONW__, pMain->vETNItems.size());
		//	DBGPRINT(L"%s - 현물: %d", __FUNCTIONW__, pMain->vRealItems.size());
		//	DBGPRINT(L"%s - 1차 필터: %d", __FUNCTIONW__, pMain->vValidItems.size());
		//}
		// etf 출력
		//for (auto& stock : pMain->vETFItems)
		//{
		//	DBGPRINT(L"%s - ETF: %s / %s", __FUNCTIONW__, stock->szCode, stock->szName);
		//}
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
}

bool C_CREON_PLUS::create_creon_plus()
{
	bool bResult = false;
	int nCurrentLine = __LINE__;
	try
	{
		do
		{
			DWORD dwCreonProcessID = dk::GetProcessOf(L"CpStart.exe");
			nCurrentLine = __LINE__;
			DWORD dwStarterProcessID = dk::GetProcessOf(L"coStarter.exe");
			nCurrentLine = __LINE__;
			if (!dwCreonProcessID && !dwStarterProcessID)	// 크레온플러스를 실행해야하는거다.
			{
				wchar_t szParam[_MAX_PATH]{};
				::swprintf(szParam, L" /prj:cp /id:%s /pwd:%s /pwdcert:%s /autostart"
					, this->크레온계정.c_str()
					, this->크레온비밀번호.c_str()
					, this->인증서비밀번호.c_str()
				);
				if (1 == dk::FileExists(L"C:/CREON/STARTER/coStarter.exe"))
				{
					nCurrentLine = __LINE__;
					bool bExecuted = dk::ExecuteFile(L"C:/CREON/STARTER/coStarter.exe", szParam, true, nullptr);
					if (!bExecuted)
					{
						DBGPRINT(L"C_CREON_PLUS::connect_creon_plus() - 크레온플러스 실행에 실패함");
						DBGPRINT(L"C_CREON_PLUS::InitCreonAPI() - 크레온플러스 실행에 실패함");
						break;
					}
				}
				else
				{
					nCurrentLine = __LINE__;
					DBGPRINT(L"C_CREON_PLUS::connect_creon_plus() - 파일이 없음: C:/CREON/STARTER/coStarter.exe");
					DBGPRINT(L"C_CREON_PLUS::InitCreonAPI() - 파일이 없음: C:/CREON/STARTER/coStarter.exe");
					break;
				}
				DWORD dwTimeCount = 0, dwTimeBuffer = 0;
				do
				{	// 스타터 종료 대기.
					dk::_DLOCAL_TIME currentTime;
					if (currentTime.wSecond != dwTimeBuffer)
					{	// 
						++dwTimeCount;
						dwTimeBuffer = currentTime.wSecond;
					}
					if (300 < dwTimeCount)
					{	// 
						DBGPRINT(L"C_CREON_PLUS::InitCreonAPI() - 5분동안 실행이 안됨");
						DBGPRINT(L"C_CREON_PLUS::InitCreonAPI() - 크레온플러스를 종료합니다.");
						dk::TerminateProcess(L"CpStart.exe");
						dk::TerminateProcess(L"DibServer.exe");
					}
					nCurrentLine = __LINE__;
					dwStarterProcessID = dk::GetProcessOf(L"coStarter.exe");
					if (!dwStarterProcessID)
					{
						nCurrentLine = __LINE__;
						dwCreonProcessID = dk::GetProcessOf(L"CpStart.exe");
						if (dwCreonProcessID) { break; }
					}
					dk::sleep(200);
				} while (true);
				nCurrentLine = __LINE__;
				dwTimeCount = dwTimeBuffer = 0;
				do
				{	// 크레온플러스가 실행된거다.
					dk::_DLOCAL_TIME currentTime;
					if (currentTime.wSecond != dwTimeBuffer)
					{	// 
						++dwTimeCount;
						dwTimeBuffer = currentTime.wSecond;
					}
					if (300 < dwTimeCount)
					{	//
						DBGPRINT(L"C_CREON_PLUS::InitCreonAPI() - 5분동안 실행이 안됨");
						DBGPRINT(L"C_CREON_PLUS::InitCreonAPI() - 크레온플러스를 종료합니다.");
						dk::TerminateProcess(L"CpStart.exe");
						dk::TerminateProcess(L"DibServer.exe");
					}
					nCurrentLine = __LINE__;
					HWND hFound = ::FindWindowW(L"Afx:00400000:0", L"공지사항");
					if (hFound)
					{	// 공지사항을 닫고 빠져나감.
						nCurrentLine = __LINE__;
						::PostMessageW(hFound, WM_CLOSE, 0, 0);
						break;
					}
					nCurrentLine = __LINE__;
					dk::sleep(200);
				} while (true);
			}
		} while (false);
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
	return(bResult);
}

bool C_CREON_PLUS::create_creon_object(_OBJECT_INFO& _pInfo)
{
	if (SUCCEEDED(::CoCreateInstance(_pInfo.clsid, 0, CLSCTX_ALL, _pInfo.iid, &_pInfo.pObject)))
	{
		return(true);
	}
	return(false);
}

bool C_CREON_PLUS::connect_creon_plus()
{
	bool bResult = false;
	int nCurrentLine = __LINE__;
	try
	{
		do
		{
			size_t nCompleteCount = 0;
			for (auto& [key, value] : this->umapCreonObjects)
			{
				if (!this->create_creon_object(value))
				{
					DBGPRINT(L"%s - %s 생성 실패", __FUNCTIONW__, key.c_str());
					break;
				}
				DBGPRINT(L"%s - %s 생성 완료", __FUNCTIONW__, key.c_str());
				++nCompleteCount;
			}
			if (nCompleteCount == this->umapCreonObjects.size())
			{
				DBGPRINT(L"%s - %d 개 생성 완료", __FUNCTIONW__, this->umapCreonObjects.size());
			}
			else
			{
				DBGPRINT(L"%s - %d / %d 개 생성 실패", __FUNCTIONW__, nCompleteCount, this->umapCreonObjects.size());
				break;
			}
			// 자주 쓰는 객체는 포인터로 저장한다.
			this->pCybos = static_cast<CPUTILLib::ICpCybos*>(this->get_creon_object(L"CpCybos"));
			this->pStockCode = static_cast<CPUTILLib::ICpStockCode*>(this->get_creon_object(L"CpStockCode"));
			this->pCodeMgr = static_cast<CPUTILLib::ICpCodeMgr*>(this->get_creon_object(L"CpCodeMgr"));
			this->pStockBid = static_cast<DSCBO1Lib::IDib*>(this->get_creon_object(L"StockBid"));
			this->pMarketEye = static_cast<CPSYSDIBLib::ISysDib*>(this->get_creon_object(L"MarketEye"));
			this->pStockChart = static_cast<CPSYSDIBLib::ISysDib*>(this->get_creon_object(L"StockChart"));
			this->pTrade = static_cast<CPTRADELib::ICpTdUtil*>(this->get_creon_object(L"CpTdUtil"));

			if (!this->is_connected())
			{
				DBGPRINT(L"C_CREON_PLUS::InitCreonAPI() - 크레온플러스를 종료합니다.");
				dk::TerminateProcess(L"CpStart.exe");
				dk::TerminateProcess(L"DibServer.exe");
				DBGPRINT(L"C_CREON_PLUS::InitCreonAPI() - 크레온 중계기를 재실행합니다.");
				nCurrentLine = __LINE__;
				wchar_t 실행파일[_MAX_PATH] = { 0 }, szParam[_MAX_PATH] = { L"restart" };
				::GetModuleFileNameW(::GetModuleHandleW(0), 실행파일, _countof(실행파일));
				dk::ExecuteFile(실행파일, szParam, true);
				nCurrentLine = __LINE__;
				break;
			}
			bResult = true;
			DBGPRINT(L"%s - 크레온플러스 초기화 완료.", __FUNCTIONW__);
		} while (false);
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
	return(bResult);
}

long C_CREON_PLUS::get_market_start_time()
{
	if (this->pCodeMgr) { return this->pCodeMgr->GetMarketStartTime(); }
	DBGPRINT(L"%s - pCodeMgr is nullptr", __FUNCTIONW__);
	return(0);
}

long C_CREON_PLUS::get_market_end_time()
{
	if (this->pCodeMgr) { return this->pCodeMgr->GetMarketEndTime(); }
	DBGPRINT(L"%s - pCodeMgr is nullptr", __FUNCTIONW__);
	return(0);
}

long C_CREON_PLUS::get_limit_remain_count(CPUTILLib::LIMIT_TYPE _nLimit_Type)
{
	if (this->pCybos) { return this->pCybos->GetLimitRemainCount(_nLimit_Type); }
	DBGPRINT(L"%s - pCybos is nullptr", __FUNCTIONW__);
	return(0);
}

long C_CREON_PLUS::get_limit_request_remain_time()
{
	if (this->pCybos) { return this->pCybos->GetLimitRequestRemainTime(); }
	DBGPRINT(L"%s - pCybos is nullptr", __FUNCTIONW__);
	return(0);
}

bool C_CREON_PLUS::is_connected()
{
	if (this->pCybos) { return this->pCybos->GetIsConnect(); }
	DBGPRINT(L"%s - pCybos is nullptr", __FUNCTIONW__);
	return(false);
}

short C_CREON_PLUS::get_stock_count()
{
	if (this->pStockCode) { return this->pStockCode->GetCount(); }
	DBGPRINT(L"%s - pStockCode is nullptr", __FUNCTIONW__);
	return(0);
}

_variant_t C_CREON_PLUS::get_data(short type, short index)
{
	if (this->pStockCode) { return this->pStockCode->GetData(type, index); }
	DBGPRINT(L"%s - pStockCode is nullptr", __FUNCTIONW__);
	return(_variant_t());
}

_variant_t C_CREON_PLUS::get_data_value(int __MIDL__ISysDib0001, int __MIDL__ISysDib0002)
{
	if (this->pStockChart) { return this->pStockChart->GetDataValue(__MIDL__ISysDib0001, __MIDL__ISysDib0002); }
	DBGPRINT(L"%s - pStockChart is nullptr", __FUNCTIONW__);
	return(_variant_t());
}

long C_CREON_PLUS::get_continue()
{
	if (this->pStockChart) { return this->pStockChart->GetContinue(); }
	DBGPRINT(L"%s - pStockChart is nullptr", __FUNCTIONW__);
	return(0);
}

HRESULT C_CREON_PLUS::set_input_value(int __MIDL__ISysDib0004, const _variant_t& newVal)
{
	if (this->pStockChart) { return this->pStockChart->SetInputValue(__MIDL__ISysDib0004, newVal); }
	DBGPRINT(L"%s - pStockChart is nullptr", __FUNCTIONW__);
	return(E_FAIL);
}

_variant_t C_CREON_PLUS::get_header_value(int __MIDL__ISysDib0000)
{
	if (this->pStockChart) { return this->pStockChart->GetHeaderValue(__MIDL__ISysDib0000); }
	DBGPRINT(L"%s - pStockChart is nullptr", __FUNCTIONW__);
	return(_variant_t());
}

short C_CREON_PLUS::block_request()
{
	if (this->pStockChart) { return this->pStockChart->BlockRequest(); }
	DBGPRINT(L"%s - pStockChart is nullptr", __FUNCTIONW__);
	return(0);
}

short C_CREON_PLUS::block_request2(short BlockOption)
{
	if (this->pStockChart) { return this->pStockChart->BlockRequest2(BlockOption); }
	DBGPRINT(L"%s - pStockChart is nullptr", __FUNCTIONW__);
	return(0);
}

long C_CREON_PLUS::get_stock_market_kind(LPCWSTR _pCode)
{
	if (this->pCodeMgr) { return this->pCodeMgr->GetStockMarketKind(_pCode); }
	DBGPRINT(L"%s - pCodeMgr is nullptr", __FUNCTIONW__);
	return(0);
}

long C_CREON_PLUS::get_stock_section_kind(LPCWSTR _pCode)
{
	if (this->pCodeMgr) { return this->pCodeMgr->GetStockSectionKind(_pCode); }
	DBGPRINT(L"%s - pCodeMgr is nullptr", __FUNCTIONW__);
	return(0);
}

long C_CREON_PLUS::get_stock_status_kind(LPCWSTR _pCode)
{
	if (this->pCodeMgr) { return this->pCodeMgr->GetStockStatusKind(_pCode); }
	DBGPRINT(L"%s - pCodeMgr is nullptr", __FUNCTIONW__);
	return(0);
}

long C_CREON_PLUS::get_stock_supervision_kind(LPCWSTR _pCode)
{
	if (this->pCodeMgr) { return this->pCodeMgr->GetStockSupervisionKind(_pCode); }
	DBGPRINT(L"%s - pCodeMgr is nullptr", __FUNCTIONW__);
	return(0);
}

long C_CREON_PLUS::get_stock_control_kind(LPCWSTR _pCode)
{
	if (this->pCodeMgr) { return this->pCodeMgr->GetStockControlKind(_pCode); }
	DBGPRINT(L"%s - pCodeMgr is nullptr", __FUNCTIONW__);
	return(0);
}

long C_CREON_PLUS::is_spac(LPCWSTR _pCode)
{
	if (this->pCodeMgr) { return this->pCodeMgr->IsSPAC(_pCode); }
	DBGPRINT(L"%s - pCodeMgr is nullptr", __FUNCTIONW__);
	return(0);
}

long C_CREON_PLUS::get_stock_yd_close_price(LPCWSTR _pCode)
{
	if (this->pCodeMgr) { return this->pCodeMgr->GetStockYdClosePrice(_pCode); }
	DBGPRINT(L"%s - pCodeMgr is nullptr", __FUNCTIONW__);
	return(0);
}

long C_CREON_PLUS::is_big_listing_stock(LPCWSTR _pCode)
{
	if (this->pCodeMgr) { return this->pCodeMgr->IsBigListingStock(_pCode); }
	DBGPRINT(L"%s - pCodeMgr is nullptr", __FUNCTIONW__);
	return(0);
}

short C_CREON_PLUS::get_stock_infos(std::vector<_STOCK_INFO_CREON>& _v)
{
	const short nStockCount = this->get_stock_count();
	int nCurrentLine = __LINE__;
	try
	{
		//typedef enum
		//{
		//  [helpstring("구분없음")] CPC_KSE_SECTION_KIND_NULL = 0,
		//  [helpstring("주권")]   CPC_KSE_SECTION_KIND_ST = 1,
		//  [helpstring("투자회사")]   CPC_KSE_SECTION_KIND_MF = 2,
		//  [helpstring("부동산투자회사"]   CPC_KSE_SECTION_KIND_RT = 3,
		//  [helpstring("선박투자회사")]   CPC_KSE_SECTION_KIND_SC = 4,
		//  [helpstring("사회간접자본투융자회사")]CPC_KSE_SECTION_KIND_IF = 5,
		//  [helpstring("주식예탁증서")]   CPC_KSE_SECTION_KIND_DR = 6,
		//  [helpstring("신수인수권증권")]   CPC_KSE_SECTION_KIND_SW = 7,
		//  [helpstring("신주인수권증서")]   CPC_KSE_SECTION_KIND_SR = 8,
		//  [helpstring("주식워런트증권")]   CPC_KSE_SECTION_KIND_ELW = 9,
		//  [helpstring("상장지수펀드(ETF)")]CPC_KSE_SECTION_KIND_ETF = 10,
		//  [helpstring("수익증권")]    CPC_KSE_SECTION_KIND_BC = 11,
		//  [helpstring("해외ETF")]      CPC_KSE_SECTION_KIND_FETF = 12,
		//  [helpstring("외국주권")]    CPC_KSE_SECTION_KIND_FOREIGN = 13,
		//  [helpstring("선물")]      CPC_KSE_SECTION_KIND_FU = 14,
		//  [helpstring("옵션")]      CPC_KSE_SECTION_KIND_OP = 15,
		//  [helpstring("KONEX")]      CPC_KSE_SECTION_KIND_KN = 16,
		//  [helpstring("ETN")]      CPC_KSE_SECTION_KIND_ETN = 17,
		//} CPE_KSE_SECTION_KIND;
		// 부구분코드 (CPE_KSE_SECTION_KIND)
		std::vector<std::wstring> vKseSection;
		vKseSection.reserve(18);
		vKseSection.emplace_back(L"구분없음");
		vKseSection.emplace_back(L"주권");
		vKseSection.emplace_back(L"투자회사");
		vKseSection.emplace_back(L"부동산투자회사");
		vKseSection.emplace_back(L"선박투자회사");
		vKseSection.emplace_back(L"사회간접자본투융자회사");
		vKseSection.emplace_back(L"주식예탁증서");
		vKseSection.emplace_back(L"신수인수권증권");
		vKseSection.emplace_back(L"신주인수권증서");
		vKseSection.emplace_back(L"주식워런트증권(ELW)");
		vKseSection.emplace_back(L"상장지수펀드(ETF)");
		vKseSection.emplace_back(L"수익증권");
		vKseSection.emplace_back(L"해외ETF");
		vKseSection.emplace_back(L"외국주권");
		vKseSection.emplace_back(L"선물");
		vKseSection.emplace_back(L"옵션");
		vKseSection.emplace_back(L"KONEX");
		vKseSection.emplace_back(L"ETN");
		nCurrentLine = __LINE__;
		//typedef enum
		//{
		//	[helpstring("구분없음")] CPC_MARKET_NULL = 0,
		//	[helpstring("거래소")]   CPC_MARKET_KOSPI = 1,
		//	[helpstring("코스닥")]   CPC_MARKET_KOSDAQ = 2,
		//	[helpstring("K-OTC")] CPC_MARKET_FREEBOARD = 3,
		//	[helpstring("KRX")]       CPC_MARKET_KRX = 4,
		//	[helpstring("KONEX")] CPC_MARKET_KONEX = 5,
		//} CPE_MARKET_KIND;
		// 소속부 (CPE_MARKET_KIND)
		std::vector<std::wstring> vMarketType;
		vMarketType.reserve(6);
		vMarketType.emplace_back(L"구분없음");
		vMarketType.emplace_back(L"거래소");
		vMarketType.emplace_back(L"코스닥");
		vMarketType.emplace_back(L"K-OTC");
		vMarketType.emplace_back(L"KRX");
		vMarketType.emplace_back(L"KONEX");
		nCurrentLine = __LINE__;
		//typedef enum
		//{
		//	[helpstring("정상")] CPC_CONTROL_NONE = 0,
		//	[helpstring("주의")]   CPC_CONTROL_ATTENTION = 1,
		//	[helpstring("경고")]   CPC_CONTROL_WARNING = 2,
		//	[helpstring("위험예고")]CPC_CONTROL_DANGER_NOTICE = 3,
		//	[helpstring("위험")]   CPC_CONTROL_DANGER = 4,
		//	[helpstring("경고예고")]   CPC_CONTROL_WARNING_NOTICE = 5,
		//}CPE_CONTROL_KIND;
		// 감리구분 (CPE_CONTROL_KIND)
		std::vector<std::wstring> vControlType;
		vControlType.reserve(6);
		vControlType.push_back(L"정상");
		vControlType.push_back(L"주의");
		vControlType.push_back(L"경고");
		vControlType.push_back(L"위험예고");
		vControlType.push_back(L"위험");
		vControlType.push_back(L"경고예고");
		nCurrentLine = __LINE__;
		//typedef enum
		//{
		//	[helpstring("일반종목")] CPC_SUPERVISION_NONE = 0,
		//	[helpstring("관리")]   CPC_SUPERVISION_NORMAL = 1,
		//}CPE_SUPERVISION_KIND;
		// 관리구분 (CPE_SUPERVISION_KIND)
		std::vector<std::wstring> vSupervisionType;
		vSupervisionType.reserve(2);
		vSupervisionType.push_back(L"일반종목");
		vSupervisionType.push_back(L"관리");
		nCurrentLine = __LINE__;
		//typedef enum
		//{
		//	[helpstring("정상")]    CPC_STOCK_STATUS_NORMAL = 0,
		//	[helpstring("거래정지")] CPC_STOCK_STATUS_STOP = 1,
		//	[helpstring("거래중단")] CPC_STOCK_STATUS_BREAK = 2,
		//}CPE_STOCK_STATUS_KIND;
		// 정지구분 (CPE_STOCK_STATUS_KIND)
		std::vector<std::wstring> vStockStatus;
		vStockStatus.reserve(3);
		vStockStatus.push_back(L"정상");
		vStockStatus.push_back(L"거래정지");
		vStockStatus.push_back(L"거래중단");
		nCurrentLine = __LINE__;
		// 모든 종목 총 갯수

		DBGPRINT(L"GetIsConnect(): %s / 종목 총 갯수 = %d", this->is_connected() ? L"true" : L"false", nStockCount);
		nCurrentLine = __LINE__;
		// vector 크기를 초기화 한다.
		_v.clear();
		_v.reserve(nStockCount);	// resize 는 용량을 할당하고 실제로 데이터를 넣는다. vector 의 크기를 변경한다.
		// reserve 는 용량을 할당하고 실제로 데이터를 넣지 않는다. vector 의 크기를 변경하지 않는다.
		// 컴파일 후 속도는 reserve 가 더 빠르다.
		nCurrentLine = __LINE__;
		_variant_t varCode, varName, varFullCode;
		for (short i = 0; i < nStockCount; ++i)
		{
			nCurrentLine = __LINE__;
			// 복사가 일어나지 않도록 emplace_back 를 사용한다.
			_v.emplace_back();
			nCurrentLine = __LINE__;

			_STOCK_INFO_CREON* pCurrentStock = &_v[i];

			//pCurrentStock->nSequence = i+1;				// 순번
			//pCurrentStock->nMaxCount = nStockCount;	// 최대 순번
			// 메모리를 Detach 하기 위해 _variant_t 를 사용한다.
			varCode = this->get_data(0, i);	// 코드
			varName = this->get_data(1, i);	// 이름
			varFullCode = this->get_data(2, i);	// 풀코드

			pCurrentStock->creon_code = varCode.bstrVal;					// 크레온 코드

			pCurrentStock->stock_name = varName.bstrVal;						// szName

			pCurrentStock->full_code = varFullCode.bstrVal;				// 풀코드
			// 코드는 크레온 코드가 A005930 이라면 005930 이다.
			pCurrentStock->code = varCode.bstrVal + 1;					// szCode

			//CPUTILLib::CPE_MARKET_KIND marketType = pCodeMgr->GetStockMarketKind(varCode.bstrVal);							// 거래소, 코스닥 등의 종류
			pCurrentStock->nMarketKind = this->pCodeMgr->GetStockMarketKind(varCode.bstrVal);											// 거래소, 코스닥 등의 종류
			::wcscpy_s(pCurrentStock->szMarketKind, vMarketType[pCurrentStock->nMarketKind].c_str());							// 거래소, 코스닥 등의 종류
			//CPUTILLib::CPE_KSE_SECTION_KIND kseSectionType = pCodeMgr->GetStockSectionKind(varCode.bstrVal);					// ETF, ELW 등의 종류
			pCurrentStock->nKseSectionKind = this->pCodeMgr->GetStockSectionKind(varCode.bstrVal);									// ETF, ELW 등의 종류
			::wcscpy_s(pCurrentStock->szKseSectionKind, vKseSection[pCurrentStock->nKseSectionKind].c_str());					// ETF, ELW 등의 종류
			//CPUTILLib::CPE_STOCK_STATUS_KIND stockStatus = pCodeMgr->GetStockStatusKind(varCode.bstrVal);						// 거래 정지 구분
			pCurrentStock->nStatus = this->pCodeMgr->GetStockStatusKind(varCode.bstrVal);												// 거래 정지 구분
			::wcscpy_s(pCurrentStock->szStatus, vStockStatus[pCurrentStock->nStatus].c_str());									// [0]: 정상, [1]: 거래정지, [2]: 거래중단
			//CPUTILLib::CPE_SUPERVISION_KIND supervisionType = pCodeMgr->GetStockSupervisionKind(varCode.bstrVal);				// 관리 종목 구분
			pCurrentStock->nSupervisionType = this->pCodeMgr->GetStockSupervisionKind(varCode.bstrVal);								// 관리 종목 구분
			::wcscpy_s(pCurrentStock->szSupervisionType, vSupervisionType[pCurrentStock->nSupervisionType].c_str());			// 관리 종목 구분
			//CPUTILLib::CPE_CONTROL_KIND controlType = pCodeMgr->GetStockControlKind(varCode.bstrVal);							// 위험, 경고 등의 종류
			pCurrentStock->nControlType = this->pCodeMgr->GetStockControlKind(varCode.bstrVal);										// 위험, 경고 등의 종류
			::wcscpy_s(pCurrentStock->szControlType, vControlType[pCurrentStock->nControlType].c_str());						// 위험, 경고 등의 종류
			//long nSpac = pCodeMgr->IsSPAC(varCode.bstrVal);																	// nSpac 구분
			pCurrentStock->nSpac = this->pCodeMgr->IsSPAC(varCode.bstrVal);															// nSpac 구분
			//long nPreClose = pCodeMgr->GetStockYdClosePrice(varCode.bstrVal);													// nPreClose
			pCurrentStock->nPreClose = this->pCodeMgr->GetStockYdClosePrice(varCode.bstrVal);											// nPreClose
			pCurrentStock->bIsBigListingStock = this->pCodeMgr->IsBigListingStock(varCode.bstrVal);									// 대량 증거금 구분 (상장주식수 20억 이상 여부 종목 확인)
			//DBGPRINT(L"[%s] %s, %s - %s, %s, %s, %s, %d, %d, %s, %s, %d")
			//	, pCurrentStock->szCreonCode
			//	, pCurrentStock->szCode
			//	, pCurrentStock->szFullCode
			//	, pCurrentStock->szMarketKind
			//	, pCurrentStock->szStatus
			//	, pCurrentStock->szSupervisionType
			//	, pCurrentStock->szControlType
			//	, pCurrentStock->nSpac
			//	, pCurrentStock->nPreClose
			//	, pCurrentStock->szName
			//	, pCurrentStock->szKseSectionKind
			//	, pCurrentStock->bIsBigListingStock
			//);
			varCode.Clear();
			varName.Clear();
			varFullCode.Clear();
			nCurrentLine = __LINE__;
		}
		vKseSection.clear();
		vMarketType.clear();
		vStockStatus.clear();
		vSupervisionType.clear();
		vControlType.clear();
		nCurrentLine = __LINE__;
		//DBGPRINT(L"get_stock_infos 완료"));
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
	return(nStockCount);
}

void C_CREON_PLUS::get_kospi200_list(std::vector<std::wstring>& _v)
{
	int nCurrentLine = __LINE__;
	try
	{
		_v.clear();
		::_variant_t vtList = pCodeMgr->GetGroupCodeList(180);			// _variant_t 는 _variant_t::Detach() 함수를 호출하면 자동으로 메모리를 해제한다
		::tagSAFEARRAY* pSafeArray = vtList.parray;

		long lBound = 0, uBound = 0;
		::SafeArrayGetLBound(pSafeArray, 1, &lBound);					// 이 함수는 1차원 배열의 첫번째 인덱스를 얻는다
		::SafeArrayGetUBound(pSafeArray, 1, &uBound);					// 이 함수는 1차원 배열의 마지막 인덱스를 얻는다
		//DBGPRINT(L"lBound: %d, uBound: %d", lBound, uBound);
		long nMaxStockCount = pSafeArray->rgsabound[0].cElements;		// 1차원 배열의 크기를 얻는다
		//DBGPRINT(L"종목개수: %d", nMaxStockCount);
		//for (long i = lBound; i <= uBound; ++i)
		_v.reserve(nMaxStockCount);
		for (long i = lBound; i <= uBound && i < nMaxStockCount; ++i)	// nMaxStockCount 를 이용해서 혹시 모를 무한루프 방지
		{
			::tagVARIANT varCode;
			::VariantInit(&varCode);
			if (SUCCEEDED(::SafeArrayGetElement(pSafeArray, &i, &varCode)) && VT_BSTR == varCode.vt)
			{
				_v.emplace_back(varCode.bstrVal);
				//std::wstring strName = pCodeMgr->CodeToName(varCode.bstrVal).GetBSTR();
				//DBGPRINT(L"코스피[%d]: [%s] %s\n", i, szCreonCode, strName.c_str());
			}
			::VariantClear(&varCode);
		}
		vtList.Clear();
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
}

void C_CREON_PLUS::get_kosdaq150_list(std::vector<std::wstring>& _v)
{
	int nCurrentLine = __LINE__;
	try
	{
		_v.clear();

		::_variant_t vtList = pCodeMgr->GetGroupCodeList(390);			// _variant_t 는 _variant_t::Detach() 함수를 호출하면 자동으로 메모리를 해제한다
		::tagSAFEARRAY* pSafeArray = vtList.parray;

		const long nMaxStockCount = pSafeArray->rgsabound[0].cElements;		// 1차원 배열의 크기를 얻는다
		//DBGPRINT(L"종목개수: %d", nMaxStockCount);
		_v.reserve(nMaxStockCount);

		long lBound = 0, uBound = 0;
		::SafeArrayGetLBound(pSafeArray, 1, &lBound);					// 이 함수는 1차원 배열의 첫번째 인덱스를 얻는다
		::SafeArrayGetUBound(pSafeArray, 1, &uBound);					// 이 함수는 1차원 배열의 마지막 인덱스를 얻는다
		//DBGPRINT(L"lBound: %d, uBound: %d", lBound, uBound);
		
		//for (long i = lBound; i <= uBound; ++i)
		for (long i = lBound; i <= uBound && i < nMaxStockCount; ++i)	// nMaxStockCount 를 이용해서 혹시 모를 무한루프 방지
		{
			::tagVARIANT varCode;
			::VariantInit(&varCode);
			if (SUCCEEDED(::SafeArrayGetElement(pSafeArray, &i, &varCode)) && VT_BSTR == varCode.vt)
			{
				_v.emplace_back(varCode.bstrVal);
				//std::wstring strName = pCodeMgr->CodeToName(varCode.bstrVal).GetBSTR();
				//DBGPRINT(L"코스피[%d]: [%s] %s\n", i, szCreonCode, strName.c_str());
			}
			::VariantClear(&varCode);
		}
		vtList.Clear();
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
}

std::wstring C_CREON_PLUS::make_data_path(std::wstring _pDataPath, std::wstring _pFullCode, std::wstring _pCreonCode, std::wstring _tf, long _nLength)
{
	return _pDataPath + L"\\candles\\" + _pFullCode + L"\\" + _pCreonCode + L"_" + _tf + std::to_wstring(_nLength) + L".csv";
}

void C_CREON_PLUS::set_va_tick()
{
	C_VARIANT_VTI4::C_ACCESSOR pAccessor = va_tick.access();
	PLONG pElem = pAccessor.get();
	if (pElem)
	{
		size_t nIndex = 0;
		pElem[nIndex++] = _요청구분_날짜_;									// 0: 날짜(ulong)
		pElem[nIndex++] = _요청구분_시간_;									// 1:시간(long) - hhmm 
		pElem[nIndex++] = _요청구분_종가_;									// 5:종가(long or float)
		pElem[nIndex++] = _요청구분_거래량_;								// 8:거래량(ulong or ulonglong)주) 정밀도만원단위
		pElem[nIndex++] = _요청구분_누적체결매도수량_호가비교방식_;			// 10:누적체결매도수량(ulong or ulonglong) - 호가비교방식 (분 틱 시에만 제공)
		pElem[nIndex++] = _요청구분_누적체결매수수량_호가비교방식_;			// 11:누적체결매수수량(ulong or ulonglong) - 호가비교방식 (분 틱 시에만 제공)
	}
}

void C_CREON_PLUS::set_va_sec()
{
	C_VARIANT_VTI4::C_ACCESSOR pAccessor = va_sec.access();
	PLONG pElem = pAccessor.get();
	if (pElem)
	{
		size_t nIndex = 0;
		pElem[nIndex++] = _요청구분_날짜_;									// 0: 날짜(ulong)
		pElem[nIndex++] = _요청구분_시간_;									// 1:시간(long) - hhmm 
		pElem[nIndex++] = _요청구분_시가_;									// 2:시가(long or float)
		pElem[nIndex++] = _요청구분_고가_;									// 3:고가(long or float)
		pElem[nIndex++] = _요청구분_저가_;									// 4:저가(long or float)
		pElem[nIndex++] = _요청구분_종가_;									// 5:종가(long or float)
		pElem[nIndex++] = _요청구분_거래량_;								// 8:거래량(ulong or ulonglong)주) 정밀도만원단위
		pElem[nIndex++] = _요청구분_거래대금_;								// 9:거래대금(ulong or ulonglong) - 주) 정밀도만원단위
		pElem[nIndex++] = _요청구분_누적체결매도수량_호가비교방식_;		// 10:누적체결매도수량(ulong or ulonglong) - 호가비교방식 (분 틱 시에만 제공)
		pElem[nIndex++] = _요청구분_누적체결매수수량_호가비교방식_;		// 11:누적체결매수수량(ulong or ulonglong) - 호가비교방식 (분 틱 시에만 제공)
	}
}

void C_CREON_PLUS::set_va_day()
{
	C_VARIANT_VTI4::C_ACCESSOR pAccessor = va_day.access();
	PLONG pElem = pAccessor.get();
	if (pElem)
	{
		size_t nIndex = 0;
		pElem[nIndex++] = _요청구분_날짜_;						// 0: 날짜(ulong)
		pElem[nIndex++] = _요청구분_시가_;						// 2:시가(long or float)
		pElem[nIndex++] = _요청구분_고가_;						// 3:고가(long or float)
		pElem[nIndex++] = _요청구분_저가_;						// 4:저가(long or float)
		pElem[nIndex++] = _요청구분_종가_;						// 5:종가(long or float)
		pElem[nIndex++] = _요청구분_거래량_;					// 8:거래량(ulong or ulonglong)주) 정밀도만원단위
		pElem[nIndex++] = _요청구분_거래대금_;					// 9:거래대금(ulong or ulonglong) - 주) 정밀도만원단위
		pElem[nIndex++] = _요청구분_상장주식수_;				// 12:상장주식수(ulonglong)
	}
}

void C_CREON_PLUS::set_va_week()
{
	C_VARIANT_VTI4::C_ACCESSOR pAccessor = va_week.access();
	PLONG pElem = pAccessor.get();
	if (pElem)
	{
		size_t nIndex = 0;
		pElem[nIndex++] = _요청구분_날짜_;							// 0:날짜(ulong)
		pElem[nIndex++] = _요청구분_시가_;							// 2:시가(long or float)
		pElem[nIndex++] = _요청구분_고가_;							// 3:고가(long or float)
		pElem[nIndex++] = _요청구분_저가_;							// 4:저가(long or float)
		pElem[nIndex++] = _요청구분_종가_;							// 5:종가(long or float)
		pElem[nIndex++] = _요청구분_거래량_;						// 8:거래량(ulong or ulonglong) - 주) 정밀도만원단위
	}
}

void C_CREON_PLUS::set_va_month()
{
	C_VARIANT_VTI4::C_ACCESSOR pAccessor = va_month.access();
	PLONG pElem = pAccessor.get();
	if (pElem)
	{
		size_t nIndex = 0;
		pElem[nIndex++] = _요청구분_날짜_;							// 0:날짜(ulong)
		pElem[nIndex++] = _요청구분_시가_;							// 2:시가(long or float)
		pElem[nIndex++] = _요청구분_고가_;							// 3:고가(long or float)
		pElem[nIndex++] = _요청구분_저가_;							// 4:저가(long or float)
		pElem[nIndex++] = _요청구분_종가_;							// 5:종가(long or float)
		pElem[nIndex++] = _요청구분_거래량_;							// 8:거래량(ulong or ulonglong) - 주) 정밀도만원단위
	}
}

void C_CREON_PLUS::set_input_values(LPCWSTR _creon_code, const C_VARIANT_VTI4& _va, const char _cType, const USHORT _nUnitLength, const ULONG _nReqCount)
{
	this->set_input_value(0, _creon_code);		// 0 - 종목코드(string):				주식(A003540), 업종(U001), ELW(J517016)
	this->set_input_value(1, (char)'2');		// 1 - 요청구분(char):					'1': 기간 '2': 갯수로 요청, 기간은 일봉만 가능.
	//this->set_input_value(2, _nEndDate);		// 2 - 요청종료일(ulong): YYYYMMDD형식으로데이터의마지막(가장최근) 날짜 Default(0) - 최근거래날짜
	//this->set_input_value(3, _nStartDate);		// 3 - 요청시작일(ulong): YYYYMMDD형식으로데이터의시작(가장오래된) 날짜
	this->set_input_value(4, _nReqCount);		// 4 - 요청개수(ulong): 요청할데이터의개수	ex) 최근 390개의 봉
	this->set_input_value(5, _va);				// 5 - 필드(long array): 필드 배열
	this->set_input_value(6, _cType);			// 6 - 차트구분(char)					D 일봉, W 주봉, M 월봉, m 분봉, T 틱
	this->set_input_value(7, _nUnitLength);		// 7 - 주기(ushort):				1, 3, 5, 10, 15, 30, 60, 120, 240, 300, 600, 900, 1800, 3600
	this->set_input_value(8, (char)'0');		// 8 - 갭보정여부(char):				'0' - 보정안함, '1' - 보정함
	this->set_input_value(9, (char)'0');		// 9 - 수정주가구분(char):				'0' - 수정주가적용안함, '1' - 수정주가적용
	this->set_input_value(10, (char)'3');		// 10 - 거래량구분(char):				'1' - 시간외모두포함, '2' - 장종료시간외거래량만포함, '3' - 시간외거래량모두제외, '4' - 장전시간외거래량만포함
	this->set_input_value(11, (char)'N');		// 11 - 조기적용여부(char):			'Y' - 8시 45분부터 분차트주기 계산, 'N' - 9시부터 분차트주기 계산(default)
}

void C_CREON_PLUS::set_input_values(LPCWSTR _creon_code, const C_VARIANT_VTI4& _va, const ULONG _nStartDate, const ULONG _nEndDate, const USHORT _nUnitLength)
{
	this->set_input_value(0, _creon_code);		// 0 - 종목코드(string):				주식(A003540), 업종(U001), ELW(J517016)
	this->set_input_value(1, (char)'1');		// 1 - 요청구분(char):					'1': 기간 '2': 갯수로 요청, 기간은 일봉만 가능.
	this->set_input_value(2, _nEndDate);		// 2 - 요청종료일(ulong): YYYYMMDD형식으로데이터의마지막(가장최근) 날짜 Default(0) - 최근거래날짜
	this->set_input_value(3, _nStartDate);		// 3 - 요청시작일(ulong): YYYYMMDD형식으로데이터의시작(가장오래된) 날짜
	//this->set_input_value(4, ULONG_MAX);		// 4 - 요청개수(ulong): 요청할데이터의개수	ex) 최근 390개의 봉
	this->set_input_value(5, _va);				// 5 - 필드(long array): 필드 배열
	this->set_input_value(6, (char)'D');		// 6 - 차트구분(char)					D 일봉, W 주봉, M 월봉, m 분봉, T 틱
	this->set_input_value(7, _nUnitLength);		// 7 - 주기(ushort):				1, 3, 5, 10, 15, 30, 60, 120, 240, 300, 600, 900, 1800, 3600
	this->set_input_value(8, (char)'0');		// 8 - 갭보정여부(char):				'0' - 보정안함, '1' - 보정함
	this->set_input_value(9, (char)'0');		// 9 - 수정주가구분(char):				'0' - 수정주가적용안함, '1' - 수정주가적용
	this->set_input_value(10, (char)'3');		// 10 - 거래량구분(char):				'1' - 시간외모두포함, '2' - 장종료시간외거래량만포함, '3' - 시간외거래량모두제외, '4' - 장전시간외거래량만포함
	this->set_input_value(11, (char)'N');		// 11 - 조기적용여부(char):			'Y' - 8시 45분부터 분차트주기 계산, 'N' - 9시부터 분차트주기 계산(default)
}

long C_CREON_PLUS::request_data()
{
	// 요청한다. SetInputValue 를 진행하지 않았다면 연속 조회. ( 이거 한번에 1씩 소모한다. )
	// BlockRequest 또는 BlockRequest2(0) 는 나중에 호출된게 먼저 수행되고, BlockRequest2(1) 는 순차적으로 수행됨.
	const long nResult = this->block_request2(1);
	//const long nResult = this->block_request();
	if (0 == nResult)
	{
		return this->get_header_value(3);
	}
	else if (1 == nResult)
	{
		DBGPRINT(L"BlockRequest() - 통신 요청 실패");
	}
	else if (4 == nResult)
	{
		DBGPRINT(L"BlockRequest() - 요청 제한에 걸림");
	}
	else
	{
		DBGPRINT(L"BlockRequest() 실패(%d)", nResult);
	}
	return(0);
}

bool C_CREON_PLUS::wait_request_possible()
{
	if (!this->is_connected())
	{
		DBGPRINT(L"연결이 끊어짐");
		return(false);
	}
	const long nRemainRequestCount = this->get_limit_remain_count();
	const long nRemainRequestTime = this->get_limit_request_remain_time();
	if (0 < nRemainRequestCount || 15000 == nRemainRequestTime)
	{	// 요청할 수 있는 횟수가 남아있거나 시간이 15초 남아있는 경우.
		return(true);	// 요청 가능 상태
	}
	return m_RequestEvent.Wait(m_hShutdownEvent, nRemainRequestTime + 10);
}

size_t C_CREON_PLUS::get_tick_data(std::vector<_TICK_DATA>& _v, LPCWSTR _creon_code, const ULONG _nLimitDayCount)
{
	// _nLimitDayCount = 1 이면 1일치, 2 이면 2일치, 3 이면 3일치
	// 날짜를 기록할 변수
	ULONG nDate = 0;
	// 날짜 변경을 기록할 변수
	ULONG nDateChangeCount = 0;

	_v.clear();

	bool bRequestFirst = false, bExitLoop = false;
	int nCurrentLine = __LINE__;
	try
	{
		do
		{
			// 요청 가능 상태가 아닐 경우 대기한다.
			if (!this->wait_request_possible()) { break; }
			// 첫 요청이라면 종목코드와 요청주문을 작성한다
			if (!bRequestFirst)
			{
				bRequestFirst = true;
				this->set_input_values(_creon_code, this->va_tick, _요청구분_틱_);
			}
			// 요청한다
			const long nRetDataCount = this->request_data();
			// 수신개수
			if (0 < nRetDataCount)
			{
				//if (_DEBUG_MODE_)
				//{
				//	DBGPRINT(L"[%s] 수신개수: %d, %d, %d", _creon_code, nRetDataCount, _nLimitDayCount, nDateChangeCount);
				//}
				for (long nStickIndex = 0; nStickIndex < nRetDataCount; ++nStickIndex)
				{
					size_t nIndex = 0;
					const ULONG nCurrentDate = this->get_data_value(nIndex++, nStickIndex);	// 우선 날짜를 가져온다.
					// 처음 가져온거라면 날짜를 저장한다.
					if (0 == nDate)
					{
						nDate = nCurrentDate;
					}
					// 두번째 데이터부터 확인해서 날짜가 바뀌었다면
					else if (nDate != nCurrentDate)
					{
						DBGPRINT(L"[%s] 날짜가 바뀜 %d, count: %d, %d", _creon_code, nDate, nDateChangeCount, nCurrentDate);
						nDate = nCurrentDate;		// 날짜를 갱신한다.
						++nDateChangeCount;			// 날짜 변경 카운트를 증가시킨다.
					}
					// 날짜가 변경되었는데 요청한 최근일 수와 같다면
					if (_nLimitDayCount == nDateChangeCount)
					{
						DBGPRINT(L"[%s] 최근 %d 일치의 데이터가 모두 수신됨", _creon_code, _nLimitDayCount);
						bExitLoop = true;
						break;
					}
					_v.emplace_back();	// 벡터에 공간을 확보한다.
					_TICK_DATA& pCandleData = _v.back();	// 벡터에 추가한 마지막 요소의 주소를 가져온다.
					//pCandleData.nDate = nCurrentDate;
					//pCandleData.nTime = this->get_data_value(nIndex++, nStickIndex);
					const ULONG nCurrentTime = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.set_ts(nCurrentDate, nCurrentTime);

					pCandleData.nClose = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nVolume = static_cast<ULONG>(this->get_data_value(nIndex++, nStickIndex));
					pCandleData.nAccrueVolumeSell = static_cast<ULONG>(this->get_data_value(nIndex++, nStickIndex));
					pCandleData.nAccrueVolumeBuy = static_cast<ULONG>(this->get_data_value(nIndex++, nStickIndex));
					// 거래대금은 직접 계산한다.
					pCandleData.nTransferPayment = static_cast<ULONG>(pCandleData.nVolume * pCandleData.nClose);	// 거래대금
					// 누적거래량도 직접 더한다.
					pCandleData.nAccrueVolume = pCandleData.nAccrueVolumeSell + pCandleData.nAccrueVolumeBuy;
					// 출력을 한번에 한다.
					//DBGPRINT(L"[%s] %08d %04d - 종: %d, 량: %d"
					//	L", 거래대금: %lld, 매도체결량: %d, 매수체결량: %d, 누적체결합: %lld"
					//	, _creon_code
					//	, pCandleData.nDate, pCandleData.nTime, pCandleData.nClose, pCandleData.nVolume
					//	, pCandleData.nTransferPayment
					//	, pCandleData.nAccrueVolumeSell, pCandleData.nAccrueVolumeBuy
					//	, pCandleData.nAccrueVolume
					//);
				}
			}
			else
			{
				DBGPRINT(L"[%s] 받은게 없음(실패)", _creon_code);
				break;
			}
		} while (!bExitLoop && 0 < this->get_continue());
		DBGPRINT(L"[%s] 최근 %d 일치의 데이터가 수신됨", _creon_code, nDateChangeCount);
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
	return (_v.size());
}

size_t C_CREON_PLUS::get_min_data(std::vector<_CANDLE_MIN>& _v, LPCWSTR _creon_code, const USHORT _nUnitLength, long _nReqCandleCount)
{
	// _nLimitDayCount = 1 이면 1일치, 2 이면 2일치, 3 이면 3일치
	//ULONG nToday = 0;
	//const dk::_DLOCAL_TIME lt;
	//// lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute
	//if (lt.wHour < 18) // 18시 이전이라면
	//{
	//	nToday = lt.wYear * 10000 + lt.wMonth * 100 + lt.wDay;	// 오늘 날짜
	//	++_nLimitDayCount;	// 오늘 날짜를 포함하기 위해서 요청 개수를 증가시킨다.
	//}
	//// 날짜를 기록할 변수
	//ULONG nDate = 0;
	//// 날짜 변경을 기록할 변수
	//ULONG nDateChangeCount = 0;

	_v.clear();
	bool bRequestFirst = false, bExitLoop = false;
	int nCurrentLine = __LINE__;

	try
	{
		do
		{
			// 요청 가능 상태가 아닐 경우 대기한다.
			if (!this->wait_request_possible()) { break; }
			// 첫 요청이라면 종목코드와 요청주문을 작성한다
			if (!bRequestFirst)
			{
				bRequestFirst = true;
				this->set_input_values(_creon_code, this->va_sec, _요청구분_분봉_, _nUnitLength, _nReqCandleCount);
			}
			// 요청한다
			const long nRetDataCount = this->request_data();
			// 수신개수
			if (0 < nRetDataCount)
			{
				//DBGPRINT(L"[%s] 수신개수: %d", _creon_code, nRetDataCount);
				for (long nStickIndex = 0; nStickIndex < nRetDataCount; ++nStickIndex)
				{
					size_t nIndex = 0;
					const ULONG nCurrentDate = this->get_data_value(nIndex++, nStickIndex);	// 우선 날짜를 가져온다.
					//// 처음 가져온거라면 날짜를 저장한다.
					//if (0 == nDate)
					//{
					//	nDate = nCurrentDate;
					//	if (0 != nToday && nToday == nCurrentDate)
					//	{
					//		// 오늘 날짜는 제외한다.
					//		//DBGPRINT(L"[%s] 오늘 날짜는 제외", _creon_code);
					//		continue;	// 오늘 날짜는 제외한다.
					//	}
					//}
					//// 두번째 데이터부터 확인해서 날짜가 바뀌었다면
					//else if (nDate != nCurrentDate)
					//{
					//	//DBGPRINT(L"[%s] 날짜가 바뀜 %d, count: %d, %d, %d", _creon_code, nDate, nDateChangeCount
					//	//	, nCurrentDate, lt.wYear * 10000 + lt.wMonth * 100 + lt.wDay
					//	//);
					//	nDate = nCurrentDate;		// 날짜를 갱신한다.
					//	++nDateChangeCount;			// 날짜 변경 카운트를 증가시킨다.
					//}
					//// 날짜가 변경되었는데 요청한 최근일 수와 같다면
					//if (_nLimitDayCount == nDateChangeCount)
					//{
					//	DBGPRINT(L"[%s] 최근 %d 일치의 데이터가 모두 수신됨", _creon_code, _nLimitDayCount);
					//	bExitLoop = true;
					//	break;
					//}
					_v.emplace_back();	// 벡터에 공간을 확보한다.
					_CANDLE_MIN& pCandleData = _v.back();	// 벡터에 추가한 마지막 요소의 주소를 가져온다.
					//pCandleData.nDate = nCurrentDate;
					//pCandleData.nTime = this->get_data_value(nIndex++, nStickIndex);
					const ULONG nCurrentTime = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.set_ts(nCurrentDate, nCurrentTime);

					pCandleData.nOpen = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nHigh = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nLow = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nClose = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nVolume = this->get_data_value(nIndex++, nStickIndex);
					//pCandleData.nTransferPayment = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nAccrueVolumeSell = this->get_data_value(nIndex++, nStickIndex);	// 누적체결매도수량
					pCandleData.nAccrueVolumeBuy = this->get_data_value(nIndex++, nStickIndex);		// 누적체결매수수량
					// 거래대금은 평균가로 계산한다.
					//const ULONG nAvgOC = static_cast<ULONG>(((pCandleData.nClose + pCandleData.nOpen) * 0.5));
					//pCandleData.nAccruePaymentSell = pCandleData.nAccrueVolumeSell * nAvgOC;	// 누적체결매도금액
					//pCandleData.nAccruePaymentBuy = pCandleData.nAccrueVolumeBuy * nAvgOC;		// 누적체결매수금액
					//pCandleData.nTransferPayment = pCandleData.nAccruePaymentSell + pCandleData.nAccruePaymentBuy;	// 거래대금2

					//DBGPRINT(L"[%s] %08d %04d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %d"
					//	L", 거래대금: %lld, 매도체결량: %d, 매수체결량: %d"
					//	L", 매도체결금액: %lld, 매수체결금액: %lld"
					//	, _creon_code
					//	, pCandleData.nDate, pCandleData.nTime
					//	, pCandleData.nOpen, pCandleData.nHigh, pCandleData.nLow
					//	, pCandleData.nClose, pCandleData.nVolume
					//	, pCandleData.nTransferPayment
					//	, pCandleData.nAccrueVolumeSell, pCandleData.nAccrueVolumeBuy
					//	, pCandleData.nAccruePaymentSell, pCandleData.nAccruePaymentBuy
					//	//, pCandleData.nTransferPayment2
					//);
				}

				_nReqCandleCount -= nRetDataCount;	// 요청 개수를 감소시킨다.
				if (0 >= _nReqCandleCount)
				{
					//DBGPRINT(L"[%s] 데이터가 모두 수신됨", _creon_code, _nReqCandleCount);
					bExitLoop = true;
					break;
				}
			}
			else
			{
				DBGPRINT(L"[%s] 받은게 없음(실패)", _creon_code);
				break;
			}
		} while (!bExitLoop && 0 < this->get_continue());
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
	return (_v.size());
}

size_t C_CREON_PLUS::get_day_data(std::vector<_CANDLE_DAY>& _v, LPCWSTR _creon_code, const USHORT _nUnitLength, ULONG _nLimitDayCount)
{
	// _nLimitDayCount = 1 이면 1일치, 2 이면 2일치, 3 이면 3일치

	// 만약 오늘 오후 18시 이전이라면 당일 날짜를 거른다.
	ULONG nToday = 0;
	const dk::_DLOCAL_TIME lt;
	// lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute
	if (lt.wHour < 18) // 18시 이전이라면
	{
		nToday = lt.wYear * 10000 + lt.wMonth * 100 + lt.wDay;	// 오늘 날짜
		++_nLimitDayCount;	// 오늘 날짜를 포함하기 위해서 요청 개수를 증가시킨다.
	}
	// 날짜를 기록할 변수
	ULONG nDate = 0;
	// 날짜 변경을 기록할 변수
	ULONG nDateChangeCount = 0;

	_v.clear();

	bool bRequestFirst = false, bExitLoop = false;
	int nCurrentLine = __LINE__;
	try
	{
		do
		{
			// 요청 가능 상태가 아닐 경우 대기한다.
			if (!this->wait_request_possible()) { break; }
			// 첫 요청이라면 종목코드와 요청주문을 작성한다
			if (!bRequestFirst)
			{
				bRequestFirst = true;
				this->set_input_values(_creon_code, this->va_day, _요청구분_일봉_, _nUnitLength);
			}
			// 요청한다
			const long nRetDataCount = this->request_data();
			// 수신개수
			if (0 < nRetDataCount)
			{
				//DBGPRINT(L"[%s] 수신개수: %d", _creon_code, nRetDataCount);
				for (long nStickIndex = 0; nStickIndex < nRetDataCount; ++nStickIndex)
				{
					size_t nIndex = 0;
					const ULONG nCurrentDate = this->get_data_value(nIndex++, nStickIndex);	// 우선 날짜를 가져온다.
					//DBGPRINT(L"[%s] %d", _creon_code, nCurrentDate);
					// 처음 가져온거라면 날짜를 저장한다.
					if (0 == nDate)
					{
						nDate = nCurrentDate;
						if (0 != nToday && nToday == nCurrentDate)
						{
							// 오늘 날짜는 제외한다.
							//DBGPRINT(L"[%s] 오늘 날짜는 제외", _creon_code);
							continue;	// 오늘 날짜는 제외한다.
						}
					}
					// 두번째 데이터부터 확인해서 날짜가 바뀌었다면
					else if (nDate != nCurrentDate)
					{
						//DBGPRINT(L"[%s] 날짜가 바뀜 %d, count: %d, %d, %d", _creon_code, nDate, nDateChangeCount
						//	, nCurrentDate, lt.wYear * 10000 + lt.wMonth * 100 + lt.wDay
						//);
						nDate = nCurrentDate;		// 날짜를 갱신한다.
						++nDateChangeCount;			// 날짜 변경 카운트를 증가시킨다.
					}
					// 날짜가 변경되었는데 요청한 최근일 수와 같다면
					if (_nLimitDayCount == nDateChangeCount)
					{
						//DBGPRINT(L"[%s] 최근 %d 일치의 데이터가 모두 수신됨", _creon_code, _nLimitDayCount);
						bExitLoop = true;
						break;
					}
					_v.emplace_back();	// 벡터에 공간을 확보한다.
					_CANDLE_DAY& pCandleData = _v.back();	// 벡터에 추가한 마지막 요소의 주소를 가져온다.
					pCandleData.nDate = nCurrentDate;
					pCandleData.nOpen = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nHigh = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nLow = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nClose = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nVolume = this->get_data_value(nIndex++, nStickIndex);
					//pCandleData.nTransferPayment = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nListedStocks = this->get_data_value(nIndex++, nStickIndex);
					// 시가총액은 상장주식수 * 종가로 계산한다.
					pCandleData.nMarketCap = pCandleData.nListedStocks * pCandleData.nClose;

					//DBGPRINT(L"[%s] %08d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %d"
					//	L", 거래대금: %lld, 상장주식수: %lld, 시가총액: %lld"
					//	, _creon_code
					//	, pCandleData.nDate
					//	, pCandleData.nOpen, pCandleData.nHigh, pCandleData.nLow
					//	, pCandleData.nClose, pCandleData.nVolume
					//	, pCandleData.nTransferPayment
					//	, pCandleData.nListedStocks, pCandleData.nMarketCap
					//);
				}
			}
		} while (!bExitLoop && 0 < this->get_continue());
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
	return (_v.size());
}

size_t C_CREON_PLUS::get_day_data(std::vector<_CANDLE_DAY>& _v, LPCWSTR _creon_code, ULONG _nStartDate, ULONG _nEndDate, USHORT _nUnitLength)
{
	_v.clear();

	bool bRequestFirst = false, bExitLoop = false;
	int nCurrentLine = __LINE__;
	try
	{
		do
		{
			// 요청 가능 상태가 아닐 경우 대기한다.
			if (!this->wait_request_possible()) { break; }
			// 첫 요청이라면 종목코드와 요청주문을 작성한다
			if (!bRequestFirst)
			{
				bRequestFirst = true;
				this->set_input_values(_creon_code, this->va_day, _nStartDate, _nEndDate, _nUnitLength);
			}
			// 요청한다
			const long nRetDataCount = this->request_data();
			// 수신개수
			if (0 < nRetDataCount)
			{
				//DBGPRINT(L"[%s] 수신개수: %d", _creon_code, nRetDataCount);
				for (long nStickIndex = 0; nStickIndex < nRetDataCount; ++nStickIndex)
				{
					size_t nIndex = 0;
					const ULONG nCurrentDate = this->get_data_value(nIndex++, nStickIndex);	// 우선 날짜를 가져온다.
					//DBGPRINT(L"[%s] %d", _creon_code, nCurrentDate);
					if (0 < nStickIndex && _v[nStickIndex - 1].nDate == nCurrentDate)
					{
						continue;
					}
					_v.emplace_back();	// 벡터에 공간을 확보한다.
					_CANDLE_DAY& pCandleData = _v.back();	// 벡터에 추가한 마지막 요소의 주소를 가져온다.
					
					pCandleData.nDate = nCurrentDate;
					pCandleData.nOpen = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nHigh = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nLow = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nClose = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nVolume = this->get_data_value(nIndex++, nStickIndex);
					//pCandleData.nTransferPayment = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nListedStocks = this->get_data_value(nIndex++, nStickIndex);
					// 시가총액은 상장주식수 * 종가로 계산한다.
					pCandleData.nMarketCap = pCandleData.nListedStocks * pCandleData.nClose;

					//DBGPRINT(L"[%s] %08d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %d"
					//	L", 거래대금: %lld, 상장주식수: %lld, 시가총액: %lld, size: %d"
					//	, _creon_code
					//	, pCandleData.nDate
					//	, pCandleData.nOpen, pCandleData.nHigh, pCandleData.nLow
					//	, pCandleData.nClose, pCandleData.nVolume
					//	, pCandleData.nTransferPayment
					//	, pCandleData.nListedStocks, pCandleData.nMarketCap
					//	, _v.size()
					//);
				}
			}
		} while (!bExitLoop && 0 < this->get_continue());
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
	return (_v.size());
}

size_t C_CREON_PLUS::get_week_data(std::vector<_CANDLE_WEEK>& _v, LPCWSTR _creon_code, bool _bNone)
{
	_v.clear();
	bool bRequestFirst = false, bExitLoop = false;
	int nCurrentLine = __LINE__;
	try
	{
		do
		{
			// 요청 가능 상태가 아닐 경우 대기한다.
			if (!this->wait_request_possible()) { break; }
			// 첫 요청이라면 종목코드와 요청주문을 작성한다
			if (!bRequestFirst)
			{
				bRequestFirst = true;
				const long _nReqDataCount = _bNone ? ULONG_MAX : 1;	// 요청 개수
				this->set_input_values(_creon_code, this->va_week, _요청구분_주봉_, 1, _nReqDataCount);
			}
			// 요청한다
			const long nRetDataCount = this->request_data();
			// 수신개수
			if (0 < nRetDataCount)
			{
				if (_DEBUG_MODE_)
				{
					DBGPRINT(L"[%s] 수신개수: %d", _creon_code, nRetDataCount);
				}
				for (long nStickIndex = 0; nStickIndex < nRetDataCount; ++nStickIndex)
				{
					size_t nIndex = 0;
					const ULONG nCurrentDate = this->get_data_value(nIndex++, nStickIndex);	// 우선 날짜를 가져온다.
					
					_v.emplace_back();	// 벡터에 공간을 확보한다.
					_CANDLE_WEEK& pCandleData = _v.back();	// 벡터에 추가한 마지막 요소의 주소를 가져온다.
					pCandleData.nDate = nCurrentDate;
					pCandleData.nOpen = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nHigh = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nLow = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nClose = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nVolume = static_cast<ULONGLONG>(this->get_data_value(nIndex++, nStickIndex));
					if (_DEBUG_MODE_)
					{
						DBGPRINT(L"[%s] %08d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %I64u"
							, _creon_code
							, pCandleData.nDate
							, pCandleData.nOpen, pCandleData.nHigh, pCandleData.nLow
							, pCandleData.nClose, pCandleData.nVolume
						);
					}
				}
			}
			else
			{
				DBGPRINT(L"[%s] 받은게 없음(실패)", _creon_code);
				break;
			}
		} while (!bExitLoop && 0 < this->get_continue());
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
	return (_v.size());
}

size_t C_CREON_PLUS::get_month_data(std::vector<_CANDLE_MONTH>& _v, LPCWSTR _creon_code, bool _bNone)
{
	_v.clear();
	bool bRequestFirst = false, bExitLoop = false;
	int nCurrentLine = __LINE__;
	try
	{
		do
		{
			// 요청 가능 상태가 아닐 경우 대기한다.
			if (!this->wait_request_possible()) { break; }
			// 첫 요청이라면 종목코드와 요청주문을 작성한다
			if (!bRequestFirst)
			{
				bRequestFirst = true;
				const long _nReqDataCount = _bNone ? ULONG_MAX : 1;	// 요청 개수
				this->set_input_values(_creon_code, this->va_month, _요청구분_월봉_, 1, _nReqDataCount);
			}
			// 요청한다
			const long nRetDataCount = this->request_data();
			// 수신개수
			if (0 < nRetDataCount)
			{
				//DBGPRINT(L"[%s] 수신개수: %d", _creon_code, nRetDataCount);
				for (long nStickIndex = 0; nStickIndex < nRetDataCount; ++nStickIndex)
				{
					size_t nIndex = 0;
					const ULONG nCurrentDate = this->get_data_value(nIndex++, nStickIndex);	// 우선 날짜를 가져온다.
					
					_v.emplace_back();	// 벡터에 공간을 확보한다.
					_CANDLE_MONTH& pCandleData = _v.back();	// 벡터에 추가한 마지막 요소의 주소를 가져온다.
					pCandleData.nDate = nCurrentDate;
					pCandleData.nOpen = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nHigh = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nLow = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nClose = this->get_data_value(nIndex++, nStickIndex);
					pCandleData.nVolume = static_cast<ULONGLONG>(this->get_data_value(nIndex++, nStickIndex));

					//DBGPRINT(L"[%s] %08d - 시: %d, 고: %d, 저: %d, 종: %d, 량: %d"
					//	L", 거래대금: %lld"
					//	, _creon_code
					//	, pCandleData.nDate
					//	, pCandleData.nOpen, pCandleData.nHigh, pCandleData.nLow
					//	, pCandleData.nClose, pCandleData.nVolume
					//	, pCandleData.nTransferPayment
					//);
				}
			}
			else
			{
				DBGPRINT(L"[%s] 받은게 없음(실패)", _creon_code);
				break;
			}
		} while (!bExitLoop && 0 < this->get_continue());
	}
	catch (...)
	{
		DBGPRINT(L"[예외 발생] %s - %s(%d)", __FUNCTIONW__, __FILEW__, nCurrentLine);
	}
	return (_v.size());
}
