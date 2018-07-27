#include "StdAfx.h"
#include "MyCommonString.h"

namespace myString
{
//check if string is comments
bool IsComment(tstring str)
{
	//TCHAR buff[MAX_STRINGBUFF_LEN];
	//TCHAR buff[MAX_STRINGBUFF_LEN];
	//str.copy(buff,MAX_STRINGBUFF_LEN);
	//str.copy(buff,MAX_STRINGBUFF_LEN);
	LPCTSTR szSpace=_T(" ");
	LPCTSTR szTab=_T("	");
	//1:trim blank and tab of beginning and ending 
	str.erase(0,str.find_first_not_of(szSpace));
	str.erase(str.find_last_not_of(szSpace) + 1);
	str.erase(0,str.find_first_not_of(szTab));
	str.erase(str.find_last_not_of(szTab) + 1);
	int pos=str.find(_T("//"));
	if(0==pos)
		return true;
	else
		return false;
}
//trim space and tab
void TrimSpaceAndTab(tstring& str)
{
	LPCTSTR szSpace=_T(" ");
	LPCTSTR szTab=_T("	");
	str.erase(0,str.find_first_not_of(szSpace));
	str.erase(str.find_last_not_of(szSpace) + 1);
	str.erase(0,str.find_first_not_of(szTab));
	str.erase(str.find_last_not_of(szTab) + 1);
}
//trim <=left of first finded symbol
void TrimLofFF(tstring& str,tstring symbol)
{
	int pos=str.find(symbol);
	if(-1!=pos)
		str.erase(0,pos+symbol.length());
}
//trim >=right of first finded symbol
void TrimRofFF(tstring& str,tstring symbol)
{
	int pos=str.find(symbol);
	if(-1!=pos)
		str.erase(pos);
}
//trim <=left of last finded symbol
void TrimLofLF(tstring& str,tstring symbol)
{
	int pos=str.find_last_of(symbol);
	if(-1!=pos)
		str.erase(0,pos+symbol.length());
}
//trim <=right of last finded symbol
void TrimRofLF(tstring& str,tstring symbol)
{
	int pos=str.find_last_of(symbol);
	if(-1!=pos)
		str.erase(pos);
}

//transform upper case to lower case
void StrToLowerCase(tstring& str)
{
	int length=str.length();
	if(0==length)
		return;
	LPTSTR lpszBuf=(LPTSTR)malloc((length+1/*for '\0'*/)*sizeof(TCHAR));
	_tcscpy_s(lpszBuf,length+1,str.c_str());
	_tcslwr_s(lpszBuf,length+1);
	str=lpszBuf;
	free(lpszBuf);
}


































}
