#pragma once
//CpgsParser instance 
extern CpgsParser* pPgsParser;
//CpdsParser instance 
extern CpdsParser* pPdsParser;
//CDataSheet instance 
extern CDataSheet* pDataSheetParser;
//runcmd function
extern BOOL RunCMD(const TCHAR *lpcszCMD,const TCHAR* lpcszParam,DWORD dwMilliSeconds,int nShow);
//get current exe directory
extern DWORD GetCurrentEXEDirectory(LPTSTR lpszOutBuf,DWORD dwSize_of_t);

bool UpgradeAcco(const TCHAR* szPath,TCHAR* szErrorOut,int dwSizeOfErrType);
