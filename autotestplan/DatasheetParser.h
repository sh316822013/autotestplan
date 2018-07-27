// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DATASHEETPARSER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DATASHEETPARSER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#pragma once
#ifdef DATASHEETPARSER_EXPORTS
#define DATASHEETPARSER_API __declspec(dllexport)
#else
#define DATASHEETPARSER_API __declspec(dllimport)
#endif

using namespace std;
#define PROJTYPE_EAGLE  _T("eagle")
#define PROJTYPE_ACCO  _T("acco")

//testplan referenced stuff
#define RMT_MAX_PARAMINDEX_STR_LENTH   10
#define RMT_MAX_PINNAME_STR_LENTH 32
#define RMT_MAX_STAT_STR_LENTH   32
#define RMT_MAX_PIN   64 //max pin supported
//Max test param supported
#define RMT_MAX_TESTPLAN_PARAM (200*1024*1024)/sizeof(ParamStruct)
typedef struct 
{
	char Name[RMT_MAX_PINNAME_STR_LENTH];
	char Stat[RMT_MAX_STAT_STR_LENTH];
} PinStatStruct;
#define RMT_MAX_HIGHLIGHTPIN 24
#define RMT_MAX_MEASMODE 24
#define RMT_MAX_COMMENTS 128
typedef struct 
{
	char Parameter[RMT_MAX_PARAMINDEX_STR_LENTH];
	char HighLightPin[RMT_MAX_HIGHLIGHTPIN];
	char MeasMode[RMT_MAX_MEASMODE];
	char Comments[RMT_MAX_COMMENTS];
	PinStatStruct PinInfo[RMT_MAX_PIN];
} ParamStruct;
//testplan referenced stuff END

#define MAX_TMP_STR_LENTH 256
#define MAX_LIMIT_SET 16
#define MAX_FUNCTION 32
#define MAX_PARAMETER_PER_FUNCTION 1024
#define MAX_PARAMETER (200*1024*1024)/sizeof(ParamStruct)
#define MAX_PARAM_NAME_STR_LENTH 64
#define MAX_PARAM_LIMIT_STR_LENTH 16
#define MAX_PARAM_UNIT_STR_LENTH 10
//for datasheet parser using
typedef struct tagLimitSetName
{
	TCHAR LowLimitName[MAX_PARAM_NAME_STR_LENTH];
	TCHAR HighLimitName[MAX_PARAM_NAME_STR_LENTH];
}LIMITSETNAME_STR;

typedef struct tagParameter
{
	TCHAR ParamName[MAX_PARAM_NAME_STR_LENTH];
	TCHAR ParamIndex[MAX_PARAM_NAME_STR_LENTH];
	TCHAR LowLimit[MAX_LIMIT_SET][MAX_PARAM_LIMIT_STR_LENTH];
	TCHAR HighLimit[MAX_LIMIT_SET][MAX_PARAM_LIMIT_STR_LENTH];
	TCHAR Unit[MAX_PARAM_UNIT_STR_LENTH];
}	PARAM_STR;

typedef struct tagFunction
{
	int IsEnable;
	int TotalParam;
	TCHAR Name[MAX_TMP_STR_LENTH];
	TCHAR szFuncIndex[MAX_TMP_STR_LENTH];
	PARAM_STR Parameter[MAX_PARAMETER_PER_FUNCTION];
	int ParamTag;
}	FUNC_STR;
typedef struct tagCALL_ORDER
{
	TCHAR szFuncName[MAX_FUNCTION];
	int nOrder[MAX_FUNCTION];
}	CALL_ORDER;

#pragma region "pgs/acco"
class DATASHEETPARSER_API CpgsParser
{
public:
	CpgsParser(void);
	~CpgsParser(void);
public:
	//input method
	void init();
	int analysis(const TCHAR* lpszPath);
	//output
	BOOL inline getInitState(void) {return this->bIsInitSuccess;};
	BOOL getProductProgramName(LPTSTR szOutBuffer);
	DWORD inline getFuncCnt(void) {return this->TestFunctionCount;}
	int getAllParamCnt(void);
	FUNC_STR* getFunc(DWORD wFuncNum);
public:
	//protected:
	BOOL bIsInitSuccess;
	int sSiteCount;
	TCHAR szProductName[MAX_TMP_STR_LENTH];
	FUNC_STR Function[MAX_FUNCTION];
	int funcTag;
	int TestFunctionCount;
	//new added attribute
	int iLimitSets;
	LIMITSETNAME_STR uLimitSetName[MAX_LIMIT_SET];
public://inner use method
	BOOL parseParam(int iFuncNum,int iParamNum,TCHAR inbuf[],PARAM_STR* pParam);
	BOOL parseFunc(TCHAR inbuf[],FUNC_STR* pFunc);
};
#pragma endregion "pgs/acco"

#pragma region "pds/eagle"
//max DatasheetVariableMap count
#define MAX_DVM_CNT 32 
#define MAX_DVM_STR_LENTH 32 
typedef struct tagPDSKeyWords
{
	
	//tag inherited from acco pgs
	TCHAR tagEagleProductName[MAX_TMP_STR_LENTH];
	TCHAR tagEagleSite[MAX_TMP_STR_LENTH];
	TCHAR tagEagleFuncCnt[MAX_TMP_STR_LENTH];
	TCHAR tagEagleFunc[MAX_TMP_STR_LENTH];
	TCHAR tagEagleEndFunc[MAX_TMP_STR_LENTH];
	TCHAR tagEagleSeperator[MAX_TMP_STR_LENTH];
	//tag only for eagle pds
	TCHAR tagEagleDatasheetPreferences[MAX_TMP_STR_LENTH];
	TCHAR tagEagleDocumentationBlock[MAX_TMP_STR_LENTH];
	TCHAR tagEagleBinningBlock[MAX_TMP_STR_LENTH];
	TCHAR tagEagleDatasheetVariableMap[MAX_TMP_STR_LENTH];
	TCHAR tagEagleFunction[MAX_TMP_STR_LENTH];
	TCHAR tagEagleTestOrder[MAX_TMP_STR_LENTH];
	TCHAR tagEagleBlockEnd[MAX_TMP_STR_LENTH];
	TCHAR tagEagleDataSheetFileName[MAX_TMP_STR_LENTH];
	TCHAR tagEagleLowLimitName[MAX_TMP_STR_LENTH];
	TCHAR tagEagleHighLimitName[MAX_TMP_STR_LENTH];
	TCHAR tagEagleUnit[MAX_TMP_STR_LENTH];
	TCHAR tagEagleDLogDesc[MAX_TMP_STR_LENTH];
	TCHAR tagEagleTestNmbr[MAX_TMP_STR_LENTH];
	TCHAR tagEagleSubTestNmbr[MAX_TMP_STR_LENTH];
}PDSKEYWORDS;

class DATASHEETPARSER_API CpdsParser
{
public:
	CpdsParser(void);
	~CpdsParser(void);
public://inner use method
	int parseParam(const TCHAR* lpszInput,TCHAR (*strOutput)[32] );
	BOOL parseFunc(TCHAR inbuf[],FUNC_STR* pFunc);
public:
	//input method
	void init();
	int analysis(const TCHAR* lpszPath);
	//output
	BOOL inline getInitState(void) {return this->bIsInitSuccess;};
	DWORD getProductProgramName(LPTSTR szOutBuffer);
	DWORD inline getFuncCnt(void) {return this->TestFunctionCount;}
	int getAllParamCnt(void);
	//FUNC_STR* getFunc(DWORD wFuncNum);
public:
	//compatible with ACCO pgs file
	BOOL bIsInitSuccess;
	int sSiteCount;
	TCHAR szProductName[MAX_TMP_STR_LENTH];
	//FUNC_STR Function[MAX_FUNCTION];
	int funcTag;
	int TestFunctionCount;
	//eagle pds distinctive member
	int TestParamCount;
	PDSKEYWORDS uKeyWords;
	TCHAR szDataSheetFileName[MAX_TMP_STR_LENTH];
	TCHAR szDatasheetVariableMap[MAX_DVM_CNT][MAX_DVM_STR_LENTH];
	TCHAR szDatasheetVariable[MAX_PARAMETER][MAX_DVM_CNT][MAX_DVM_STR_LENTH];
	int iDVMcnt;
	int iDVcnt;
	int iLimitSets;
	LIMITSETNAME_STR uLimitSetName[MAX_LIMIT_SET];
};
#pragma endregion "pds/eagle"

#pragma region "datasheet"
class DATASHEETPARSER_API CDataSheet
{
//constructor and destructor
public:
	CDataSheet(void);//clear all content to zero
	~CDataSheet(void);
public:
	//input method
	void init();
	INT loadFromClass(CpdsParser* pCpds,LPTSTR type);
	INT loadFromClass(CpgsParser* pCpgs,LPTSTR type);
	//output
	BOOL inline getInitState(void) {return this->bIsInitSuccess;};
	DWORD inline getFuncCnt(void) {return this->TestFunctionCount;}
	int getProductProgramName(LPTSTR szOutBuffer);
	int getAllParamCnt(void);
	FUNC_STR* getFunc(DWORD wFuncNum);
public:
	TCHAR szProjectType[MAX_TMP_STR_LENTH];
	BOOL bIsInitSuccess;
	TCHAR szProductName[MAX_TMP_STR_LENTH];
	int sSiteCount;
	int TestFunctionCount;
	FUNC_STR Function[MAX_FUNCTION];
	int funcTag;
public://new added attribute
	int iLimitSets;
	LIMITSETNAME_STR uLimitSetName[MAX_LIMIT_SET];
	CALL_ORDER structCallOrder;
};
#pragma endregion "datasheet"
