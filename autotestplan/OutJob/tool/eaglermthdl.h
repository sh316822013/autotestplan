#pragma once
#ifdef EAGLERMTHDL_EXPORTS
#define EAGLERMTHDL_API __declspec(dllexport)
#else
#define EAGLERMTHDL_API __declspec(dllimport)
#endif
//eaglermthdl version=0.0.1
#define EAGLERMTHDLVERSION _T("0.0.1")

EAGLERMTHDL_API int EnableRecord();
EAGLERMTHDL_API int RecordCondition(char* pParamIndex, char* HighLightPin="",char* MeasMode="",char* Comments="");
EAGLERMTHDL_API int SaveImage(void);

