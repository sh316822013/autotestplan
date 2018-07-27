#pragma once

typedef struct tagExcelCoord{
	DWORD row;
	DWORD col;
}ExcelCoord;
typedef struct tagTestplanFormat{
	DWORD pinCnt;
	ExcelCoord pProdInfo_start;
	ExcelCoord pProdInfo_end;
	
	ExcelCoord pFS[1024];
	ExcelCoord pFE[1024];
}TPF_all,*pTPF_all;

//global testplan column format
typedef struct tagTPF_col//TPF=="testplanformat"
{
	DWORD Function_num;//=1
	DWORD FT_Test_num;//=1

	DWORD WS_Test_num;//=2
	DWORD Test_type;//=3

	DWORD Function_name;//=4
	DWORD Test_name;//=4

	DWORD First_pin;//=5
	DWORD Last_pin;//=?

	DWORD RelayStat;//=?

	DWORD Typical_value;//=?+1
	DWORD AccuracyAndRange;//=?+2
	DWORD iLimitStart;//=?+3
	DWORD QA_limit;//=?+4
	DWORD iLimitStop;//=?+5
	DWORD Unit;//=?+6
	DWORD Comments;//=?+7

	DWORD _pincnt;
	DWORD _totalcolumn;
	DWORD _firstcolumn;
	DWORD _lastcolumn;
}TPF_col;






















