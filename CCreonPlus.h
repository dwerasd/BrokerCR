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


#include <Windows.h>
#include <comutil.h>

#include <vector>
#include <unordered_map>

#include <DarkCore/DSingleton.h>
#include <DarkCore/DString.h>
#include <DarkCore/DLocale.h>
#include <DarkCore/DTimer.h>
#include <DarkCore/DUtil.h>
#include <DarkCore/DConfig.h>
#include <DarkCore/DLog.h>
#include <DarkCore/DEvent.h>

#include <DOpenSources/readerwriterqueue.h>
#include <DOpenSources/blockingconcurrentqueue.h>

// 이걸 반드시 써야하고 쓰면 /MP 를 사용하지 못한다.
#import <cpdib.dll>
#import <cputil.dll>
#import <cpsysdib.dll>
#import <cptrade.dll>

#include <Defines/StockDef.h>
#include <Defines/CreonPlusDef.h>



struct _OBJECT_INFO
{
	dk::DGUID clsid{};
	dk::DGUID iid{};
	LPVOID pObject{ nullptr };
};

class C_CREON_PLUS
{
private:
	CPUTILLib::ICpCybos* pCybos{ nullptr };
	CPUTILLib::ICpStockCode* pStockCode{ nullptr };
	CPUTILLib::ICpCodeMgr* pCodeMgr{ nullptr };
	DSCBO1Lib::IDib* pStockBid{ nullptr };
	CPSYSDIBLib::ISysDib* pMarketEye{ nullptr };
	CPSYSDIBLib::ISysDib* pStockChart{ nullptr };
	CPTRADELib::ICpTdUtil* pTrade{ nullptr };

	dk::C_EVENT_AUTO_RESET m_RequestEvent;
	dk::C_HANDLE m_hShutdownEvent;

	std::wstring 크레온계정;
	std::wstring 크레온비밀번호;
	std::wstring 인증서비밀번호;

	std::unordered_map<std::wstring, _OBJECT_INFO> umapCreonObjects;

	//typedef std::unordered_map<char, std::wstring> UMAP_TF_PARAMS;
	//UMAP_TF_PARAMS umap_tf_params;

public:
	C_CREON_PLUS();
	C_CREON_PLUS(LPCWSTR _pCreonID, LPCWSTR _pCreonPW, LPCWSTR _pCertPW);
	C_CREON_PLUS(std::wstring_view _pCreonID, std::wstring_view _pCreonPW, std::wstring_view _pCertPW);
	virtual ~C_CREON_PLUS();

	void init(LPCWSTR _pCreonID, LPCWSTR _pCreonPW, LPCWSTR _pCertPW);
	void init(std::wstring_view _pCreonID, std::wstring_view _pCreonPW, std::wstring_view _pCertPW);
	bool create();
	void destroy();

	void set_shutdown_handle(const dk::C_HANDLE& hEvent) { m_hShutdownEvent = hEvent; }

	bool create_creon_object(_OBJECT_INFO& _pInfo);

	inline LPVOID get_creon_object(LPCWSTR _pName) { return this->umapCreonObjects[_pName].pObject; }
	inline LPVOID get_creon_object(std::wstring_view _pName) { return this->umapCreonObjects[_pName.data()].pObject; }

	bool create_creon_plus();
	bool connect_creon_plus();

	inline void signal_request_possible() { m_RequestEvent.Set(); }

	long get_market_start_time();
	long get_market_end_time();

	long get_limit_remain_count(CPUTILLib::LIMIT_TYPE _nLimit_Type = CPUTILLib::LIMIT_TYPE::LT_NONTRADE_REQUEST);
	long get_limit_request_remain_time();

	bool is_connected();
	short get_stock_count();

	_variant_t get_data(short type, short index);
	_variant_t get_data_value(int __MIDL__ISysDib0001, int __MIDL__ISysDib0002);

	long get_continue();

	HRESULT set_input_value(int __MIDL__ISysDib0004, const _variant_t& newVal);
	_variant_t get_header_value(int __MIDL__ISysDib0000);
	short block_request();
	short block_request2(short BlockOption);

	long get_stock_market_kind(LPCWSTR _pCode);
	long get_stock_section_kind(LPCWSTR _pCode);
	long get_stock_status_kind(LPCWSTR _pCode);
	long get_stock_supervision_kind(LPCWSTR _pCode);
	long get_stock_control_kind(LPCWSTR _pCode);
	long is_spac(LPCWSTR _pCode);
	long get_stock_yd_close_price(LPCWSTR _pCode);
	long is_big_listing_stock(LPCWSTR _pCode);

	short get_stock_infos(std::vector<_STOCK_INFO_CREON>& _v);
	void get_kospi200_list(std::vector<std::wstring>& _v);
	void get_kosdaq150_list(std::vector<std::wstring>& _v);

	inline long request_data();
	inline bool wait_request_possible();

	// ts는 틱, 분, day는 일봉, wm은 주봉, 월봉
	C_VARIANT_VTI4 va_tick{ 6, VT_I4 };
	void set_va_tick();

	C_VARIANT_VTI4 va_sec{ 10, VT_I4 };
	void set_va_sec();

	C_VARIANT_VTI4 va_day{ 8, VT_I4 };
	void set_va_day();

	C_VARIANT_VTI4 va_week{ 6, VT_I4 };
	void set_va_week();

	C_VARIANT_VTI4 va_month{ 6, VT_I4 };
	void set_va_month();
	//inline void set_input_value_tick(LPCWSTR _creon_code, USHORT _nUnitLength = 1, ULONG _nReqCount = ULONG_MAX);
	//inline void set_input_value_sec(LPCWSTR _creon_code, USHORT _nUnitLength = 1, ULONG _nReqCount = 390);
	inline void set_input_values(LPCWSTR _creon_code, const C_VARIANT_VTI4& _va, const char _cType = 'T', const USHORT _nUnitLength = 1, const ULONG _nReqCount = ULONG_MAX);
	inline void set_input_values(LPCWSTR _creon_code, const C_VARIANT_VTI4& _va, const ULONG _nStartDate, const ULONG _nEndDate = 0, const USHORT _nUnitLength = 1);

	// 갯수로 틱 요청
	size_t get_tick_data(std::vector<_TICK_DATA>& _v, LPCWSTR _creon_code, const ULONG _nLimitDayCount = 1);
	inline size_t get_tick_data(std::vector<_TICK_DATA>& _v, std::wstring_view _creon_code, const ULONG _nLimitDayCount = 1)
	{
		return this->get_tick_data(_v, _creon_code.data(), _nLimitDayCount);
	}
	// 갯수로 분봉 요청
	size_t get_min_data(std::vector<_CANDLE_MIN>& _v, LPCWSTR _creon_code, const USHORT _nUnitLength = 1, long _nReqCandleCount = 1);
	inline size_t get_min_data(std::vector<_CANDLE_MIN>& _v, std::wstring_view _creon_code, const USHORT _nUnitLength = 1, const long _nReqCandleCount = 1)
	{
		return this->get_min_data(_v, _creon_code.data(), _nUnitLength, _nReqCandleCount);
	}
	// 갯수로 일봉 요청
	size_t get_day_data(std::vector<_CANDLE_DAY>& _v, LPCWSTR _creon_code, const USHORT _nUnitLength = 1, ULONG _nLimitDayCount = 1);
	inline size_t get_day_data(std::vector<_CANDLE_DAY>& _v, std::wstring_view _creon_code, const USHORT _nUnitLength = 1, const ULONG _nLimitDayCount = 1)
	{
		return this->get_day_data(_v, _creon_code.data(), _nUnitLength, _nLimitDayCount);
	}
	// 날짜로 일봉 요청
	size_t get_day_data(std::vector<_CANDLE_DAY>& _v, LPCWSTR _creon_code, ULONG _nStartDate, ULONG _nEndDate, USHORT _nUnitLength = 1);
	inline size_t get_day_data(std::vector<_CANDLE_DAY>& _v, std::wstring_view _creon_code, ULONG _nStartDate, ULONG _nEndDate, USHORT _nUnitLength = 1)
	{
		return this->get_day_data(_v, _creon_code.data(), _nStartDate, _nEndDate, _nUnitLength);
	}
	
	// 갯수로 주봉 요청
	size_t get_week_data(std::vector<_CANDLE_WEEK>& _v, LPCWSTR _creon_code, bool _bNone = false);
	inline size_t get_week_data(std::vector<_CANDLE_WEEK>& _v, std::wstring_view _creon_code, bool _bNone = false)
	{
		return this->get_week_data(_v, _creon_code.data(), _bNone);
	}
	// 갯수로 월봉 요청
	size_t get_month_data(std::vector<_CANDLE_MONTH>& _v, LPCWSTR _creon_code, bool _bNone = false);
	inline size_t get_month_data(std::vector<_CANDLE_MONTH>& _v, std::wstring_view _creon_code, bool _bNone = false)
	{
		return this->get_month_data(_v, _creon_code.data(), _bNone);
	}

	std::wstring make_data_path(std::wstring _pDataPath, std::wstring _pFullCode, std::wstring _pCreonCode, std::wstring _tf, long _nLength);
	//std::wstring get_tf_params(char _tf) { return umap_tf_params[_tf]; }

	void test_creon();
};
