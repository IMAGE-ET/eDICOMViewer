// dllmain.cpp: DLL 응용 프로그램의 진입점을 정의합니다.
#include "stdafx.h"
#include "NodeDCMTK.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

int test_dcmtk(int a, int b)
{
	DcmFileFormat dcmfileFormat;
	OFCondition status = dcmfileFormat.loadFile("..\\etc\\sampleDICOM\\0001.DCM");
	if (status.good())
	{
		for (long i = 0; i < dcmfileFormat.getDataset()->card(); i++)
		{
			DcmElement* pElement = dcmfileFormat.getDataset()->getElement(i);
			if (pElement == NULL)
				continue;

			OFString value;
			status = pElement->getOFString(value, 0);
			if (!status.good())
				continue;

			char buffer[256];
			sprintf(buffer, "[%04X:%04X] %s : %s", pElement->getGTag(), pElement->getETag(), ((DcmTag&)pElement->getTag()).getTagName(), value);
			OutputDebugStringA(buffer);
		}
	}
	return a + b;
}

void test_parameter_string(char* param)
{
	char buffer[256];
	sprintf(buffer, "test_parameter_string::param=%s", param);
	OutputDebugStringA(buffer);
}