#pragma once
#include "SrcCodeParserInterface.h"

//for distinction mark
typedef struct tagEAGLESYMBOLDATABASE
{
	//resistor
	AISyblAndMark DivisionSign;
	//general
	AISyblAndMark RecordCondition;
	AISyblAndMark Space;
	AISyblAndMark Tab;
	AISyblAndMark Quote;
	AISyblAndMark Equal;
	AISyblAndMark Seperator;
	AISyblAndMark LeftBracket;
	AISyblAndMark RightBracket;
//eagle used symbol
/*tpp-1*/
	//apu spu
	AISyblAndMark mv;
	AISyblAndMark mi;
	AISyblAndMark apu12mv;
	AISyblAndMark apu12mi;
	AISyblAndMark sp100mv;
	AISyblAndMark sp100mi;
	AISyblAndMark sp112mv;
	AISyblAndMark sp112mi;
	AISyblAndMark sp500mv;
	AISyblAndMark sp500mi;
	AISyblAndMark apu12;
	AISyblAndMark sp100;
	AISyblAndMark sp112;
	AISyblAndMark sp500;
	AISyblAndMark _MI_;
	AISyblAndMark _MV_;

	//timing
	AISyblAndMark qtmumt;
	AISyblAndMark QTMU_READTIME;
	AISyblAndMark QTMU_READFREQ;
	AISyblAndMark QTMU_READCOUNTER;
	AISyblAndMark QTMU_READ_ELAPSEDTIME;
	//awg
	AISyblAndMark awgselect;
	AISyblAndMark adcmode;
	AISyblAndMark whereat;
	AISyblAndMark mcurun;
	

}EAGLESYMBOLDATABASE;


class CEagleSrcCodeParser:public CSrcCodeParserInterface
{
public:
	CEagleSrcCodeParser(void);
	~CEagleSrcCodeParser(void);

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

	//new added, Acco no need this
	void CEagleSrcCodeParser::FormatAccyString(LPTSTR pstrOut,tstring& PinMark,tstring& PinMethod,LPCTSTR szRange,LPCTSTR lpszAccy);

public:
	bool bIsAnalysisSuccess;
	ParamText Param[MAX_PARAMETER];
	ParamText uAnalysisFailParam;
	ParamText uAnalysisSuccessButNoSpecifiedParam;
	
protected:
	EAGLESYMBOLDATABASE DB;//search symbol
	DWORD dParamCnt;
};



