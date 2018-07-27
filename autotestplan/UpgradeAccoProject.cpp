#include "stdafx.h"
#include "UpgradeAccoProject.h"
bool UpgradeAcco(const TCHAR* szPath,TCHAR* szErrorOut,int dwSizeOfErrType)
{
	if(!szPath||!szErrorOut)
	{
		_stprintf_s(szErrorOut,dwSizeOfErrType,_T("%s"),_T("invalid input buffer parameter"));
		return false;
	}
	TCHAR szSourcePath[MAX_PATH];
	zerocopystr(szSourcePath,MAX_PATH,szPath);
	trimrol(szSourcePath,_T("\\"));
	_tcscat(szSourcePath,_T("\\source\\test.cpp"));
	if(_taccess_s(szSourcePath,6))//rw permission
	{
		_stprintf_s(szErrorOut,dwSizeOfErrType,_T("%s"),_T("no write permission to source file"));
		return false;
	}
	//destiny folder
	TCHAR szProjPath[MAX_PATH];
	zerocopystr(szProjPath,MAX_PATH,szPath);
	trimrol(szProjPath,_T("\\"));
	//source file path
	TCHAR szSrcFilePath[MAX_PATH];
	TCHAR szSrcFilePKG[MAX_PATH];
	TCHAR szSrcFileBAT[MAX_PATH];
	GetCurrentEXEDirectory(szSrcFilePath,MAX_PATH);
	_tcscpy_s(szSrcFilePKG,MAX_PATH,szSrcFilePath);
	_tcscpy_s(szSrcFileBAT,MAX_PATH,szSrcFilePath);
	_tcscat(szSrcFilePKG,_T("\\tool\\ResourceMonitor.pkg"));
	_tcscat(szSrcFileBAT,_T("\\tool\\run.bat"));
	if(_taccess_s(szSrcFilePKG,0)||_taccess_s(szSrcFileBAT,0))//existance
	{
		_stprintf_s(szErrorOut,dwSizeOfErrType,_T("%s"),_T("file missing"));
		return false;
	}
	//execute copy cmd
	TCHAR szCmd[MAX_PATH*3];
	_stprintf_s(szCmd,MAX_PATH*3,_T("copy \"%s\"  \"%s\""),szSrcFilePKG,szProjPath);
	_tsystem(szCmd);
	_stprintf_s(szCmd,MAX_PATH*3,_T("copy \"%s\"  \"%s\""),szSrcFileBAT,szProjPath);
	_tsystem(szCmd);

	//analysis datasheet
	pPgsParser->init();
	pDataSheetParser->init();
	FILE* pDataSheetFile=0;
	_tfopen_s(&pDataSheetFile,szPath,_T("r"));
	if(!pDataSheetFile)
	{
		_stprintf_s(szErrorOut,dwSizeOfErrType,_T("%s"),_T("open datasheet file fail"));
		return false;
	}
	fclose(pDataSheetFile);
	if(pPgsParser->analysis(szPath))//0=parse success
	{
		_stprintf_s(szErrorOut,dwSizeOfErrType,_T("%s"),_T("analysis datasheet fail"));
		return false;
	}
	else
		pDataSheetParser->loadFromClass(pPgsParser,PROJTYPE_ACCO);
	fclose(pDataSheetFile);
	//start updating
	TCHAR szAccoCppFile[MAX_PATH];
	zerocopystr(szAccoCppFile,MAX_PATH,szPath);
	trimrol(szAccoCppFile,_T("\\"));
	_tcscat(szAccoCppFile,_T("\\source\\test.cpp"));
	if(_taccess_s(szAccoCppFile,6))//rw permission
	{
		_stprintf_s(szErrorOut,dwSizeOfErrType,_T("%s"),_T("invalid ACCO project"));
		return false;
	}
	//check source file content
	FILE* fpCppFile;
	TCHAR szLine[MAX_FILEOP_BUFF];
	LPTSTR lpszPtrLine=0;
	_tfopen_s(&fpCppFile,szAccoCppFile,_T("r+"));
	if(!fpCppFile)
	{
		_stprintf_s(szErrorOut,dwSizeOfErrType,_T("%s"),_T("open source file fail"));
		return false;
	}
	lpszPtrLine=_fgetts(szLine,MAX_FILEOP_BUFF,fpCppFile);
	while(lpszPtrLine)
	{
		if(_tcsstr(szLine,_T("RecordCondition("))||_tcsstr(szLine,_T("EnableRecord(")))
		{
			fclose(fpCppFile);
			_stprintf_s(szErrorOut,dwSizeOfErrType,_T("%s"),_T("program already added keywords"));
			return false;
		}
		if(_tcsstr(szLine,_T("++")))
		{
			//
			//tstring szBuf=szLine;
			//if(szBuf.find(_T(""))>=0)
			//	
		}
		lpszPtrLine=_fgetts(szLine,MAX_FILEOP_BUFF,fpCppFile);
	}
	fclose(fpCppFile);
	fpCppFile=0;
	//update test program
	FILE* fpTmpFile;
	TCHAR szTmpFileName[MAX_PATH];
	zerocopystr(szTmpFileName,MAX_PATH,szAccoCppFile);
	trimrol(szTmpFileName,_T("\\"));
	_tcscat(szTmpFileName,_T("\\tmp.tmp"));
	_tfopen_s(&fpTmpFile,szTmpFileName,_T("w+"));
	_tfopen_s(&fpCppFile,szAccoCppFile,_T("r+"));
	if(!fpCppFile||!fpTmpFile)
	{
		if(fpCppFile)
			fclose(fpCppFile);
		if(fpTmpFile)
			fclose(fpTmpFile);
		_stprintf_s(szErrorOut,dwSizeOfErrType,_T("%s"),_T("access violation before write cache"));
		return false;
	}
	//write to tmp.tmp
	lpszPtrLine=_fgetts(szLine,MAX_FILEOP_BUFF,fpCppFile);
	while(lpszPtrLine)
	{
		_fputts(szLine,fpTmpFile);
		lpszPtrLine=_fgetts(szLine,MAX_FILEOP_BUFF,fpCppFile);
	}
	fclose(fpTmpFile);
	fpTmpFile=0;
	fclose(fpCppFile);
	fpCppFile=0;
	//write back to source file
	_tfopen_s(&fpTmpFile,szTmpFileName,_T("r"));
	_tfopen_s(&fpCppFile,szAccoCppFile,_T("w+"));
	if(!fpCppFile||!fpTmpFile)
	{
		if(fpCppFile)
			fclose(fpCppFile);
		if(fpTmpFile)
			fclose(fpTmpFile);
		_stprintf_s(szErrorOut,dwSizeOfErrType,_T("%s"),_T("access violation when writeback cache"));
		return false;
	}
	lpszPtrLine=_fgetts(szLine,MAX_FILEOP_BUFF,fpTmpFile);
	//record current function
	TCHAR szCurFunction[MAX_FILEOP_BUFF];
	ZeroMemory(szCurFunction,MAX_FILEOP_BUFF*sizeof(TCHAR));
	while(lpszPtrLine)
	{
		if(_tcsstr(szLine,_T("DUT_API int"))&&_tcsstr(szLine,_T("(")))
		{
			if(!isccmt(szLine))
			{
				_tcscpy_s(szCurFunction,MAX_FILEOP_BUFF,szLine);
				trimlof(szCurFunction,_T("DUT_API int"));
				trimrof(szCurFunction,_T("("));
				trimblank(szCurFunction);
			}
		}
		_fputts(szLine,fpCppFile);
		//insert "EnableRecord() function"
		if(_tcsstr(szLine,_T("DUT_API int Init")))
		{
			if(!isccmt(szLine))
			{
				//find {
				while(!_tcsstr(szLine,_T("{"))&&lpszPtrLine)
					lpszPtrLine=_fgetts(szLine,MAX_FILEOP_BUFF,fpTmpFile);
				_fputts(szLine,fpCppFile);
				_fputts(_T("	EnableRecord();\n"),fpCppFile);
			}
		}
		////insert "SaveImage() function"
		//if(_tcsstr(szLine,_T("InitAfterTestFlow")))
		//{
		//	if(!isccmt(szLine))
		//	{
		//		//find {
		//		while(!_tcsstr(szLine,_T("{"))&&lpszPtrLine)
		//			lpszPtrLine=_fgetts(szLine,MAX_FILEOP_BUFF,fpTmpFile);
		//		_fputts(szLine,fpCppFile);
		//		_fputts(_T("	SaveImage();\n"),fpCppFile);
		//	}
		//}
		//insert "RecordCondition() function"
		if(_tcsstr(szLine,_T("groupgetresults")))
		{
			if(!isccmt(szLine))
			{
				TCHAR szWorking[MAX_FILEOP_BUFF];
				zerocopystr(szWorking,MAX_FILEOP_BUFF,szLine);
				trimlof(szWorking,_T(","));
				trimlof(szWorking,_T(","));
				trimrol(szWorking,_T(","));
				trimblank(szWorking);
				int dwIndex=0;
				int dwFuncnt=pPgsParser->getFuncCnt();
				for(;dwIndex<dwFuncnt;dwIndex++)
				{
					TCHAR szTmp[MAX_FILEOP_BUFF];
					zerocopystr(szTmp,MAX_FILEOP_BUFF,pPgsParser->Function[dwIndex].Name);
					trimblank(szTmp);
					if(!_tcscmp(szTmp,szCurFunction))
						break;
				}
				int dwParamcnt=pPgsParser->Function[dwIndex].TotalParam;
				for(int i=0;i<dwParamcnt;i++)
				{
					PARAM_STR curParam=pPgsParser->Function[dwIndex].Parameter[i];
					if(!_tcscmp(curParam.ParamName,szWorking))
					{
						TCHAR szCodeToAdd[MAX_PATH];
						_stprintf_s(szCodeToAdd,_T("	RecordCondition(\"%s\");\n"),curParam.ParamIndex);
						_fputts(szCodeToAdd,fpCppFile);
					}
				}
			}
		}
		lpszPtrLine=_fgetts(szLine,MAX_FILEOP_BUFF,fpTmpFile);
	}
	fclose(fpTmpFile);
	fpTmpFile=0;
	fclose(fpCppFile);
	fpCppFile=0;
	pPdsParser->init();
	pDataSheetParser->init();
	//create remote process to update project
	int dwExResult=RunCMD(szSrcFileBAT,0,100,SW_SHOWNORMAL);
	Sleep(1000);
	return true;
}
