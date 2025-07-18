// BrokerCR.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "CMain.h"


dk::C_CONFIG* g_pConfig = nullptr;
dk::C_LOG* g_pLog = nullptr;

int APIENTRY wWinMain(_In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE _hPrevInstance, _In_ LPWSTR _lpCmdLine, _In_ int _nCmdShow)
{
	UNREFERENCED_PARAMETER(_hPrevInstance);
	UNREFERENCED_PARAMETER(_lpCmdLine);
	UNREFERENCED_PARAMETER(_nCmdShow);

	// TODO: 여기에 코드를 입력합니다
	dk::C_MUTEX mutex;
	if (!::wcscmp(_lpCmdLine, L"restart"))
	{	// 자신을 실행하고 종료하는 경우 기다리기 위해.
		do
		{
			if (!mutex.Create(L"BrokerCR")) { break; }	// 뮤텍스 생성에 성공하면 빠져나간다.
			dk::sleep(200);
		} while (true);
	}
	else
	{	// 재실행이 아닌 경우
		mutex.Create(L"BrokerCR");
	}
	if (!mutex.CheckError())
	{
		C_MAIN* pMain = new C_MAIN();
		if (pMain)
		{
			
			if (pMain->Init(_hInstance))
			{
				if (::wcsstr(_lpCmdLine, L"update"))
				{	// 업데이트가 필요한 경우
					pMain->update();
				}
				else
				{
					pMain->Display();
				}
			}
			pMain->Destroy();
		}
		delete pMain;
	}
	return(0);
}
