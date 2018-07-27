#include "stdafx.h"
#include "FileManage.h"
//_T("pds file(*.pds)\0*.pds\0pgs file(*.pgs)\0*.pgs\0All files(*.*)\0*.*\0"))
 //GetFileName(LPOPENFILENAME lpOutputOfn,LPCTSTR lpFileType)
BOOL GetFileName(LPCTSTR lpszFileType,LPTSTR lpszSelectPath,int _size_of_t)
{
	if(!lpszFileType||!lpszSelectPath||0==lpszFileType[0])
		return -1;
	OPENFILENAME opfn;
	TCHAR file_name[MAX_PATH]=_T("");
	ZeroMemory(&opfn,sizeof(OPENFILENAME));
	opfn.lStructSize=sizeof(OPENFILENAME);
	opfn.lpstrFilter=lpszFileType;
	opfn.nFilterIndex=1;
	opfn.lpstrFile=file_name;
	opfn.lpstrFile[0]='\0';
	opfn.nMaxFile=MAX_PATH;

	TCHAR szCurrentPath[MAX_PATH]=_T("");
	GetCurrentDirectory(MAX_PATH,szCurrentPath);
	opfn.lpstrInitialDir=szCurrentPath;

	opfn.Flags=OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST/*|OFN_NOCHANGEDIR*/;
	BOOL isFileSuccess=GetOpenFileName(&opfn);
	if(isFileSuccess)
	{
		_tcscpy_s(lpszSelectPath,_size_of_t,file_name);
		return 0;
	}
	return -1;
}
BOOL SaveFileName(LPOPENFILENAME lpOutputOfn,LPCTSTR lpFileName,LPCTSTR lpFileType)
{
	OPENFILENAME opfn={0};
	TCHAR file_name[MAX_PATH]=_T("");
	TCHAR lpszFilePath[MAX_PATH]=_T("");
	_tcscpy_s(file_name,MAX_PATH,lpFileName);
	ZeroMemory(&opfn,sizeof(OPENFILENAME));
	TCHAR szType[MAX_PATH];
	ZeroMemory(szType,MAX_PATH);
	TCHAR* lpszType=szType;
	int pathOffset=0;
	_stprintf(lpszType,lpFileType);//using _stprintf,_tcscat, _tcscpy, _tcscmp
	lpszType+=_tcslen(lpFileType);
	_stprintf(lpszType,_T(" files"));//using _stprintf,_tcscat, _tcscpy, _tcscmp
	lpszType+=_tcslen(_T(" files"))+1;
	_stprintf(lpszType,_T("*."));//using _stprintf,_tcscat, _tcscpy, _tcscmp
	lpszType+=_tcslen(_T("*."));
	_stprintf(lpszType,lpFileType);//using _stprintf,_tcscat, _tcscpy, _tcscmp
	lpszType+=_tcslen(lpFileType)+1;
	_stprintf(lpszType,_T("All files"));//using _stprintf,_tcscat, _tcscpy, _tcscmp
	lpszType+=_tcslen(_T("All files"))+1;
	_stprintf(lpszType,_T("*.*"));//using _stprintf,_tcscat, _tcscpy, _tcscmp
	opfn.lStructSize=sizeof(OPENFILENAME);
	opfn.lpstrFilter=szType;
	opfn.nFilterIndex=1;
	opfn.lpstrFileTitle=file_name;
	opfn.nMaxFileTitle=MAX_PATH;
	opfn.lpstrFile=lpszFilePath;
	opfn.lpstrFile[0]=0;
	opfn.nMaxFile=MAX_PATH;

	TCHAR szCurrentPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,szCurrentPath);
	opfn.lpstrInitialDir=szCurrentPath;

	opfn.Flags=OFN_SHOWHELP|OFN_OVERWRITEPROMPT;
	BOOL isFileSuccess=GetSaveFileName(&opfn);
	if(isFileSuccess)
	{
		*lpOutputOfn=opfn;
		return 1;
	}
	return 0;
}


//ErrorLog function include error location
void ErrorLog_Debug(LPCTSTR lpszFunction,LPCTSTR lpszFile,DWORD lLine) 
 { 
   TCHAR szBuf[1024]; 
   LPVOID lpMsgBuf;
   DWORD dw = GetLastError(); 

   FormatMessage(
     FORMAT_MESSAGE_ALLOCATE_BUFFER | 
     FORMAT_MESSAGE_FROM_SYSTEM,
     NULL,
     dw,
     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
       (LPTSTR) &lpMsgBuf,
       0, NULL);

   _stprintf_s(szBuf, 
	   _T("%s failed with error %d: %s\n\
File:%s\n\
Line:%d"), 
     lpszFunction, dw, lpMsgBuf,lpszFile,lLine-1); 

   MessageBox(NULL, szBuf,  _T("Error"), MB_OK); 

   LocalFree(lpMsgBuf);
 } 
//ErrorLog function not include error location
void ErrorLog_Release(LPCTSTR lpszFunction) 
 { 
   TCHAR szBuf[80]; 
   LPVOID lpMsgBuf;
   DWORD dw = GetLastError(); 

   FormatMessage(
     FORMAT_MESSAGE_ALLOCATE_BUFFER | 
     FORMAT_MESSAGE_FROM_SYSTEM,
     NULL,
     dw,
     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
       (LPTSTR) &lpMsgBuf,
       0, NULL);

   _stprintf_s(szBuf, 
     _T("%s failed with error %d: %s"), 
     lpszFunction, dw, lpMsgBuf); 

   MessageBox(NULL, szBuf,  _T("Error"), MB_OK); 

   LocalFree(lpMsgBuf);
 } 

errno_t fopenEX(FILE*& _File,const TCHAR * _Filename, const TCHAR * _Mode)
{
	errno_t  err=_tfopen_s(&_File,_Filename,_Mode);
	if(!_File)
		return err;
	TCHAR szLineBuf[4096]=_T("");
	TCHAR* lpszBuffer=0;
	lpszBuffer=_fgetts(szLineBuf,4096,_File);
	//check MS UTF-8
	if(0xFF==szLineBuf[0]&&0xFE==szLineBuf[1])
	{
		fclose(_File);
		_File=0;
		_stprintf_s(szLineBuf,4096,_T("%s,ccs=utf-8"),_Mode);
		err=_tfopen_s(&_File,_Filename,szLineBuf);
		if(!_File)
			return err;
	}
	return err;
}

TCHAR gszTextFileBuff[MAX_FILE_LINE][MAX_FILEOP_BUFF];
int LoadPathFileToLineBuffer(LPCTSTR lpszPath)
{
	FILE* pFile=0;
	if(_taccess_s(lpszPath,0))
		return -1;
	fopenEX(pFile,lpszPath,_T("r"));
	if(!pFile)
		return -1;
	int dwSrcLine=LoadFileToLineBuffer(pFile);
	fclose(pFile);
	return dwSrcLine;
}
int LoadFileToLineBuffer(FILE * pFile)
{
	if(!pFile)
		return -1;
	ZeroMemory(gszTextFileBuff,MAX_FILE_LINE*MAX_FILEOP_BUFF*sizeof(TCHAR));
	int dwSrcLine=0;
	fseek(pFile,0,SEEK_SET);
	LPTSTR lpRdStr=_fgetts(gszTextFileBuff[dwSrcLine],MAX_FILEOP_BUFF,pFile);
	while(lpRdStr)
	{
		lpRdStr=_fgetts(gszTextFileBuff[++dwSrcLine],MAX_FILEOP_BUFF,pFile);
	}
	fseek(pFile,0,SEEK_SET);
	return dwSrcLine;
}
//get function start/stop lines in file by function excute order
bool TraceFunction(FuncTrack* pFTstruct,CDataSheet* pDS,LPCTSTR lpszFuncStartTag,LPCTSTR lpszFuncStopTag,int nLine)
{
	if(!pFTstruct||!pDS||!lpszFuncStartTag||!lpszFuncStopTag)
		return false;
	TCHAR szFuncName[MAX_FUNCTION][MAX_TMP_STR_LENTH];
	ZeroMemory(szFuncName,sizeof(TCHAR)*MAX_FUNCTION*MAX_TMP_STR_LENTH);
	ZeroMemory(pFTstruct,sizeof(FuncTrack));
	for(int i=0;i<MAX_FUNCTION;i++)
	{
		TCHAR szBuff[MAX_TMP_STR_LENTH]=_T("");
		_tcscpy_s(szBuff,MAX_TMP_STR_LENTH,pDS->Function[i].Name);
		trimblank(szBuff);
		_stprintf_s(szFuncName[i],MAX_TMP_STR_LENTH,_T(" %s("),szBuff);
	}
	//find function order
	for(int i=0;i<nLine;i++)
	{
		for(int j=0;j<MAX_FUNCTION&&_T(')')!=szFuncName[j][0];j++)
		{
			if(0!=_tcsstr(gszTextFileBuff[i],szFuncName[j]))
			{
				if(isccmt(gszTextFileBuff[i]))
					continue;
				if(_tcsstr(gszTextFileBuff[i],lpszFuncStartTag))
				{
					pFTstruct->nFuncLineStart[j]=i++;
					pFTstruct->nFunctionCnt++;
					while(i<nLine&&!_tcsstr(gszTextFileBuff[i++],lpszFuncStopTag));
					pFTstruct->nFuncLineStop[j]=i-1;
				}
			}
		}
	}
	return true;
}

