#pragma once
//#include "MyCommonString.h"
#include "SrcCodeParserInterface.h"
//for distinction mark
typedef struct tagACCOSYMBOLDATABASE
{
//acco used symbol
/*tpp-1*/
	//test mode
	AISyblAndMark TestMode;
	//V A
	AISyblAndMark MIRET;
	AISyblAndMark MVRET;
	AISyblAndMark GetMesaResult;
	AISyblAndMark MeasureVI;
	//timing
	AISyblAndMark Meas;
	AISyblAndMark SinglePlsMeas;
	AISyblAndMark MeasFreq;
	AISyblAndMark QTMU_PLUS_COARSE;
	AISyblAndMark QTMU_PLUS_FINE;
	AISyblAndMark QTMU_PLUS_TRNG_US;
	AISyblAndMark QTMU_PLUS_TRNG_NS;
	//resistor
	AISyblAndMark DivisionSign;
	//AWG
	AISyblAndMark STSAWGCreateRampData	;
	AISyblAndMark STSEnableAWG;
	AISyblAndMark STSEnableMeas;
	AISyblAndMark SetMeasVTrig;
	AISyblAndMark SetMeasITrig;
	AISyblAndMark AwgLoader;
	//general
	AISyblAndMark RecordCondition;
	AISyblAndMark Space;
	AISyblAndMark Tab;
	AISyblAndMark Quote;
	AISyblAndMark Equal;
	AISyblAndMark Seperator;

}ACCOSYMBOLDATABASE;

class CAccoSrcCodeParser:public CSrcCodeParserInterface
{
public:
	CAccoSrcCodeParser(void);
	~CAccoSrcCodeParser(void);

public://initialize methid
	void init(void);
	void initDataBase(BOOL type);//init database 1:all,0:just init result
public://inner using method
	DWORD analysis(FILE* pFile,CDataSheet* pDatasheetparser);//after select pgs will excute
	void SearchLine(tstring& str,int line);
public://input&output method
int GetAccuracy(tstring& strOut,ParamText* pstrParamText);
	int AI_ParserParameter(CString& strIn,CString* cstrUnit);
	ParamText* getParamTextStructByIndex(CString& strindex);
	CString getParamText(CString& strIn);
public:
	bool bIsAnalysisSuccess;
	ParamText Param[MAX_PARAMETER];
	ParamText uAnalysisFailParam;
	ParamText uAnalysisSuccessButNoSpecifiedParam;
	
protected:
	ACCOSYMBOLDATABASE DB;//search symbol
	DWORD dParamCnt;
};

