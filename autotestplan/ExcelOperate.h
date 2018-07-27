#pragma once
#include"excel9.h"
#include "excel.h"
#include "TestplanFormat.h"
#define COLORINDEX_BLACK		1
#define COLORINDEX_WIHTE		2
#define COLORINDEX_RED			3
#define COLORINDEX_GREEN		4
#define COLORINDEX_BLUE			5
#define COLORINDEX_YELLOW	6
#define COLORINDEX_PINK			7
#define COLORINDEX_GREY		15
#define COLORINDEX_LIGHTYELLOW		36
#define COLORINDEX_ORANGE		45


class COperateExcel//:public Range,public _Worksheet, public Worksheets,public _Workbook,public Workbooks,public _Application
{
public:
//	COperateExcel();
	void Release();
public:
	_Application app;
	Range range;
	_Worksheet sheet;
	Worksheets sheets;
	_Workbook book;
	Workbooks books;

public:
	void Init();
	void DeleteSheet(DWORD wSheet);
	DWORD ChangeSheetName(DWORD wSheetNum,LPCTSTR lpszName);
	void SetVisible(BOOL TrueorFalse);
	void WriteToCell(DWORD x, DWORD y,COleVariant oleValue);
	void WriteStringToCell(DWORD x, DWORD y,LPCTSTR lpszString);
	void WriteAnsiStringToCell(DWORD x, DWORD y,LPCSTR lpszString);
	void SetRange(LPCTSTR lpszTopLeft,LPCTSTR lpszBottomRight);
	void MergeCells(void);

public:
	CString mCurrentRange;
protected:
	BOOL bIsInited;

};


CString Int2Rng(DWORD row,DWORD col);
CString Int2Rng(ExcelCoord p);
