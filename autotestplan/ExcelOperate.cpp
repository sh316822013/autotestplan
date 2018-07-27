// ExcelOperate.cpp : implementation file
//

#include "stdafx.h"
#include "ExcelOperate.h"

//COperateExcel::COperateExcel()
//{
//
//}
//
void COperateExcel::Release()
{
	//this->range.DetachDispatch();
	this->range.ReleaseDispatch();

	//this->sheet.DetachDispatch();
	this->sheet.ReleaseDispatch();

	//this->sheets.DetachDispatch();
	this->sheets.ReleaseDispatch();

	//this->book.DetachDispatch();
	this->book.ReleaseDispatch();

	//this->books.DetachDispatch();
	this->books.ReleaseDispatch();

	//this->app.DetachDispatch();
	this->app.ReleaseDispatch();

	this->app.Quit();
}

DWORD COperateExcel::ChangeSheetName(DWORD wSheetNum,LPCTSTR lpszName)
{
	if(!wSheetNum||!lpszName)
		return -1;
	//change name of first Sheet page to TestSheet 
	this->sheet.AttachDispatch(this->sheets.GetItem(COleVariant((long)wSheetNum)),TRUE); 
	this->sheet.SetName(lpszName); 
	return 0;
}

void COperateExcel::Init()
{
	VARIANT vtMissing;
	vtMissing.vt=VT_NULL;
	//attach workbooks,workbook,worksheets
	this->books.AttachDispatch(this->app.GetWorkbooks(),TRUE); 
	this->book.AttachDispatch(this->books.Add((COleVariant)vtMissing),TRUE);//load EXCEL template 
	this->sheets.AttachDispatch(this->book.GetSheets(),TRUE);//load Sheet page 
	bIsInited=1;
}

void COperateExcel::SetVisible(BOOL TrueorFalse)
{
	this->app.SetVisible(TrueorFalse);
}

void COperateExcel::DeleteSheet(DWORD wSheet)
{
		this->sheet.AttachDispatch(this->sheets.GetItem(COleVariant((long)wSheet)),TRUE); 
		this->sheet.Delete(); 
}

void COperateExcel::SetRange(LPCTSTR lpszTopLeft,LPCTSTR lpszBottomRight)
{
	this->mCurrentRange=lpszTopLeft;
	this->mCurrentRange+=_T(",");
	this->mCurrentRange+=lpszBottomRight;
	this->range.AttachDispatch(this->sheet.GetRange(COleVariant(lpszTopLeft),COleVariant(lpszBottomRight)),TRUE); 
}

void COperateExcel::WriteToCell(DWORD x, DWORD y,COleVariant oleValue)
{
	this->range.SetItem(COleVariant((long)x),COleVariant((long)y),COleVariant(oleValue)); 
}
//Write st
void COperateExcel::WriteStringToCell(DWORD x, DWORD y,LPCTSTR lpszString)
{
	COleVariant value((LPCTSTR)(lpszString),VT_BSTR);
	this->WriteToCell(x,y,value);		
}

//special case 
void COperateExcel::WriteAnsiStringToCell(DWORD x, DWORD y,LPCSTR lpszString)
{
#ifdef _UNICODE
	USES_CONVERSION;
	COleVariant value((LPCTSTR)(A2W(lpszString)),VT_BSTR);
#else
	COleVariant value((LPCTSTR)(lpszString),VT_BSTR);
#endif
	this->WriteToCell(1,1,value);		
}


void COperateExcel::MergeCells(void)
{
	this->range.Merge(COleVariant((long)0)); 
}

//int coordinate xy convert to range point, for example: (1,1)->"A1"
CONST TCHAR i2cCoordTable[28]=_T(" ABCDEFGHIJKLMNOPQRSTUVWXYZ");
CString Int2Rng(DWORD row,DWORD col)
{
	CString strOut=_T("");
	if(row<1||col<1){
		throw (CString*)_T("attempt to pass invalid parameter to function \"CString Int2Rng(DWORD X,DWORD Y)\"");
		return  strOut;
	}
	strOut=_T("");
	col--;
	while(col>=26){
		int hiDecimal=col/26;
		col%=26;
		strOut+=i2cCoordTable[hiDecimal];	
	}
	col++;
	strOut+=i2cCoordTable[col];	
	CString tmpY;
	tmpY.Format(_T("%d"),row);
	strOut+=tmpY;
	return strOut;
}
CString Int2Rng(ExcelCoord p)
{
	DWORD row=p.row;
	DWORD col=p.col;
	if(row<1||col<1)
	{
		throw (CString*)_T("attempt to pass invalid parameter to function \"CString Int2Rng(DWORD X,DWORD Y)\"");
	}
	return Int2Rng(row,col);
	//CString strOut=_T("");
	//if(row<1||col<1){
	//	throw (CString*)_T("attempt to pass invalid parameter to function \"CString Int2Rng(DWORD X,DWORD Y)\"");
	//	return  strOut;
	//}
	//strOut=_T("");
	//while(col>26){
	//	int hiDecimal=col/26;
	//	col%=26;
	//	strOut+=i2cCoordTable[hiDecimal];	
	//}
	//strOut+=i2cCoordTable[col];	
	//CString tmpY;
	//tmpY.Format(_T("%d"),row);
	//strOut+=tmpY;
	//return strOut;
}