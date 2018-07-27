#pragma once
#include <commdlg.h>
#include <stdio.h>
#include "datasheetparser.h"
#define MAX_FILEOP_BUFF 512
#define MAX_FILE_LINE 200000

//relative path 
#define TestIndexIncreaseKeywordsFilePath _T("\\Config\\TestIncreaseKeywords.lua")

//global variable
extern TCHAR gszTextFileBuff[MAX_FILE_LINE][MAX_FILEOP_BUFF];
int LoadPathFileToLineBuffer(LPCTSTR lpszPath);
int LoadFileToLineBuffer(FILE * pFile);
//struct of function trace
typedef struct tagFuncTrack
{
	int nFuncLineStart[MAX_FUNCTION];
	int nFuncLineStop[MAX_FUNCTION];
	int nFunctionCnt;
}FuncTrack;
bool TraceFunction(FuncTrack* pFTstruct,CDataSheet* pDS,LPCTSTR lpszFuncStartTag,LPCTSTR lpszFuncStopTag,int nLine);

BOOL GetFileName(LPCTSTR lpszFileType,LPTSTR lpszSelectPath,int _size_of_t);
BOOL SaveFileName(LPOPENFILENAME lpOutputOfn,LPCTSTR lpFileName,LPCTSTR lpFileType);
errno_t fopenEX(FILE *& _File,const TCHAR * _Filename, const TCHAR * _Mode);


//useful error log function
void ErrorLog_Debug(LPCTSTR lpszFunction,LPCTSTR lpszFile,DWORD lLine);
void ErrorLog_Release(LPCTSTR lpszFunction);
//comment below macro to use ErrorLog_Release()
#define __DEBUG__
#ifdef __DEBUG__
#define ErrorLog(param) ErrorLog_Debug(param,_T(__FILE__),__LINE__)
#else
#define ErrorLog(param) ErrorLog_Release(param)
#endif

