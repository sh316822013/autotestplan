#pragma once

#define MAX_SAVE_LINE 40

#define AI_RULE(rule) (rule)||
#define AI_RULE_END 0

//saved param content
typedef struct tagParamText{
	tstring ParamIndex;
	tstring Text[MAX_SAVE_LINE];
	tstring PinMark[4];//Result[0]:pin
	tstring PinMethod[4];//Result[0]:pin
	tstring Comment;//Result[0]:pin
	tstring Accuracy;//Result[0]:pin
	tstring PinType[4];//Result[0]:pin
	tstring PinStats[4];
	tstring RelayStat;
	int SrcLineIndex;
}ParamText;
//for distinction search
typedef struct tagAISyblAndMark
{
	tstring Symbl;
	BOOL Result;
	DWORD Line[MAX_SAVE_LINE];
	DWORD Pos[MAX_SAVE_LINE];
}AISyblAndMark;
class CSrcCodeParserInterface
{
public://initialize methid
	virtual void init(void)=0;
	virtual void initDataBase(BOOL type)=0;//init database 1:all,0:just init result
public://inner using method
	virtual DWORD analysis(FILE* pFile,CDataSheet* pDatasheetparser)=0;//after select pgs will excute
	virtual void SearchLine(tstring& str,int line)=0;
public://input&output method
	virtual int GetAccuracy(tstring& strOut,ParamText* pstrParamText)=0;
	virtual int AI_ParserParameter(CString& strIn,CString* cstrUnit)=0;
	virtual ParamText* getParamTextStructByIndex(CString& strindex)=0;
	virtual CString getParamText(CString& strIn)=0;
};

//fifo
class CTextFIFO
{
public:
	CTextFIFO(); 
	~CTextFIFO(); 
	void Init(void);
	void Insert(LPCTSTR lpszInput);
	DWORD GetOut(tstring out[MAX_SAVE_LINE]);
protected:
	tstring TEXT[MAX_SAVE_LINE];
	BYTE pIn,iCnt;
};
