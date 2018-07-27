#include "StdAfx.h"
#include "EagleSrcCodeParser.h"
#define MAX_ACCURACY_LENTH 1024
//create a TextFIFO
CTextFIFO* pcFIFOInst=new CTextFIFO;
//constructor
CEagleSrcCodeParser::CEagleSrcCodeParser(void)
{
	this->init();
}
//deconstructor
CEagleSrcCodeParser::~CEagleSrcCodeParser(void)
{
	this->init();
	delete(pcFIFOInst);
}

extern LPCTSTR tagParamNum;
//init method
void CEagleSrcCodeParser::init(void)
{
	this->bIsAnalysisSuccess=0;
	ZeroMemory(&this->Param,sizeof(ParamText)*MAX_PARAMETER);
	ZeroMemory(&this->DB,sizeof(EAGLESYMBOLDATABASE));
	this->dParamCnt=0;
	ZeroMemory(&this->uAnalysisFailParam,sizeof(ParamText));
	this->uAnalysisFailParam.Comment=_T("failed to analysis source code");
	ZeroMemory(&this->uAnalysisSuccessButNoSpecifiedParam,sizeof(ParamText));
	this->uAnalysisSuccessButNoSpecifiedParam.Comment=_T("no infomation found in source code");
}
#define lpszRelayTypeI2C320							_T("relays_IC(")
#define lpszRelayTypeI2C320_CLOSE			_T("K_CLOSE")
#define lpszRelayTypeI2C320_OPEN			_T("K_OPEN")
#define lpszRelayTypeI2C64							_T("relays(")
#define lpszRelayTypeI2C64_CLOSE			_T("K_CLOSE")
#define lpszRelayTypeI2C64_OPEN				_T("K_OPEN")
#define lpszRelayTypeEagleCbit_CLOSE		_T("cbitclose(")
#define lpszRelayTypeEagleCbit_OPEN		_T("cbitopen(")
#define lpszRelayTypeEagleCbit_END			_T(")")

DWORD CEagleSrcCodeParser::analysis(FILE* pFile,CDataSheet* pDatasheetparser)
{
	if(!pDatasheetparser->bIsInitSuccess)
		return 1;//failed
	//initialize FIFO
	pcFIFOInst->Init();
	//init AI database
	this->initDataBase(1);
	this->dParamCnt=0;
	TCHAR rcvBuf[MAX_FILEOP_BUFF];
	for(int i=0;i<MAX_PARAMETER;i++)//need optimize loop times
	{
		this->Param[i].ParamIndex.clear();
		for(short j=0;j<MAX_SAVE_LINE;j++)
		{
			this->Param[i].Text[j].clear();
		}
		for(int j=0;j<4;j++)
		{
			this->Param[i].PinMark[j].clear();
			this->Param[i].PinMethod[j].clear();
			this->Param[i].PinType[j].clear();
		}
		this->Param[i].Accuracy.clear();
		this->Param[i].Comment.clear();
	}
	int dwSrcLine=1;
	TCHAR buff[MAX_FILEOP_BUFF]=_T("");
	int nLine=LoadFileToLineBuffer(pFile);
	if(nLine<0)
		return 1;//failed
	FuncTrack uEagleCallOrder;
	!TraceFunction(&uEagleCallOrder,pDataSheetParser,_T("ETS_PRGFLOW_FUNC"),_T("END_ETS_PRGFLOW_FUNC"),nLine);
	tstring strRelayRecord=_T("");
	int debugpiont=0;
	//find in each function
	for(int j=0;j<uEagleCallOrder.nFunctionCnt;j++)
	{
		if(uEagleCallOrder.nFuncLineStart[j]>=uEagleCallOrder.nFuncLineStop[j]||0==uEagleCallOrder.nFuncLineStart[j]||0==uEagleCallOrder.nFuncLineStop[j])
			return 1;// fail return
		for(int k=uEagleCallOrder.nFuncLineStart[j];k<uEagleCallOrder.nFuncLineStop[j];k++)
		{
			if(_tcsstr(gszTextFileBuff[k],lpszRelayTypeI2C320))
			{
				_tcscpy_s(buff,MAX_FILEOP_BUFF,gszTextFileBuff[k]);
				if(!isccmt(buff))
				{
					//relays_IC(KHB5,K_CLOSE);
					if(_tcsstr(buff,lpszRelayTypeI2C320_CLOSE))
					{
						trimlof(buff,lpszRelayTypeI2C320);
						trimrof(buff,lpszRelayTypeI2C320_CLOSE);
						trimblank(buff); 
						trimrof(buff,_T(","));
						trimblank(buff); 
						_tcscat_s(buff,MAX_FILEOP_BUFF,_T(","));
						int pos=strRelayRecord.find(buff);
						if(pos==-1)
						{
							debugpiont++;
							strRelayRecord+=buff;
						}
					}
					//relays_IC(KHB5,K_OPEN);
					if(_tcsstr(buff,lpszRelayTypeI2C320_OPEN))
					{
						trimlof(buff,lpszRelayTypeI2C320);
						trimrof(buff,lpszRelayTypeI2C320_OPEN);
						trimblank(buff); 
						trimrof(buff,_T(","));
						trimblank(buff);
						_tcscat_s(buff,MAX_FILEOP_BUFF,_T(","));
						int pos=strRelayRecord.find(buff);
						if(pos!=-1)
						{
							strRelayRecord.erase(pos,_tcslen(buff));
							debugpiont++;
						}
					}
				}
			}
			//lpszRelayTypeI2C64
			if(_tcsstr(gszTextFileBuff[k],lpszRelayTypeI2C64))
			{
				_tcscpy_s(buff,MAX_FILEOP_BUFF,gszTextFileBuff[k]);
				if(!isccmt(buff))
				{
					//relays(K7,K_CLOSE);
					if(_tcsstr(buff,lpszRelayTypeI2C64_CLOSE))
					{
						trimlof(buff,lpszRelayTypeI2C64);
						trimrof(buff,lpszRelayTypeI2C64_CLOSE);
						trimblank(buff); 
						trimrof(buff,_T(","));
						trimblank(buff); 
						_tcscat_s(buff,MAX_FILEOP_BUFF,_T(","));
						int pos=strRelayRecord.find(buff);
						if(pos==-1)
						{
							debugpiont++;
							strRelayRecord+=buff;
						}
					}
					//relays(K7,K_CLOSE);
					if(_tcsstr(buff,lpszRelayTypeI2C64_OPEN))
					{
						trimlof(buff,lpszRelayTypeI2C64);
						trimrof(buff,lpszRelayTypeI2C64_OPEN);
						trimblank(buff); 
						trimrof(buff,_T(","));
						trimblank(buff);
						_tcscat_s(buff,MAX_FILEOP_BUFF,_T(","));
						int pos=strRelayRecord.find(buff);
						if(pos!=-1)
						{
							strRelayRecord.erase(pos,_tcslen(buff));
							debugpiont++;
						}
					}
				}
			}
			//lpszRelayTypeEagleCbit
			if(_tcsstr(gszTextFileBuff[k],lpszRelayTypeEagleCbit_CLOSE))
			{
				_tcscpy_s(buff,MAX_FILEOP_BUFF,gszTextFileBuff[k]);
				if(!isccmt(buff))
				{
					//cbitclose(HB_K7,HB_K8);
					if(_tcsstr(buff,lpszRelayTypeEagleCbit_CLOSE))
					{
						trimlof(buff,lpszRelayTypeEagleCbit_CLOSE);
						trimrof(buff,lpszRelayTypeEagleCbit_END);
						trimstr(buff,_T("site_"));
						trimstr(buff,_T("[site]"));
						trimstr(buff,_T(" "));
						trimstr(buff,_T("	"));
						tstring strTmpRelay=_T("");
						int nBufLen=_tcslen(buff);
						for(int x=0;x<nBufLen;x++)
						{
							if(buff[x]==_T(','))
								buff[x]=0;
						}
						int pos=strRelayRecord.find(buff);
						if(pos==-1)
						{
							debugpiont++;
							strRelayRecord+=buff;
							strRelayRecord+=_T(",");
						}
						for(int x=0;x<nBufLen;x++)
						{
							if(buff[x]==0)
							{
								strTmpRelay=&buff[x+1];
								int pos=strRelayRecord.find(strTmpRelay);
								if(pos==-1)
								{
									debugpiont++;
									strRelayRecord+=strTmpRelay;
									strRelayRecord+=_T(",");
								}
							}
						}
					}
				}
			}
			if(_tcsstr(gszTextFileBuff[k],lpszRelayTypeEagleCbit_OPEN))
			{
				_tcscpy_s(buff,MAX_FILEOP_BUFF,gszTextFileBuff[k]);
				if(!isccmt(buff))
				{
					//cbitopen(HB_K7);
					if(_tcsstr(buff,lpszRelayTypeEagleCbit_OPEN))
					{
						trimlof(buff,lpszRelayTypeEagleCbit_OPEN);
						trimrof(buff,lpszRelayTypeEagleCbit_END);
						trimstr(buff,_T("site_"));
						trimstr(buff,_T("[site]"));
						trimstr(buff,_T(" "));
						trimstr(buff,_T("	"));
						tstring strTmpRelay=_T("");
						int nBufLen=_tcslen(buff);
						for(int x=0;x<nBufLen;x++)
						{
							if(buff[x]==_T(','))
								buff[x]=0;
						}

						int pos=strRelayRecord.find(buff);
						if(pos!=-1)
						{
							debugpiont++;
							strRelayRecord.erase(pos,_tcslen(buff)+1);
						}
						for(int x=0;x<nBufLen;x++)
						{
							if(buff[x]==0)
							{
								int pos=strRelayRecord.find(&buff[x+1]);
								if(pos!=-1)
								{
									debugpiont++;
									strRelayRecord.erase(pos,_tcslen(&buff[x+1])+1);
								}
							}
						}
					}
				}
			}
			//insert fifo
			pcFIFOInst->Insert(gszTextFileBuff[k]);
			if(_tcsstr(gszTextFileBuff[k],DB.RecordCondition.Symbl.c_str()))
			{
				_tcscpy_s(buff,MAX_FILEOP_BUFF,gszTextFileBuff[k]);
				if(isccmt(buff))
					continue;
				trimlof(buff,_T("RecordCondition("));
				trimrol(buff,_T(")"));
				trimlof(buff,_T("\""));
				trimrof(buff,_T("\""));
				this->Param[this->dParamCnt].ParamIndex=buff;
				this->Param[this->dParamCnt].SrcLineIndex=dwSrcLine;
				this->Param[this->dParamCnt].RelayStat=strRelayRecord;
				pcFIFOInst->GetOut(this->Param[this->dParamCnt].Text);
				if(buff[0]!=0)
					this->dParamCnt++;
			}
		}
	}
	this->bIsAnalysisSuccess=1;
	return 0;
}
void CEagleSrcCodeParser::initDataBase(BOOL type=0)
{
	/*tpp-2*/
	ZeroMemory(&this->DB,sizeof(EAGLESYMBOLDATABASE));
	//general
	this->DB.DivisionSign.Symbl=_T("/");
	this->DB.RecordCondition.Symbl=_T("RecordCondition(");
	this->DB.Space.Symbl=_T(" ");
	this->DB.Tab.Symbl=_T("	");
	this->DB.Quote.Symbl=_T("&");
	this->DB.Equal.Symbl=_T("=");
	this->DB.Seperator.Symbl=_T(",");
	this->DB.LeftBracket.Symbl=_T("(");
	this->DB.RightBracket.Symbl=_T(")");

	//eagle used symbol
	//apuspu
	this->DB.mv.Symbl=_T("mv");
	this->DB.mi.Symbl=_T("mi");
	this->DB.apu12mv.Symbl=_T("apu12mv");
	this->DB.apu12mi.Symbl=_T("apu12mi");
	this->DB.sp100mv.Symbl=_T("sp100mv");
	this->DB.sp100mi.Symbl=_T("sp100mi");
	this->DB.sp112mv.Symbl=_T("sp112mv");
	this->DB.sp112mi.Symbl=_T("sp112mi");
	this->DB.sp500mv.Symbl=_T("sp500mv");
	this->DB.sp500mi.Symbl=_T("sp500mi");

	this->DB.apu12.Symbl=_T("apu12");
	this->DB.sp100.Symbl=_T("sp100");
	this->DB.sp112.Symbl=_T("sp112");
	this->DB.sp500.Symbl=_T("sp500");
	this->DB._MI_.Symbl=_T("_MI_");
	this->DB._MV_.Symbl=_T("_MV_");

	//timing
	this->DB.qtmumt.Symbl=_T("qtmumt");
	this->DB.QTMU_READTIME.Symbl=_T("QTMU_READTIME");
	this->DB.QTMU_READFREQ.Symbl=_T("QTMU_READFREQ");
	this->DB.QTMU_READCOUNTER.Symbl=_T("QTMU_READCOUNTER");
	this->DB.QTMU_READ_ELAPSEDTIME.Symbl=_T("QTMU_READ_ELAPSEDTIME");
	//awg
	this->DB.awgselect.Symbl=_T("awgselect");
	this->DB.adcmode.Symbl=_T("adcmode");
	this->DB.whereat.Symbl=_T("whereat");
	this->DB.mcurun.Symbl=_T("mcurun");

}
//search line
void CEagleSrcCodeParser::SearchLine(tstring& str,int line)
{
	int dPos;
//caculate
	//Equal
	dPos=-1;
	dPos=str.find(this->DB.Equal.Symbl);
	if(dPos>=0)
	{
		DB.Equal.Pos[DB.Equal.Result]=dPos;
		DB.Equal.Line[DB.Equal.Result++]=line;
	}
	//DivisionSign
	dPos=-1;
	dPos=str.find(this->DB.DivisionSign.Symbl);
	if(dPos>=0)
	{
		DB.DivisionSign.Pos[DB.DivisionSign.Result]=dPos;
		DB.DivisionSign.Line[DB.DivisionSign.Result++]=line;
	}
	//LeftBracket
	dPos=-1;
	dPos=str.find(this->DB.LeftBracket.Symbl);
	if(dPos>=0)
	{
		DB.LeftBracket.Pos[DB.LeftBracket.Result]=dPos;
		DB.LeftBracket.Line[DB.LeftBracket.Result++]=line;
	}
	//RightBracket
	dPos=-1;
	dPos=str.find(this->DB.RightBracket.Symbl);
	if(dPos>=0)
	{
		DB.RightBracket.Pos[DB.RightBracket.Result]=dPos;
		DB.RightBracket.Line[DB.RightBracket.Result++]=line;
	}
//other
	//Quote
	dPos=-1;
	dPos=str.find(this->DB.Quote.Symbl);
	if(dPos>=0)
	{
		DB.Quote.Pos[DB.Quote.Result]=dPos;
		DB.Quote.Line[DB.Quote.Result++]=line;
	}
	//RecordCondition
	dPos=-1;
	dPos=str.find(this->DB.RecordCondition.Symbl);
	if(dPos>=0)
	{
		DB.RecordCondition.Pos[DB.RecordCondition.Result]=dPos;
		DB.RecordCondition.Line[DB.RecordCondition.Result++]=line;
	}
//eagle used symbol
/*tpp-3*/
	//apu12mv
	dPos=-1;
	dPos=str.find(this->DB.apu12mv.Symbl);
	if(dPos>=0)
	{
		DB.apu12mv.Pos[DB.apu12mv.Result]=dPos;
		DB.apu12mv.Line[DB.apu12mv.Result++]=line;
	}
	//apu12mi
	dPos=-1;
	dPos=str.find(this->DB.apu12mi.Symbl);
	if(dPos>=0)
	{
		DB.apu12mi.Pos[DB.apu12mi.Result]=dPos;
		DB.apu12mi.Line[DB.apu12mi.Result++]=line;
	}
	//sp100mv
	dPos=-1;
	dPos=str.find(this->DB.sp100mv.Symbl);
	if(dPos>=0)
	{
		DB.sp100mv.Pos[DB.sp100mv.Result]=dPos;
		DB.sp100mv.Line[DB.sp100mv.Result++]=line;
	}
	//sp100mi
	dPos=-1;
	dPos=str.find(this->DB.sp100mi.Symbl);
	if(dPos>=0)
	{
		DB.sp100mi.Pos[DB.sp100mi.Result]=dPos;
		DB.sp100mi.Line[DB.sp100mi.Result++]=line;
	}
	//sp112mv
	dPos=-1;
	dPos=str.find(this->DB.sp112mv.Symbl);
	if(dPos>=0)
	{
		DB.sp112mv.Pos[DB.sp112mv.Result]=dPos;
		DB.sp112mv.Line[DB.sp112mv.Result++]=line;
	}
	//sp112mi
	dPos=-1;
	dPos=str.find(this->DB.sp112mi.Symbl);
	if(dPos>=0)
	{
		DB.sp112mi.Pos[DB.sp112mi.Result]=dPos;
		DB.sp112mi.Line[DB.sp112mi.Result++]=line;
	}
	//sp500mv
	dPos=-1;
	dPos=str.find(this->DB.sp500mv.Symbl);
	if(dPos>=0)
	{
		DB.sp500mv.Pos[DB.sp500mv.Result]=dPos;
		DB.sp500mv.Line[DB.sp500mv.Result++]=line;
	}
	//sp500mi
	dPos=-1;
	dPos=str.find(this->DB.sp500mi.Symbl);
	if(dPos>=0)
	{
		DB.sp500mi.Pos[DB.sp500mi.Result]=dPos;
		DB.sp500mi.Line[DB.sp500mi.Result++]=line;
	}
	//apu12
	dPos=-1;
	dPos=str.find(this->DB.apu12.Symbl);
	if(dPos>=0)
	{
		DB.apu12.Pos[DB.apu12.Result]=dPos;
		DB.apu12.Line[DB.apu12.Result++]=line;
	}
	//sp100
	dPos=-1;
	dPos=str.find(this->DB.sp100.Symbl);
	if(dPos>=0)
	{
		DB.sp100.Pos[DB.sp100.Result]=dPos;
		DB.sp100.Line[DB.sp100.Result++]=line;
	}
	//sp112
	dPos=-1;
	dPos=str.find(this->DB.sp112.Symbl);
	if(dPos>=0)
	{
		DB.sp112.Pos[DB.sp112.Result]=dPos;
		DB.sp112.Line[DB.sp112.Result++]=line;
	}
	//sp500
	dPos=-1;
	dPos=str.find(this->DB.sp500.Symbl);
	if(dPos>=0)
	{
		DB.sp500.Pos[DB.sp500.Result]=dPos;
		DB.sp500.Line[DB.sp500.Result++]=line;
	}
	//_MI_
	dPos=-1;
	dPos=str.find(this->DB._MI_.Symbl);
	if(dPos>=0)
	{
		DB._MI_.Pos[DB._MI_.Result]=dPos;
		DB._MI_.Line[DB._MI_.Result++]=line;
	}
	//_MV_
	dPos=-1;
	dPos=str.find(this->DB._MV_.Symbl);
	if(dPos>=0)
	{
		DB._MV_.Pos[DB._MV_.Result]=dPos;
		DB._MV_.Line[DB._MV_.Result++]=line;
	}
	//timing
	//qtmumt
	dPos=-1;
	dPos=str.find(this->DB.qtmumt.Symbl);
	if(dPos>=0)
	{
		DB.qtmumt.Pos[DB.qtmumt.Result]=dPos;
		DB.qtmumt.Line[DB.qtmumt.Result++]=line;
	}
	//QTMU_READTIME
	dPos=-1;
	dPos=str.find(this->DB.QTMU_READTIME.Symbl);
	if(dPos>=0)
	{
		DB.QTMU_READTIME.Pos[DB.QTMU_READTIME.Result]=dPos;
		DB.QTMU_READTIME.Line[DB.QTMU_READTIME.Result++]=line;
	}
	//QTMU_READFREQ
	dPos=-1;
	dPos=str.find(this->DB.QTMU_READFREQ.Symbl);
	if(dPos>=0)
	{
		DB.QTMU_READFREQ.Pos[DB.QTMU_READFREQ.Result]=dPos;
		DB.QTMU_READFREQ.Line[DB.QTMU_READFREQ.Result++]=line;
	}
	//QTMU_READCOUNTER
	dPos=-1;
	dPos=str.find(this->DB.QTMU_READCOUNTER.Symbl);
	if(dPos>=0)
	{
		DB.QTMU_READCOUNTER.Pos[DB.QTMU_READCOUNTER.Result]=dPos;
		DB.QTMU_READCOUNTER.Line[DB.QTMU_READCOUNTER.Result++]=line;
	}
	//QTMU_READ_ELAPSEDTIME
	dPos=-1;
	dPos=str.find(this->DB.QTMU_READ_ELAPSEDTIME.Symbl);
	if(dPos>=0)
	{
		DB.QTMU_READ_ELAPSEDTIME.Pos[DB.QTMU_READ_ELAPSEDTIME.Result]=dPos;
		DB.QTMU_READ_ELAPSEDTIME.Line[DB.QTMU_READ_ELAPSEDTIME.Result++]=line;
	}
	//awgselect
	dPos=-1;
	dPos=str.find(this->DB.awgselect.Symbl);
	if(dPos>=0)
	{
		DB.awgselect.Pos[DB.awgselect.Result]=dPos;
		DB.awgselect.Line[DB.awgselect.Result++]=line;
	}
	//adcmode
	dPos=-1;
	dPos=str.find(this->DB.adcmode.Symbl);
	if(dPos>=0)
	{
		DB.adcmode.Pos[DB.adcmode.Result]=dPos;
		DB.adcmode.Line[DB.adcmode.Result++]=line;
	}
	//whereat
	dPos=-1;
	dPos=str.find(this->DB.whereat.Symbl);
	if(dPos>=0)
	{
		DB.whereat.Pos[DB.whereat.Result]=dPos;
		DB.whereat.Line[DB.whereat.Result++]=line;
	}
	//mcurun
	dPos=-1;
	dPos=str.find(this->DB.mcurun.Symbl);
	if(dPos>=0)
	{
		DB.mcurun.Pos[DB.mcurun.Result]=dPos;
		DB.mcurun.Line[DB.mcurun.Result++]=line;
	}
}
void CEagleSrcCodeParser::FormatAccyString(LPTSTR pszOut,tstring& PinMark,tstring& PinMethod,LPCTSTR lpszRange,LPCTSTR lpszAccy)
{
	TCHAR szBuffer[MAX_ACCURACY_LENTH/2];
	ZeroMemory(szBuffer,MAX_ACCURACY_LENTH/2*sizeof(TCHAR));
	//verbose accuracy
	//_stprintf_s(szBuffer,MAX_ACCURACY_LENTH/2,_T("%s-%s-%s %s\n"),PinMark.c_str(),PinMethod.c_str(),lpszRange,lpszAccy);
	//brief accuracy
	_stprintf_s(szBuffer,MAX_ACCURACY_LENTH/2,_T("%s\n"),lpszAccy);
	_tcscat_s(pszOut,MAX_ACCURACY_LENTH,szBuffer);
}
//#define _ACCURACY_VERBOSE
#define _ACCURACY_BRIEF
#ifdef _ACCURACY_VERBOSE
//verbose accuracy
LPCTSTR ACCY_APU12_MV_3P6V=_T("accuracy:\n?0.45mV+0.025%xRdg)\nresolution:122uV");
LPCTSTR ACCY_APU12_MV_10V=_T("accuracy:\n?1.3mV+0.025%xRdg)\nresolution:305uV");
LPCTSTR ACCY_APU12_MV_30V=_T("accuracy:\n?4mV+0.025%xRdg)\nresolution:915uV");
LPCTSTR ACCY_APU12_MI1X_10UA=_T("accuracy:\n?8nA+0.05%xRdg+0.2nA/V)\nresolution:0.305nA");
LPCTSTR ACCY_APU12_MI1X_100UA=_T("accuracy:\n?20nA+0.05%xRdg+0.8nA/V)\nresolution:3.05nA");
LPCTSTR ACCY_APU12_MI1X_1MA=_T("accuracy:\n?125nA+0.05%xRdg+8nA/V)\nresolution:30.5nA");
LPCTSTR ACCY_APU12_MI1X_10MA=_T("accuracy:\n?1.25uA+0.05%xRdg+80nA/V)\nresolution:305nA");
LPCTSTR ACCY_APU12_MI1X_100MA=_T("accuracy:\n?12.5uA+0.05%xRdg+8uA/V)\nresolution:3.05uA");
LPCTSTR ACCY_APU12_MI1X_200MA=_T("accuracy:\n?25uA+0.05%xRdg+1.6uA/V)\nresolution:6.1uA");
LPCTSTR ACCY_APU12_MI10X_10UA=_T("invalid measure mode");
LPCTSTR ACCY_APU12_MI10X_100UA=_T("accuracy:\n?12nA+0.05%xRdg+0.4nA/V)\nresolution:0.305nA");
LPCTSTR ACCY_APU12_MI10X_1MA=_T("accuracy:\n?60nA+0.05%xRdg+4nA/V)\nresolution:3.05nA");
LPCTSTR ACCY_APU12_MI10X_10MA=_T("accuracy:\n?600uA+0.05%xRdg+40nA/V)\nresolution:30.5nA");
LPCTSTR ACCY_APU12_MI10X_100MA=_T("accuracy:\n?6uA+0.05%xRdg+0.4uA/V)\nresolution:3.05uA");
LPCTSTR ACCY_APU12_MI10X_200MA=_T("accuracy:\n?12uA+0.05%xRdg+0.8uA/V)\nresolution:6.1uA");
//sp100 10v range
LPCTSTR ACCY_SP100_MV1X_10V=_T("accuracy:\n?800uV+0.025%xRdg)\nresolution:300uV");
LPCTSTR ACCY_SP100_MV10X_10V=_T("accuracy:\n?600uV+0.025%xRdg)\nresolution:30uV");
LPCTSTR ACCY_SP100_MV100X_10V=_T("accuracy:\n?600uV+0.025%xRdg)\nresolution:3uV");
LPCTSTR ACCY_SP100_MI1X_2UA_10V=_T("accuracy:\n?12nA+0.05%xRdg)\nresolution:60pA");
LPCTSTR ACCY_SP100_MI10X_2UA_10V=_T("accuracy:\n?6nA+0.1%xRdg)\nresolution:6pA");
LPCTSTR ACCY_SP100_MI100X_2UA_10V=_T("accuracy:\n?6nA+0.1%xRdg)\nresolution:600fA");
LPCTSTR ACCY_SP100_MI1X_20UA_10V=_T("accuracy:\n?15nA+0.05%xRdg)\nresolution:600pA");
LPCTSTR ACCY_SP100_MI10X_20UA_10V=_T("accuracy:\n?10nA+0.1%xRdg)\nresolution:60pA");
LPCTSTR ACCY_SP100_MI100X_20UA_10V=_T("accuracy:\n?10nA+0.1%xRdg)\nresolution:6pA");
LPCTSTR ACCY_SP100_MI1X_200UA_10V=_T("accuracy:\n?25nA+0.05%xRdg)\nresolution:6nA");
LPCTSTR ACCY_SP100_MI10X_200UA_10V=_T("accuracy:\n?15nA+0.1%xRdg)\nresolution:600pA");
LPCTSTR ACCY_SP100_MI100X_200UA_10V=_T("accuracy:\n?15nA+0.1%xRdg)\nresolution:60pA");
LPCTSTR ACCY_SP100_MI1X_2MA_10V=_T("accuracy:\n?250nA+0.05%xRdg)\nresolution:60nA");
LPCTSTR ACCY_SP100_MI10X_2MA_10V=_T("accuracy:\n?125nA+0.05%xRdg)\nresolution:6nA");
LPCTSTR ACCY_SP100_MI100X_2MA_10V=_T("accuracy:\n?125nA+0.1%xRdg)\nresolution:600pA");
LPCTSTR ACCY_SP100_MI1X_20MA_10V=_T("accuracy:\n?2.5uA+0.05%xRdg)\nresolution:600nA");
LPCTSTR ACCY_SP100_MI10X_20MA_10V=_T("accuracy:\n?1.2uA+0.05%xRdg)\nresolution:60nA");
LPCTSTR ACCY_SP100_MI100X_20MA_10V=_T("accuracy:\n?1.2uA+0.05%xRdg)\nresolution:6nA");
LPCTSTR ACCY_SP100_MI1X_200MA_10V=_T("accuracy:\n?25uA+0.05%xRdg)\nresolution:6uA");
LPCTSTR ACCY_SP100_MI10X_200MA_10V=_T("accuracy:\n?12uA+0.05%xRdg)\nresolution:600nA");
LPCTSTR ACCY_SP100_MI100X_200MA_10V=_T("accuracy:\n?12uA+0.05%xRdg)\nresolution:60nA");
LPCTSTR ACCY_SP100_MI1X_2A_10V=_T("accuracy:\n?250uA+0.05%xRdg)\nresolution:60uA");
LPCTSTR ACCY_SP100_MI10X_2A_10V=_T("accuracy:\n?125uA+0.05%xRdg)\nresolution:6uA");
LPCTSTR ACCY_SP100_MI100X_2A_10V=_T("accuracy:\n?125uA+0.05%xRdg)\nresolution:600nA");
//sp100 30v range
LPCTSTR ACCY_SP100_MV1X_30V=_T("accuracy:\n?2.5mV+0.025%xRdg)\nresolution:900uV");
LPCTSTR ACCY_SP100_MV10X_30V=_T("accuracy:\n?2mV+0.025%xRdg)\nresolution:90uV");
LPCTSTR ACCY_SP100_MV100X_30V=_T("accuracy:\n?2mV+0.025%xRdg)\nresolution:9uV");
LPCTSTR ACCY_SP100_MI1X_1UA_30V=_T("accuracy:\n?8nA+0.05%xRdg)\nresolution:30pA");
LPCTSTR ACCY_SP100_MI10X_1UA_30V=_T("accuracy:\n?4nA+0.1%xRdg)\nresolution:3pA");
LPCTSTR ACCY_SP100_MI1X_10UA_30V=_T("accuracy:\n?12nA+0.05%xRdg)\nresolution:300pA");
LPCTSTR ACCY_SP100_MI10X_10UA_30V=_T("accuracy:\n?8nA+0.1%xRdg)\nresolution:30pA");
LPCTSTR ACCY_SP100_MI100X_10UA_30V=_T("accuracy:\n?8nA+0.1%xRdg)\nresolution:3pA");
LPCTSTR ACCY_SP100_MI1X_100UA_30V=_T("accuracy:\n?20nA+0.05%xRdg)\nresolution:3nA");
LPCTSTR ACCY_SP100_MI10X_100UA_30V=_T("accuracy:\n?12nA+0.1%xRdg)\nresolution:300pA");
LPCTSTR ACCY_SP100_MI100X_100UA_30V=_T("accuracy:\n?12nA+0.1%xRdg)\nresolution:30pA");
LPCTSTR ACCY_SP100_MI1X_1MA_30V=_T("accuracy:\n?125nA+0.05%xRdg)\nresolution:30nA");
LPCTSTR ACCY_SP100_MI10X_1MA_30V=_T("accuracy:\n?60nA+0.05%xRdg)\nresolution:3nA");
LPCTSTR ACCY_SP100_MI100X_1MA_30V=_T("accuracy:\n?60nA+0.1%xRdg)\nresolution:300pA");
LPCTSTR ACCY_SP100_MI1X_10MA_30V=_T("accuracy:\n?1.25uA+0.05%xRdg)\nresolution:300nA");
LPCTSTR ACCY_SP100_MI10X_10MA_30V=_T("accuracy:\n?600nA+0.05%xRdg)\nresolution:30nA");
LPCTSTR ACCY_SP100_MI100X_10MA_30V=_T("accuracy:\n?600nA+0.05%xRdg)\nresolution:3nA");
LPCTSTR ACCY_SP100_MI1X_100MA_30V=_T("accuracy:\n?12.5uA+0.05%xRdg)\nresolution:3uA");
LPCTSTR ACCY_SP100_MI10X_100MA_30V=_T("accuracy:\n?6uA+0.05%xRdg)\nresolution:300nA");
LPCTSTR ACCY_SP100_MI100X_100MA_30V=_T("accuracy:\n?6uA+0.05%xRdg)\nresolution:30nA");
LPCTSTR ACCY_SP100_MI1X_1A_30V=_T("accuracy:\n?125uA+0.05%xRdg)\nresolution:30uA");
LPCTSTR ACCY_SP100_MI10X_1A_30V=_T("accuracy:\n?60uA+0.05%xRdg)\nresolution:3uA");
LPCTSTR ACCY_SP100_MI100X_1A_30V=_T("accuracy:\n?60uA+0.05%xRdg)\nresolution:300nA");
//sp100 100v range
LPCTSTR ACCY_SP100_MV1X_100V=_T("accuracy:\n?12.5mV+0.025%xRdg)\nresolution:3mV");
LPCTSTR ACCY_SP100_MV10X_100V=_T("accuracy:\n?6mV+0.025%xRdg)\nresolution:300uV");
LPCTSTR ACCY_SP100_MV100X_100V=_T("accuracy:\n?6mV+0.025%xRdg)\nresolution:30uV");
LPCTSTR ACCY_SP100_MI1X_1UA_100V=_T("accuracy:\n?12.5nA+0.05%xRdg)\nresolution:30pA");
LPCTSTR ACCY_SP100_MI1X_10UA_100V=_T("accuracy:\n?25nA+0.05%xRdg)\nresolution:300pA");
LPCTSTR ACCY_SP100_MI10X_10UA_100V=_T("accuracy:\n?12.5nA+0.1%xRdg)\nresolution:30pA");
LPCTSTR ACCY_SP100_MI1X_100UA_100V=_T("accuracy:\n?50nA+0.05%xRdg)\nresolution:3nA");
LPCTSTR ACCY_SP100_MI10X_100UA_100V=_T("accuracy:\n?25nA+0.1%xRdg)\nresolution:300pA");
LPCTSTR ACCY_SP100_MI100X_100UA_100V=_T("accuracy:\n?25nA+0.1%xRdg)\nresolution:30pA");
LPCTSTR ACCY_SP100_MI1X_1MA_100V=_T("accuracy:\n?125nA+0.05%xRdg)\nresolution:300nA");
LPCTSTR ACCY_SP100_MI10X_1MA_100V=_T("accuracy:\n?60nA+0.05%xRdg)\nresolution:3nA");
LPCTSTR ACCY_SP100_MI100X_1MA_100V=_T("accuracy:\n?60nA+0.1%xRdg)\nresolution:300pA");
LPCTSTR ACCY_SP100_MI1X_10MA_100V=_T("accuracy:\n?1.25uA+0.05%xRdg)\nresolution:300nA");
LPCTSTR ACCY_SP100_MI10X_10MA_100V=_T("accuracy:\n?600nA+0.05%xRdg)\nresolution:30nA");
LPCTSTR ACCY_SP100_MI100X_10MA_100V=_T("accuracy:\n?600nA+0.05%xRdg)\nresolution:3nA");
LPCTSTR ACCY_SP100_MI1X_100MA_100V=_T("accuracy:\n?12.5uA+0.05%xRdg)\nresolution:3uA");
LPCTSTR ACCY_SP100_MI10X_100MA_100V=_T("accuracy:\n?6uA+0.05%xRdg)\nresolution:300nA");
LPCTSTR ACCY_SP100_MI100X_100MA_100V=_T("accuracy:\n?6uA+0.05%xRdg)\nresolution:30nA");
LPCTSTR ACCY_SP100_MI1X_500MA_100V=_T("accuracy:\n?125uA+0.05%xRdg)\nresolution:15uA");
LPCTSTR ACCY_SP100_MI10X_500MA_100V=_T("accuracy:\n?60uA+0.05%xRdg)\nresolution:1.5uA");
LPCTSTR ACCY_SP100_MI100X_500MA_100V=_T("accuracy:\n?60uA+0.05%xRdg)\nresolution:150nA");
//sp112 10v range
LPCTSTR ACCY_SP112_MI1X_12A_10V=_T("accuracy:\n?2mA+0.1%xRdg)\nresolution:300uA");
LPCTSTR ACCY_SP112_MI10X_12A_10V=_T("accuracy:\n?1mA+0.1%xRdg)\nresolution:30uA");
LPCTSTR ACCY_SP112_MI100X_12A_10V=_T("accuracy:\n?500uA+0.1%xRdg)\nresolution:3uA");
//sp112 30v range
LPCTSTR ACCY_SP112_MI1X_12A_30V=_T("accuracy:\n?2mA+0.1%xRdg)\nresolution:300uA");
LPCTSTR ACCY_SP112_MI10X_12A_30V=_T("accuracy:\n?1mA+0.1%xRdg)\nresolution:30uA");
LPCTSTR ACCY_SP112_MI100X_12A_30V=_T("accuracy:\n?500uA+0.1%xRdg)\nresolution:3uA");
//sp500 500v range
LPCTSTR ACCY_SP500_MV1X_500V=_T("accuracy:\n?60mV+0.05%xRdg)\nresolution:15mV");
LPCTSTR ACCY_SP500_MI1X_10UA=_T("accuracy:\n?30nA+0.1%xRdg)\nresolution:300pA");
LPCTSTR ACCY_SP500_MI10X_10UA=_T("accuracy:\n?20nA+0.1%xRdg)\nresolution:30pA");
LPCTSTR ACCY_SP500_MI100X_10UA=_T("accuracy:\n?20nA+0.2%xRdg)\nresolution:3pA");
LPCTSTR ACCY_SP500_MI1X_100UA=_T("accuracy:\n?50nA+0.05%xRdg)\nresolution:3nA");
LPCTSTR ACCY_SP500_MI10X_100UA=_T("accuracy:\n?30nA+0.1%xRdg)\nresolution:300pA");
LPCTSTR ACCY_SP500_MI100X_100UA=_T("accuracy:\n?20nA+0.1%xRdg)\nresolution:30pA");
LPCTSTR ACCY_SP500_MI1X_1MA=_T("accuracy:\n?250nA+0.05%xRdg)\nresolution:30nA");
LPCTSTR ACCY_SP500_MI10X_1MA=_T("accuracy:\n?80nA+0.05%xRdg)\nresolution:3nA");
LPCTSTR ACCY_SP500_MI100X_1MA=_T("accuracy:\n?60nA+0.1%xRdg)\nresolution:300pA");
LPCTSTR ACCY_SP500_MI1X_10MA=_T("accuracy:\n?1.25uA+0.05%xRdg)\nresolution:300nA");
LPCTSTR ACCY_SP500_MI10X_10MA=_T("accuracy:\n?250nA+0.05%xRdg)\nresolution:30nA");
LPCTSTR ACCY_SP500_MI100X_10MA=_T("accuracy:\n?125nA+0.1%xRdg)\nresolution:3nA");
LPCTSTR ACCY_SP500_MI1X_50MA=_T("accuracy:\n?6uA+0.05%xRdg)\nresolution:1.5uA");
LPCTSTR ACCY_SP500_MI10X_50MA=_T("accuracy:\n?2.5uA+0.05%xRdg)\nresolution:150nA");
LPCTSTR ACCY_SP500_MI100X_50MA=_T("accuracy:\n?550nA+0.1%xRdg)\nresolution:15nA");
#else
//brief accuracy
LPCTSTR ACCY_APU12_MV_3P6V=												_T("?.45mV");
LPCTSTR ACCY_APU12_MV_10V=													_T("?.3mV");
LPCTSTR ACCY_APU12_MV_30V=													_T("?mV");
LPCTSTR ACCY_APU12_MI1X_10UA=											_T("?nA");
LPCTSTR ACCY_APU12_MI1X_100UA=											_T("?0nA");
LPCTSTR ACCY_APU12_MI1X_1MA=												_T("?25nA");
LPCTSTR ACCY_APU12_MI1X_10MA=											_T("?.25uA");
LPCTSTR ACCY_APU12_MI1X_100MA=											_T("?2.5uA");
LPCTSTR ACCY_APU12_MI1X_200MA=											_T("?5uA");
LPCTSTR ACCY_APU12_MI10X_10UA=											_T("invalid measure mode");
LPCTSTR ACCY_APU12_MI10X_100UA=										_T("?2nA");
LPCTSTR ACCY_APU12_MI10X_1MA=											_T("?0nA");
LPCTSTR ACCY_APU12_MI10X_10MA=											_T("?00uA");
LPCTSTR ACCY_APU12_MI10X_100MA=										_T("?uA");
LPCTSTR ACCY_APU12_MI10X_200MA=										_T("?2uA");
//sp100 10v range
LPCTSTR ACCY_SP100_MV1X_10V=												_T("?00uV");
LPCTSTR ACCY_SP100_MV10X_10V=											_T("?00uV");
LPCTSTR ACCY_SP100_MV100X_10V=											_T("?00uV");
LPCTSTR ACCY_SP100_MI1X_2UA_10V=										_T("?2nA");
LPCTSTR ACCY_SP100_MI10X_2UA_10V=									_T("?nA");
LPCTSTR ACCY_SP100_MI100X_2UA_10V=									_T("?nA");
LPCTSTR ACCY_SP100_MI1X_20UA_10V=									_T("?5nA");
LPCTSTR ACCY_SP100_MI10X_20UA_10V=									_T("?0nA");
LPCTSTR ACCY_SP100_MI100X_20UA_10V=								_T("?0nA");
LPCTSTR ACCY_SP100_MI1X_200UA_10V=									_T("?5nA");
LPCTSTR ACCY_SP100_MI10X_200UA_10V=								_T("?5nA");
LPCTSTR ACCY_SP100_MI100X_200UA_10V=							_T("?5nA");
LPCTSTR ACCY_SP100_MI1X_2MA_10V=										_T("?50nA");
LPCTSTR ACCY_SP100_MI10X_2MA_10V=									_T("?25nA");
LPCTSTR ACCY_SP100_MI100X_2MA_10V=								_T("?25nA");
LPCTSTR ACCY_SP100_MI1X_20MA_10V=									_T("?.5uA");
LPCTSTR ACCY_SP100_MI10X_20MA_10V=								_T("?.2uA");
LPCTSTR ACCY_SP100_MI100X_20MA_10V=								_T("?.2uA");
LPCTSTR ACCY_SP100_MI1X_200MA_10V=								_T("?5uA");
LPCTSTR ACCY_SP100_MI10X_200MA_10V=								_T("?2uA");
LPCTSTR ACCY_SP100_MI100X_200MA_10V=							_T("?2uA");
LPCTSTR ACCY_SP100_MI1X_2A_10V=										_T("?50uA");
LPCTSTR ACCY_SP100_MI10X_2A_10V=										_T("?25uA");
LPCTSTR ACCY_SP100_MI100X_2A_10V=									_T("?25uA");
//sp100 30v range
LPCTSTR ACCY_SP100_MV1X_30V=												_T("?.5mV");
LPCTSTR ACCY_SP100_MV10X_30V=											_T("?mV");
LPCTSTR ACCY_SP100_MV100X_30V=											_T("?mV");
LPCTSTR ACCY_SP100_MI1X_1UA_30V=										_T("?nA");
LPCTSTR ACCY_SP100_MI10X_1UA_30V=									_T("?nA");
LPCTSTR ACCY_SP100_MI1X_10UA_30V=									_T("?2nA");
LPCTSTR ACCY_SP100_MI10X_10UA_30V=									_T("?nA");
LPCTSTR ACCY_SP100_MI100X_10UA_30V=								_T("?nA");
LPCTSTR ACCY_SP100_MI1X_100UA_30V=									_T("?0nA");
LPCTSTR ACCY_SP100_MI10X_100UA_30V=								_T("?2nA");
LPCTSTR ACCY_SP100_MI100X_100UA_30V=							_T("?2nA");
LPCTSTR ACCY_SP100_MI1X_1MA_30V=										_T("?25nA");
LPCTSTR ACCY_SP100_MI10X_1MA_30V=									_T("?0nA");
LPCTSTR ACCY_SP100_MI100X_1MA_30V=								_T("?0nA");
LPCTSTR ACCY_SP100_MI1X_10MA_30V=									_T("?.25uA");
LPCTSTR ACCY_SP100_MI10X_10MA_30V=								_T("?00nA");
LPCTSTR ACCY_SP100_MI100X_10MA_30V=								_T("?00nA");
LPCTSTR ACCY_SP100_MI1X_100MA_30V=								_T("?2.5uA");
LPCTSTR ACCY_SP100_MI10X_100MA_30V=								_T("?uA");
LPCTSTR ACCY_SP100_MI100X_100MA_30V=							_T("?uA");
LPCTSTR ACCY_SP100_MI1X_1A_30V=										_T("?25uA");
LPCTSTR ACCY_SP100_MI10X_1A_30V=										_T("?0uA");
LPCTSTR ACCY_SP100_MI100X_1A_30V=									_T("?0uA");
//sp100 100v range
LPCTSTR ACCY_SP100_MV1X_100V=											_T("?2.5mV");
LPCTSTR ACCY_SP100_MV10X_100V=											_T("?mV");
LPCTSTR ACCY_SP100_MV100X_100V=										_T("?mV");
LPCTSTR ACCY_SP100_MI1X_1UA_100V=									_T("?2.5nA");
LPCTSTR ACCY_SP100_MI1X_10UA_100V=									_T("?5nA");
LPCTSTR ACCY_SP100_MI10X_10UA_100V=								_T("?2.5nA");
LPCTSTR ACCY_SP100_MI1X_100UA_100V=								_T("?0nA");
LPCTSTR ACCY_SP100_MI10X_100UA_100V=							_T("?5nA");
LPCTSTR ACCY_SP100_MI100X_100UA_100V=							_T("?5nA");
LPCTSTR ACCY_SP100_MI1X_1MA_100V=									_T("?25nA");
LPCTSTR ACCY_SP100_MI10X_1MA_100V=								_T("?0nA");
LPCTSTR ACCY_SP100_MI100X_1MA_100V=								_T("?0nA");
LPCTSTR ACCY_SP100_MI1X_10MA_100V=								_T("?.25uA");
LPCTSTR ACCY_SP100_MI10X_10MA_100V=								_T("?00nA");
LPCTSTR ACCY_SP100_MI100X_10MA_100V=							_T("?00nA");
LPCTSTR ACCY_SP100_MI1X_100MA_100V=								_T("?2.5uA");
LPCTSTR ACCY_SP100_MI10X_100MA_100V=							_T("?uA");
LPCTSTR ACCY_SP100_MI100X_100MA_100V=							_T("?uA");
LPCTSTR ACCY_SP100_MI1X_500MA_100V=								_T("?25uA");
LPCTSTR ACCY_SP100_MI10X_500MA_100V=							_T("?0uA");
LPCTSTR ACCY_SP100_MI100X_500MA_100V=							_T("?0uA");
//sp112 10v range
LPCTSTR ACCY_SP112_MI1X_12A_10V=										_T("?mA");
LPCTSTR ACCY_SP112_MI10X_12A_10V=									_T("?mA");
LPCTSTR ACCY_SP112_MI100X_12A_10V=									_T("?00uA");
//sp112 30v range
LPCTSTR ACCY_SP112_MI1X_12A_30V=										_T("?mA");
LPCTSTR ACCY_SP112_MI10X_12A_30V=									_T("?mA");
LPCTSTR ACCY_SP112_MI100X_12A_30V=									_T("?00uA");
//sp500 500v range
LPCTSTR ACCY_SP500_MV1X_500V=											_T("?0mV");
LPCTSTR ACCY_SP500_MI1X_10UA=											_T("?0nA");
LPCTSTR ACCY_SP500_MI10X_10UA=											_T("?0nA");
LPCTSTR ACCY_SP500_MI100X_10UA=										_T("?0nA");
LPCTSTR ACCY_SP500_MI1X_100UA=											_T("?0nA");
LPCTSTR ACCY_SP500_MI10X_100UA=										_T("?0nA");
LPCTSTR ACCY_SP500_MI100X_100UA=										_T("?0nA");
LPCTSTR ACCY_SP500_MI1X_1MA=												_T("?50nA");
LPCTSTR ACCY_SP500_MI10X_1MA=											_T("?0nA");
LPCTSTR ACCY_SP500_MI100X_1MA=											_T("?0nA");
LPCTSTR ACCY_SP500_MI1X_10MA=											_T("?.25uA");
LPCTSTR ACCY_SP500_MI10X_10MA=											_T("?50nA");
LPCTSTR ACCY_SP500_MI100X_10MA=										_T("?25nA");
LPCTSTR ACCY_SP500_MI1X_50MA=											_T("?uA");
LPCTSTR ACCY_SP500_MI10X_50MA=											_T("?.5uA");
LPCTSTR ACCY_SP500_MI100X_50MA=										_T("?50nA");
#endif
int CEagleSrcCodeParser::GetAccuracy(tstring& strOut,ParamText* pstrParamText)
{
	TCHAR szAccuracy[MAX_ACCURACY_LENTH];
	TCHAR szRange[4][MAX_TMP_STR_LENTH];
	TCHAR szVRange[4][MAX_TMP_STR_LENTH];
	TCHAR szIRange[4][MAX_TMP_STR_LENTH];
	ZeroMemory(szAccuracy,sizeof(TCHAR)*MAX_ACCURACY_LENTH);
	ZeroMemory(szRange,sizeof(TCHAR)*MAX_TMP_STR_LENTH);
	ZeroMemory(szVRange,sizeof(TCHAR)*MAX_TMP_STR_LENTH);
	ZeroMemory(szIRange,sizeof(TCHAR)*MAX_TMP_STR_LENTH);
	for(int i=0;i<4;i++)
	{
		_tcscpy_s(szRange[i],MAX_TMP_STR_LENTH,pstrParamText->PinStats[i].c_str());
		s2lower(szRange[i]);
		_tcscpy_s(szVRange[i],MAX_TMP_STR_LENTH,szRange[i]);
		_tcscpy_s(szIRange[i],MAX_TMP_STR_LENTH,szRange[i]);
		trimrof(szVRange[i],_T("/"));
		trimlof(szIRange[i],_T("/"));
		trimrof(szIRange[i],_T("\n"));
	}
	//if pin method="mv/mi1x" need seperate it
	TCHAR szPinMthdBkup[MAX_TMP_STR_LENTH];
	ZeroMemory(szPinMthdBkup,sizeof(TCHAR)*MAX_TMP_STR_LENTH);
	//tstring strPinMethodBackup;
	if(pstrParamText->PinMethod[0].find(_T("/"))!=-1&&pstrParamText->PinMark[1].empty())
	{
		_tcscpy_s(szPinMthdBkup,MAX_TMP_STR_LENTH,pstrParamText->PinMethod[0].c_str());
		TCHAR szMethod0[MAX_TMP_STR_LENTH];
		TCHAR szMethod1[MAX_TMP_STR_LENTH];
		ZeroMemory(szMethod0,sizeof(TCHAR)*MAX_TMP_STR_LENTH);
		ZeroMemory(szMethod1,sizeof(TCHAR)*MAX_TMP_STR_LENTH);
		_tcscpy_s(szMethod0,MAX_TMP_STR_LENTH,szPinMthdBkup);
		_tcscpy_s(szMethod1,MAX_TMP_STR_LENTH,szPinMthdBkup);
		pstrParamText->PinMark[1]=pstrParamText->PinMark[0];
		trimrof(szMethod0,_T("/"));
		pstrParamText->PinMethod[0]=szMethod0;
		trimlof(szMethod1,_T("/"));
		pstrParamText->PinMethod[1]=szMethod1;
		pstrParamText->PinType[1]=pstrParamText->PinType[0];
		_tcscpy_s(szIRange[1],MAX_TMP_STR_LENTH,szIRange[0]);
	}
	//find accuracy
	for(int i=0;!pstrParamText->PinMark[i].empty()&&i<4;i++)
	{
		//apu12
		if(this->DB.apu12.Symbl==pstrParamText->PinType[i])
		{
			if(pstrParamText->PinMethod[i]==_T("MV"))
			{
				if(0==_tcscmp(szVRange[i],_T("3.6v")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szVRange[i],ACCY_APU12_MV_3P6V);
				if(0==_tcscmp(szVRange[i],_T("10v")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szVRange[i],ACCY_APU12_MV_10V);
				if(0==_tcscmp(szVRange[i],_T("30v")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szVRange[i],ACCY_APU12_MV_30V);
			}
			if(pstrParamText->PinMethod[i]==_T("MI1X"))
			{
				if(0==_tcscmp(szIRange[i],_T("10ua")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_APU12_MI1X_10UA);
				if(0==_tcscmp(szIRange[i],_T("100ua")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_APU12_MI1X_100UA);
				if(0==_tcscmp(szIRange[i],_T("1ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_APU12_MI1X_1MA);
				if(0==_tcscmp(szIRange[i],_T("10ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_APU12_MI1X_10MA);
				if(0==_tcscmp(szIRange[i],_T("100ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_APU12_MI1X_100MA);
				if(0==_tcscmp(szIRange[i],_T("200ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_APU12_MI1X_200MA);
			}
			if(pstrParamText->PinMethod[i]==_T("MI10X"))
			{
				if(0==_tcscmp(szIRange[i],_T("10ua")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_APU12_MI10X_10UA);
				if(0==_tcscmp(szIRange[i],_T("100ua")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_APU12_MI10X_100UA);
				if(0==_tcscmp(szIRange[i],_T("1ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_APU12_MI10X_1MA);
				if(0==_tcscmp(szIRange[i],_T("10ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_APU12_MI10X_10MA);
				if(0==_tcscmp(szIRange[i],_T("100ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_APU12_MI10X_100MA);
				if(0==_tcscmp(szIRange[i],_T("200ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_APU12_MI10X_200MA);
			}
		}
		if(this->DB.sp100.Symbl==pstrParamText->PinType[i]||this->DB.sp112.Symbl==pstrParamText->PinType[i])
		{
			if(0==_tcscmp(szVRange[i],_T("10v")))
			{
				if(pstrParamText->PinMethod[i]==_T("MV1X"))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szVRange[i],ACCY_SP100_MV1X_10V);
				if(pstrParamText->PinMethod[i]==_T("MV10X"))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szVRange[i],ACCY_SP100_MV10X_10V);
				if(pstrParamText->PinMethod[i]==_T("MV100X"))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szVRange[i],ACCY_SP100_MV100X_10V);
				if(pstrParamText->PinMethod[i]==_T("MI1X"))
				{
					if(0==_tcscmp(szIRange[i],_T("2ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_2UA_10V);
					if(0==_tcscmp(szIRange[i],_T("20ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_20UA_10V);
					if(0==_tcscmp(szIRange[i],_T("200ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_200UA_10V);
					if(0==_tcscmp(szIRange[i],_T("2ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_2MA_10V);
					if(0==_tcscmp(szIRange[i],_T("20ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_20MA_10V);
					if(0==_tcscmp(szIRange[i],_T("200ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_200MA_10V);
					if(0==_tcscmp(szIRange[i],_T("2a")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_2A_10V);
					if(0==_tcscmp(szIRange[i],_T("12a")))//special for SP112
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP112_MI1X_12A_10V);
				}
				if(pstrParamText->PinMethod[i]==_T("MI10X"))
				{
					if(0==_tcscmp(szIRange[i],_T("2ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_2UA_10V);
					if(0==_tcscmp(szIRange[i],_T("20ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_20UA_10V);
					if(0==_tcscmp(szIRange[i],_T("200ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_200UA_10V);
					if(0==_tcscmp(szIRange[i],_T("2ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_2MA_10V);
					if(0==_tcscmp(szIRange[i],_T("20ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_20MA_10V);
					if(0==_tcscmp(szIRange[i],_T("200ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_200MA_10V);
					if(0==_tcscmp(szIRange[i],_T("2a")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_2A_10V);
					if(0==_tcscmp(szIRange[i],_T("12a")))//special for SP112
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP112_MI10X_12A_10V);
				}
				if(pstrParamText->PinMethod[i]==_T("MI100X"))
				{
					if(0==_tcscmp(szIRange[i],_T("2ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_2UA_10V);
					if(0==_tcscmp(szIRange[i],_T("20ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_20UA_10V);
					if(0==_tcscmp(szIRange[i],_T("200ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_200UA_10V);
					if(0==_tcscmp(szIRange[i],_T("2ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_2MA_10V);
					if(0==_tcscmp(szIRange[i],_T("20ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_20MA_10V);
					if(0==_tcscmp(szIRange[i],_T("200ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_200MA_10V);
					if(0==_tcscmp(szIRange[i],_T("2a")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_2A_10V);
					if(0==_tcscmp(szIRange[i],_T("12a")))//Special for sp112
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP112_MI100X_12A_10V);
				}
			}
			if(0==_tcscmp(szVRange[i],_T("30v")))
			{
				if(pstrParamText->PinMethod[i]==_T("MV1X"))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szVRange[i],ACCY_SP100_MV1X_30V);
				if(pstrParamText->PinMethod[i]==_T("MV10X"))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szVRange[i],ACCY_SP100_MV10X_30V);
				if(pstrParamText->PinMethod[i]==_T("MV100X"))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szVRange[i],ACCY_SP100_MV100X_30V);
				if(pstrParamText->PinMethod[i]==_T("MI1X"))
				{
					if(0==_tcscmp(szIRange[i],_T("1ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_1UA_30V);
					if(0==_tcscmp(szIRange[i],_T("10ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_10UA_30V);
					if(0==_tcscmp(szIRange[i],_T("100ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_100UA_30V);
					if(0==_tcscmp(szIRange[i],_T("1ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_1MA_30V);
					if(0==_tcscmp(szIRange[i],_T("10ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_10MA_30V);
					if(0==_tcscmp(szIRange[i],_T("100ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_100MA_30V);
					if(0==_tcscmp(szIRange[i],_T("1a")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_1A_30V);
					if(0==_tcscmp(szIRange[i],_T("12a")))//Special for sp112
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP112_MI1X_12A_30V);
				}
				if(pstrParamText->PinMethod[i]==_T("MI10X"))
				{
					if(0==_tcscmp(szIRange[i],_T("1ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_1UA_30V);
					if(0==_tcscmp(szIRange[i],_T("10ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_10UA_30V);
					if(0==_tcscmp(szIRange[i],_T("100ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_100UA_30V);
					if(0==_tcscmp(szIRange[i],_T("1ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_1MA_30V);
					if(0==_tcscmp(szIRange[i],_T("10ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_10MA_30V);
					if(0==_tcscmp(szIRange[i],_T("100ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_100MA_30V);
					if(0==_tcscmp(szIRange[i],_T("1a")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_1A_30V);
					if(0==_tcscmp(szIRange[i],_T("12a")))//Special for sp112
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP112_MI10X_12A_30V);
				}
				if(pstrParamText->PinMethod[i]==_T("MI100X"))
				{
					if(0==_tcscmp(szIRange[i],_T("10ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_10UA_30V);
					if(0==_tcscmp(szIRange[i],_T("100ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_100UA_30V);
					if(0==_tcscmp(szIRange[i],_T("1ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_1MA_30V);
					if(0==_tcscmp(szIRange[i],_T("10ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_10MA_30V);
					if(0==_tcscmp(szIRange[i],_T("100ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_100MA_30V);
					if(0==_tcscmp(szIRange[i],_T("1a")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_1A_30V);
					if(0==_tcscmp(szIRange[i],_T("12a")))//Special for sp112
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP112_MI100X_12A_30V);
				}
			}
			if(0==_tcscmp(szVRange[i],_T("100v")))
			{
				if(pstrParamText->PinMethod[i]==_T("MV1X"))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szVRange[i],ACCY_SP100_MV1X_100V);
				if(pstrParamText->PinMethod[i]==_T("MV10X"))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szVRange[i],ACCY_SP100_MV10X_100V);
				if(pstrParamText->PinMethod[i]==_T("MV100X"))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szVRange[i],ACCY_SP100_MV100X_100V);
				if(pstrParamText->PinMethod[i]==_T("MI1X"))
				{
					if(0==_tcscmp(szIRange[i],_T("1ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_1UA_100V);
					if(0==_tcscmp(szIRange[i],_T("10ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_10UA_100V);
					if(0==_tcscmp(szIRange[i],_T("100ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_100UA_100V);
					if(0==_tcscmp(szIRange[i],_T("1ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_1MA_100V);
					if(0==_tcscmp(szIRange[i],_T("10ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_10MA_100V);
					if(0==_tcscmp(szIRange[i],_T("100ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_100MA_100V);
					if(0==_tcscmp(szIRange[i],_T("500ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI1X_500MA_100V);
				}
				if(pstrParamText->PinMethod[i]==_T("MI10X"))
				{
					//if(strIrng[i]==_T("1ua"))
					//	this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_1UA_100V);
					if(0==_tcscmp(szIRange[i],_T("10ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_10UA_100V);
					if(0==_tcscmp(szIRange[i],_T("100ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_100UA_100V);
					if(0==_tcscmp(szIRange[i],_T("1ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_1MA_100V);
					if(0==_tcscmp(szIRange[i],_T("10ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_10MA_100V);
					if(0==_tcscmp(szIRange[i],_T("100ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_100MA_100V);
					if(0==_tcscmp(szIRange[i],_T("500ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI10X_500MA_100V);
				}
				if(pstrParamText->PinMethod[i]==_T("MI100X"))
				{
					//if(strIrng[i]==_T("10ua"))
					//	this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_10UA_100V);
					if(0==_tcscmp(szIRange[i],_T("100ua")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_100UA_100V);
					if(0==_tcscmp(szIRange[i],_T("1ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_1MA_100V);
					if(0==_tcscmp(szIRange[i],_T("10ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_10MA_100V);
					if(0==_tcscmp(szIRange[i],_T("100ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_100MA_100V);
					if(0==_tcscmp(szIRange[i],_T("500ma")))
						this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP100_MI100X_500MA_100V);
				}
			}
		}
		if(this->DB.sp500.Symbl==pstrParamText->PinType[i])
		{
			if(pstrParamText->PinMethod[i]==_T("MV1X"))
				this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szVRange[i],ACCY_SP500_MV1X_500V);
			if(pstrParamText->PinMethod[i]==_T("MI1X"))
			{
				if(0==_tcscmp(szIRange[i],_T("10ua")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP500_MI1X_10UA);
				if(0==_tcscmp(szIRange[i],_T("100ua")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP500_MI1X_100UA);
				if(0==_tcscmp(szIRange[i],_T("1ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP500_MI1X_1MA);
				if(0==_tcscmp(szIRange[i],_T("10ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP500_MI1X_10MA);
				if(0==_tcscmp(szIRange[i],_T("50ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP500_MI1X_50MA);
			}
			if(pstrParamText->PinMethod[i]==_T("MI10X"))
			{
				if(0==_tcscmp(szIRange[i],_T("10ua")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP500_MI10X_10UA);
				if(0==_tcscmp(szIRange[i],_T("100ua")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP500_MI10X_100UA);
				if(0==_tcscmp(szIRange[i],_T("1ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP500_MI10X_1MA);
				if(0==_tcscmp(szIRange[i],_T("10ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP500_MI10X_10MA);
				if(0==_tcscmp(szIRange[i],_T("50ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP500_MI10X_50MA);
			}
			if(pstrParamText->PinMethod[i]==_T("MI100X"))
			{
				if(0==_tcscmp(szIRange[i],_T("10ua")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP500_MI100X_10UA);
				if(0==_tcscmp(szIRange[i],_T("100ua")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP500_MI100X_100UA);
				if(0==_tcscmp(szIRange[i],_T("1ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP500_MI100X_1MA);
				if(0==_tcscmp(szIRange[i],_T("10ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP500_MI100X_10MA);
				if(0==_tcscmp(szIRange[i],_T("50ma")))
					this->FormatAccyString(szAccuracy,pstrParamText->PinMark[i],pstrParamText->PinMethod[i],szIRange[i],ACCY_SP500_MI100X_50MA);
			}
		}
	}
	
	if(0!=_tcscmp(szPinMthdBkup,_T("")))
	{
		pstrParamText->PinMark[1].clear();
		pstrParamText->PinMethod[1].clear();
		pstrParamText->PinType[1].clear();
		pstrParamText->PinMethod[0]=szPinMthdBkup;
	}
	strOut=szAccuracy;
	return 0;
}
//parser parameter condition
int CEagleSrcCodeParser::AI_ParserParameter(CString& strIn,CString* cstrUnit)
{
/*tpp-4*/
	//init database result
	this->initDataBase();
	//convert unit to lowercase using _tcslwr_s
	TCHAR szUnit[MAX_PARAM_UNIT_STR_LENTH];
	ZeroMemory(szUnit,MAX_PARAM_UNIT_STR_LENTH*sizeof(TCHAR));
	_tcscpy_s(szUnit,MAX_PARAM_UNIT_STR_LENTH,cstrUnit->GetBuffer());
	//tstring strUnit=*cstrUnit;
	
	s2lower(szUnit);
	//find corresponding parameter test
	int index=0;
	for(;index<MAX_PARAMETER&&this->Param[index].ParamIndex[0]!=0;index++)
	{
		if(strIn==this->Param[index].ParamIndex.c_str())
			break;
	}
	if(this->Param[index].ParamIndex[0]==0)//not find anything
	{
		return 1;
	}
	ParamText* pParam=&this->Param[index];//find something
	//clear result contents
	pParam->Accuracy.clear();
	pParam->Comment.clear();
	for(int i=0;i<4;i++)
	{
		pParam->PinMark[i].clear();
		pParam->PinMethod[i].clear();
		pParam->PinType[i].clear();
		pParam->PinStats[i].clear();
	}
	//search all symbol
	for(int i=MAX_SAVE_LINE-1;i>=0;i--)
	{
		if(isccmt(pParam->Text[i].c_str()))
			continue;
		else
		{
			this->SearchLine(pParam->Text[i],i);
		}
	}
	//if unit is current 
	if(0==_tcscmp(szUnit,_T("na"))||0==_tcscmp(szUnit,_T("ua"))||0==_tcscmp(szUnit,_T("ma"))||0==_tcscmp(szUnit,_T("a")))
	{
		if(
			//if only 1 RecordCondition
			AI_RULE(1==DB.RecordCondition.Result&&this->DB.mcurun.Result>0)
			//if only >1 RecordCondition
			AI_RULE(DB.RecordCondition.Result>1&&this->DB.mcurun.Result>0&&this->DB.mcurun.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE_END
			)
		{
			//awg pin
			TCHAR szAWGPattern[MAX_TMP_STR_LENTH];
			zerocopystr(szAWGPattern,MAX_TMP_STR_LENTH,pParam->Text[DB.mcurun.Line[0]].c_str());
			//tstring strAWGPattern=pParam->Text[DB.mcurun.Line[0]];
			trimblank(szAWGPattern);
			trimlof(szAWGPattern,DB.LeftBracket.Symbl.c_str());
			trimrof(szAWGPattern,DB.Seperator.Symbl.c_str());
			trimblank(szAWGPattern);
			if(DB.awgselect.Result>0)
			{
				//pin mark and method
				TCHAR szAWGPin[MAX_TMP_STR_LENTH];
				zerocopystr(szAWGPin,MAX_TMP_STR_LENTH,pParam->Text[DB.awgselect.Line[0]].c_str());
				trimblank(szAWGPin);
				trimlof(szAWGPin,DB.LeftBracket.Symbl.c_str());
				trimrof(szAWGPin,DB.Seperator.Symbl.c_str());
				trimblank(szAWGPin);
				//pin type
				TCHAR szAWGType[MAX_TMP_STR_LENTH];
				zerocopystr(szAWGType,MAX_TMP_STR_LENTH,pParam->Text[DB.awgselect.Line[0]].c_str());
				trimblank(szAWGType);
				trimrof(szAWGType,DB.awgselect.Symbl.c_str());
				trimblank(szAWGType);
				if(0!=_tcscmp(szAWGPin,_T("")))
				{
					pParam->PinMark[0]+=szAWGPin;
					pParam->PinMethod[0]+=_T("ramp I");
					pParam->PinType[0]+=szAWGType;
					pParam->Comment+=_T("awg pattern name:\n");
					pParam->Comment+=szAWGPattern;
				}
			}
			//monitor pin
			if(DB.adcmode.Result>0)
			{
				//pin mark and method
				TCHAR szMonitorPin[MAX_TMP_STR_LENTH];
				zerocopystr(szMonitorPin,MAX_TMP_STR_LENTH,pParam->Text[DB.adcmode.Line[0]].c_str());
				trimblank(szMonitorPin);
				trimlof(szMonitorPin,DB.LeftBracket.Symbl.c_str());
				trimrof(szMonitorPin,DB.Seperator.Symbl.c_str());
				trimblank(szMonitorPin);
				//monitor type
				TCHAR szMonitorType[MAX_TMP_STR_LENTH];
				zerocopystr(szMonitorType,MAX_TMP_STR_LENTH,pParam->Text[DB.adcmode.Line[0]].c_str());
				//tstring strMonitorType=pParam->Text[DB.adcmode.Line[0]];
				trimblank(szMonitorType);
				trimlof(szMonitorType,DB.Seperator.Symbl.c_str());
				trimrof(szMonitorType,DB.Seperator.Symbl.c_str());
				trimblank(szMonitorType);
				//trigger point
				TCHAR szTriggerPoint[MAX_TMP_STR_LENTH];
				zerocopystr(szTriggerPoint,MAX_TMP_STR_LENTH,pParam->Text[DB.whereat.Line[0]].c_str());
				//tstring strTriggerPoint=pParam->Text[DB.whereat.Line[0]];
				trimblank(szTriggerPoint);
				trimlol(szTriggerPoint,DB.Seperator.Symbl.c_str());
				trimrof(szTriggerPoint,DB.RightBracket.Symbl.c_str());
				if(_tcsstr(szMonitorType,_T("MV")))
				{
					_stprintf_s(szMonitorType,MAX_TMP_STR_LENTH,_T("monitor V\n%sV trigger"),szTriggerPoint);
				}
				else if(_tcsstr(szMonitorType,_T("MI")))
				{
					_stprintf_s(szMonitorType,MAX_TMP_STR_LENTH,_T("monitor I\n%smA trigger"),szTriggerPoint);
				}
				else
					ZeroMemory(szMonitorType,MAX_TMP_STR_LENTH*sizeof(TCHAR));
				if(0!=_tcscmp(szMonitorPin,_T("")))
				{
					pParam->PinMark[1]+=szMonitorPin;
					pParam->PinMethod[1]+=szMonitorType;
				}
			}
			//if they are the same pin
			if(pParam->PinMark[0]==pParam->PinMark[1])
			{
				pParam->PinMethod[0]+=_T("\n");
				pParam->PinMethod[0]+=pParam->PinMethod[1].c_str();
				pParam->PinMark[1].clear();
				pParam->PinMethod[1].clear();
				pParam->PinType[1].clear();
			}
		}
		else if(
			//if only 1 RecordCondition
			AI_RULE(1==DB.RecordCondition.Result&&this->DB.apu12mi.Result>0)
			AI_RULE(1==DB.RecordCondition.Result&&this->DB.sp100mi.Result>0)
			AI_RULE(1==DB.RecordCondition.Result&&this->DB.sp112mi.Result>0)
			AI_RULE(1==DB.RecordCondition.Result&&this->DB.sp500mi.Result>0)
			//if only >1 RecordCondition
			AI_RULE(DB.RecordCondition.Result>1&&this->DB.apu12mi.Result>0&&this->DB.apu12mi.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE(DB.RecordCondition.Result>1&&this->DB.sp100mi.Result>0&&this->DB.sp100mi.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE(DB.RecordCondition.Result>1&&this->DB.sp112mi.Result>0&&this->DB.sp112mi.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE(DB.RecordCondition.Result>1&&this->DB.sp500mi.Result>0&&this->DB.sp500mi.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE_END
			)
		{
			//find min of line[0]
			int iArray[4];
			iArray[0]=DB.apu12mi.Result>0?DB.apu12mi.Line[0]:0;
			iArray[1]=DB.sp100mi.Result>0?DB.sp100mi.Line[0]:0;
			iArray[2]=DB.sp112mi.Result>0?DB.sp112mi.Line[0]:0;
			iArray[3]=DB.sp500mi.Result>0?DB.sp500mi.Line[0]:0;
			int iMaxMILine=FindMaxValueofArray(iArray,4);
			if(iMaxMILine>0)
			{
				//tstring strMi;
				//tstring strPinType;
				//tstring strMIPinMethod;

				//pin mark
				TCHAR szMi[MAX_TMP_STR_LENTH];
				zerocopystr(szMi,MAX_TMP_STR_LENTH,pParam->Text[iMaxMILine].c_str());

				trimblank(szMi);
				trimlof(szMi,DB.LeftBracket.Symbl.c_str());
				trimrof(szMi,DB.Seperator.Symbl.c_str());
				trimblank(szMi);
				//pin type
				TCHAR szPinType[MAX_TMP_STR_LENTH];
				zerocopystr(szPinType,MAX_TMP_STR_LENTH,pParam->Text[iMaxMILine].c_str());
				trimblank(szPinType);
				trimrof(szPinType,DB.mi.Symbl.c_str());
				trimblank(szPinType);
				//pin method
				TCHAR szMIPinMethod[MAX_TMP_STR_LENTH];
				zerocopystr(szMIPinMethod,MAX_TMP_STR_LENTH,pParam->Text[iMaxMILine].c_str());
				trimblank(szMIPinMethod);
				trimlof(szMIPinMethod,DB._MI_.Symbl.c_str());
				trimrof(szMIPinMethod,DB.Seperator.Symbl.c_str());
				trimblank(szMIPinMethod);
				if(0!=_tcscmp(szMi,_T("")))
				{
					pParam->PinMark[0]=szMi;
					pParam->PinMethod[0]=_T("MI");
					pParam->PinMethod[0]+=szMIPinMethod;
					pParam->PinType[0]=szPinType;
				}
			}
			else
			{
				return 1;
			}
		}
	}
	else if(0==_tcscmp(szUnit,_T("v"))||0==_tcscmp(szUnit,_T("mv"))||0==_tcscmp(szUnit,_T("uv")))
	{
		if(
			//if only 1 RecordCondition
			AI_RULE(1==DB.RecordCondition.Result&&this->DB.mcurun.Result>0)
			//if only >1 RecordCondition
			AI_RULE(DB.RecordCondition.Result>1&&this->DB.mcurun.Result>0&&this->DB.mcurun.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE_END
			)
		{
			//awg pin
			//tstring strAWGPattern=pParam->Text[DB.mcurun.Line[0]];
			TCHAR szAWGPattern[MAX_TMP_STR_LENTH];
			zerocopystr(szAWGPattern,MAX_TMP_STR_LENTH,pParam->Text[DB.mcurun.Line[0]].c_str());
			trimblank(szAWGPattern);
			trimlof(szAWGPattern,DB.LeftBracket.Symbl.c_str());
			trimrof(szAWGPattern,DB.Seperator.Symbl.c_str());
			trimblank(szAWGPattern);
			if(DB.awgselect.Result>0)
			{
				//pin mark and method
				//tstring strAWGPIN=pParam->Text[DB.awgselect.Line[0]];
				TCHAR szAWGPIN[MAX_TMP_STR_LENTH];
				zerocopystr(szAWGPIN,MAX_TMP_STR_LENTH,pParam->Text[DB.awgselect.Line[0]].c_str());
				trimblank(szAWGPIN);
				trimlof(szAWGPIN,DB.LeftBracket.Symbl.c_str());
				trimrof(szAWGPIN,DB.Seperator.Symbl.c_str());
				trimblank(szAWGPIN);
				//pin type
				//tstring strAWGPinType=pParam->Text[DB.awgselect.Line[0]];
				TCHAR szAWGPinType[MAX_TMP_STR_LENTH];
				zerocopystr(szAWGPinType,MAX_TMP_STR_LENTH,pParam->Text[DB.awgselect.Line[0]].c_str());
				trimblank(szAWGPinType);
				trimrof(szAWGPinType,DB.awgselect.Symbl.c_str());
				trimblank(szAWGPinType);
				if(0!=_tcscmp(szAWGPIN,_T("")))
				{
					pParam->PinMark[0]+=szAWGPIN;
					pParam->PinMethod[0]+=_T("ramp V");
					pParam->PinType[0]+=szAWGPinType;
					pParam->Comment+=_T("awg pattern name:\n");
					pParam->Comment+=szAWGPattern;
				}
			}
			//monitor pin
			if(DB.adcmode.Result>0)
			{
				//pin mark and method
				//tstring strMonitorPin=pParam->Text[DB.adcmode.Line[0]];
				TCHAR szMonitorPin[MAX_TMP_STR_LENTH];
				zerocopystr(szMonitorPin,MAX_TMP_STR_LENTH,pParam->Text[DB.adcmode.Line[0]].c_str());
				trimblank(szMonitorPin);
				trimlof(szMonitorPin,DB.LeftBracket.Symbl.c_str());
				trimrof(szMonitorPin,DB.Seperator.Symbl.c_str());
				trimblank(szMonitorPin);
				//monitor type
				//tstring strMonitorType=pParam->Text[DB.adcmode.Line[0]];
				TCHAR szMonitorType[MAX_TMP_STR_LENTH];
				zerocopystr(szMonitorType,MAX_TMP_STR_LENTH,pParam->Text[DB.adcmode.Line[0]].c_str());
				trimblank(szMonitorType);
				trimlof(szMonitorType,DB.Seperator.Symbl.c_str());
				trimrof(szMonitorType,DB.Seperator.Symbl.c_str());
				trimblank(szMonitorType);
				//trigger point
				//tstring strTriggerPoint=pParam->Text[DB.whereat.Line[0]];
				TCHAR szTriggerPoint[MAX_TMP_STR_LENTH];
				zerocopystr(szTriggerPoint,MAX_TMP_STR_LENTH,pParam->Text[DB.whereat.Line[0]].c_str());
				trimblank(szTriggerPoint);
				trimlol(szTriggerPoint,DB.Seperator.Symbl.c_str());
				trimrof(szTriggerPoint,DB.RightBracket.Symbl.c_str());
				if(_tcsstr(szMonitorType,_T("MV")))//if include "MV"
				{
					_stprintf_s(szMonitorType,_T("monitor V\n%sV trigger"),szTriggerPoint);
				}
				else if(_tcsstr(szMonitorType,_T("MI")))//if include "MI"
				{
					_stprintf_s(szMonitorType,_T("monitor I\n%smA trigger"),szTriggerPoint);
				}
				else
					ZeroMemory(szMonitorType,(_tcslen(szMonitorType)+1)*sizeof(TCHAR));//clear content
				if(0!=_tcscmp(szMonitorPin,_T("")))
				{
					pParam->PinMark[1]+=szMonitorPin;
					pParam->PinMethod[1]+=szMonitorType;
				}
			}
			//if they are the same pin
			if(pParam->PinMark[0]==pParam->PinMark[1])
			{
				pParam->PinMethod[0]+=_T("\n");
				pParam->PinMethod[0]+=pParam->PinMethod[1].c_str();
				pParam->PinMark[1].clear();
				pParam->PinMethod[1].clear();
				pParam->PinType[1].clear();
			}
		}
		else if(
			//if only 1 RecordCondition
			AI_RULE(1==DB.RecordCondition.Result&&this->DB.apu12mv.Result>0)
			AI_RULE(1==DB.RecordCondition.Result&&this->DB.sp100mv.Result>0)
			AI_RULE(1==DB.RecordCondition.Result&&this->DB.sp112mv.Result>0)
			AI_RULE(1==DB.RecordCondition.Result&&this->DB.sp500mv.Result>0)
			//if only >1 RecordCondition
			AI_RULE(DB.RecordCondition.Result>1&&this->DB.apu12mv.Result>0&&this->DB.apu12mv.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE(DB.RecordCondition.Result>1&&this->DB.sp100mv.Result>0&&this->DB.sp100mv.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE(DB.RecordCondition.Result>1&&this->DB.sp112mv.Result>0&&this->DB.sp112mv.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE(DB.RecordCondition.Result>1&&this->DB.sp500mv.Result>0&&this->DB.sp500mv.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE_END
			)
		{
			//find min of line[0]
			int iArray[4];
			iArray[0]=DB.apu12mv.Result>0?DB.apu12mv.Line[0]:0;
			iArray[1]=DB.sp100mv.Result>0?DB.sp100mv.Line[0]:0;
			iArray[2]=DB.sp112mv.Result>0?DB.sp112mv.Line[0]:0;
			iArray[3]=DB.sp500mv.Result>0?DB.sp500mv.Line[0]:0;
			int iMaxMVLine=FindMaxValueofArray(iArray,4);
			if(iMaxMVLine>0)
			{
				//tstring strMV;
				//tstring strPinType;
				//tstring strMVPinMethod;
				//pin mark
				TCHAR szMV[MAX_TMP_STR_LENTH];
				zerocopystr(szMV,MAX_TMP_STR_LENTH,pParam->Text[iMaxMVLine].c_str());
				//strMV=pParam->Text[iMaxMVLine];
				trimblank(szMV);
				trimlof(szMV,DB.LeftBracket.Symbl.c_str());
				trimrof(szMV,DB.Seperator.Symbl.c_str());
				trimblank(szMV);
				//pin type
				TCHAR szPinType[MAX_TMP_STR_LENTH];
				zerocopystr(szPinType,MAX_TMP_STR_LENTH,pParam->Text[iMaxMVLine].c_str());
				//strPinType=pParam->Text[iMaxMVLine];
				trimblank(szPinType);
				trimrof(szPinType,DB.mv.Symbl.c_str());
				trimblank(szPinType);
				//pin method
				TCHAR szMVPinMethod[MAX_TMP_STR_LENTH];
				ZeroMemory(szMVPinMethod,MAX_TMP_STR_LENTH*sizeof(TCHAR));
				if(0!=_tcscmp(szPinType,DB.apu12.Symbl.c_str()))
				{
					zerocopystr(szMVPinMethod,MAX_TMP_STR_LENTH,pParam->Text[iMaxMVLine].c_str());
					//strMVPinMethod=pParam->Text[iMaxMVLine];
					trimblank(szMVPinMethod);
					trimlof(szMVPinMethod,DB._MV_.Symbl.c_str());
					trimrof(szMVPinMethod,DB.Seperator.Symbl.c_str());
					trimblank(szMVPinMethod);
				}
				if(0!=_tcscmp(szMV,_T("")))
				{
					pParam->PinMark[0]=szMV;
					pParam->PinMethod[0]=_T("MV");
					pParam->PinMethod[0]+=szMVPinMethod;
					pParam->PinType[0]=szPinType;
				}
			}
			else
			{
				return 1;
			}
		}
	}
	
	else if(0!=_tcsstr(szUnit,_T("ohm")))
	{
		if(
			(
			//if only 1 RecordCondition
			(
			AI_RULE(1==DB.RecordCondition.Result)
			AI_RULE_END
			)
			&&
			(
			AI_RULE(this->DB.apu12mv.Result>0)
			AI_RULE(this->DB.sp100mv.Result>0)
			AI_RULE(this->DB.sp112mv.Result>0)
			AI_RULE(this->DB.sp500mv.Result>0)
			AI_RULE_END
			)
			)
			||
			(
			//if >1 RecordCondition
			(
			AI_RULE(1<DB.RecordCondition.Result)
			AI_RULE_END
			)
			&&
			(
			AI_RULE(this->DB.apu12mv.Result>0&&this->DB.apu12mv.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE(this->DB.sp100mv.Result>0&&this->DB.sp100mv.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE(this->DB.sp112mv.Result>0&&this->DB.sp112mv.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE(this->DB.sp500mv.Result>0&&this->DB.sp500mv.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE_END
			)
			&&
			(
			AI_RULE(this->DB.apu12mi.Result>0&&this->DB.apu12mi.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE(this->DB.sp100mi.Result>0&&this->DB.sp100mi.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE(this->DB.sp112mi.Result>0&&this->DB.sp112mi.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE(this->DB.sp500mi.Result>0&&this->DB.sp500mi.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE_END
			)
			)
			)
		{
			//tstring strMV;
			//tstring strMVPinType;
			//tstring strMVPinMethod;
			//tstring strMI;
			//tstring strMIPinType;
			//tstring strMIPinMethod;
			TCHAR szMV[MAX_TMP_STR_LENTH];
			TCHAR szMVPinType[MAX_TMP_STR_LENTH];
			TCHAR szMVPinMethod[MAX_TMP_STR_LENTH];
			TCHAR szMI[MAX_TMP_STR_LENTH];
			TCHAR szMIPinType[MAX_TMP_STR_LENTH];
			TCHAR szMIPinMethod[MAX_TMP_STR_LENTH];
			ZeroMemory(szMV,MAX_TMP_STR_LENTH*sizeof(TCHAR));
			ZeroMemory(szMVPinType,MAX_TMP_STR_LENTH*sizeof(TCHAR));
			ZeroMemory(szMVPinMethod,MAX_TMP_STR_LENTH*sizeof(TCHAR));
			ZeroMemory(szMI,MAX_TMP_STR_LENTH*sizeof(TCHAR));
			ZeroMemory(szMIPinType,MAX_TMP_STR_LENTH*sizeof(TCHAR));
			ZeroMemory(szMIPinMethod,MAX_TMP_STR_LENTH*sizeof(TCHAR));

			//find min of line[0]
			int iMVArray[4];
			iMVArray[0]=DB.apu12mv.Result>0?DB.apu12mv.Line[0]:0;
			iMVArray[1]=DB.sp100mv.Result>0?DB.sp100mv.Line[0]:0;
			iMVArray[2]=DB.sp112mv.Result>0?DB.sp112mv.Line[0]:0;
			iMVArray[3]=DB.sp500mv.Result>0?DB.sp500mv.Line[0]:0;
			int iMaxMVLine=FindMaxValueofArray(iMVArray,4);
			if(0==iMaxMVLine)
				return 1;
			else
			{
				//pin mark
				//strMV=pParam->Text[iMaxMVLine];
				zerocopystr(szMV,MAX_TMP_STR_LENTH,pParam->Text[iMaxMVLine].c_str());
				trimblank(szMV);
				trimlof(szMV,DB.LeftBracket.Symbl.c_str());
				trimrof(szMV,DB.Seperator.Symbl.c_str());
				trimblank(szMV);
				//pin type
				//strMVPinType=pParam->Text[iMaxMVLine];
				zerocopystr(szMVPinType,MAX_TMP_STR_LENTH,pParam->Text[iMaxMVLine].c_str());
				trimblank(szMVPinType);
				trimrof(szMVPinType,DB.mv.Symbl.c_str());
				trimlof(szMVPinType,DB.Equal.Symbl.c_str());
				trimblank(szMVPinType);
				//pin method
				if(0!=_tcscmp(szMVPinType,DB.apu12.Symbl.c_str()))
				{
					//strMVPinMethod=pParam->Text[iMaxMVLine];
					zerocopystr(szMVPinMethod,MAX_TMP_STR_LENTH,pParam->Text[iMaxMVLine].c_str());
					trimblank(szMVPinMethod);
					trimlof(szMVPinMethod,DB._MV_.Symbl.c_str());
					trimrof(szMVPinMethod,DB.Seperator.Symbl.c_str());
					trimblank(szMVPinMethod);
				}
				if(0!=_tcscmp(szMV,_T("")))
				{
					pParam->PinMark[0]=szMV;
					pParam->PinMethod[0]=_T("MV");
					pParam->PinMethod[0]+=szMVPinMethod;
					pParam->PinType[0]=szMVPinType;
				}
			}
			//find MI
			int iMIArray[4];
			iMIArray[0]=DB.apu12mi.Result>0?DB.apu12mi.Line[0]:0;
			iMIArray[1]=DB.sp100mi.Result>0?DB.sp100mi.Line[0]:0;
			iMIArray[2]=DB.sp112mi.Result>0?DB.sp112mi.Line[0]:0;
			iMIArray[3]=DB.sp500mi.Result>0?DB.sp500mi.Line[0]:0;
			int iMaxMILine=FindMaxValueofArray(iMIArray,4);
			if(0==iMaxMILine)
				return 1;
			else
			{
				//pin mark and method
				//strMI=pParam->Text[iMaxMILine];
				zerocopystr(szMI,MAX_TMP_STR_LENTH,pParam->Text[iMaxMILine].c_str());
				trimblank(szMI);
				trimlof(szMI,DB.LeftBracket.Symbl.c_str());
				trimrof(szMI,DB.Seperator.Symbl.c_str());
				trimblank(szMI);
				//pin type
				//strMIPinType=pParam->Text[iMaxMILine];
				zerocopystr(szMIPinType,MAX_TMP_STR_LENTH,pParam->Text[iMaxMILine].c_str());
				trimblank(szMIPinType);
				trimrof(szMIPinType,DB.mi.Symbl.c_str());
				trimblank(szMIPinType);
				//pin method
				//strMIPinMethod=pParam->Text[iMaxMILine];
				zerocopystr(szMIPinMethod,MAX_TMP_STR_LENTH,pParam->Text[iMaxMILine].c_str());
				trimblank(szMIPinMethod);
				trimlof(szMIPinMethod,DB._MI_.Symbl.c_str());
				trimrof(szMIPinMethod,DB.Seperator.Symbl.c_str());
				trimblank(szMIPinMethod);
				if(0!=_tcscmp(szMI,_T("")))
				{
					pParam->PinMark[1]=szMI;
					pParam->PinMethod[1]=_T("MI");
					pParam->PinMethod[1]+=szMIPinMethod;
					pParam->PinType[1]=szMIPinType;
				}
			}
			if(pParam->PinMark[0]==pParam->PinMark[1])
			{
				pParam->PinMethod[0]+=_T("/");
				pParam->PinMethod[0]+=pParam->PinMethod[1];
				pParam->PinMark[1].clear();
				pParam->PinMethod[1].clear();
				pParam->PinType[1].clear();
				//comments
				pParam->Comment=_T("     ");
				pParam->Comment+=_T("MV");
				pParam->Comment+=_T("(");
				pParam->Comment+=pParam->PinMark[0];
				pParam->Comment+=_T(")");
				pParam->Comment+=_T("\nR = -------------------------\n");
				pParam->Comment+=_T("     ");
				pParam->Comment+=_T("MI");
				pParam->Comment+=_T("(");
				pParam->Comment+=pParam->PinMark[0];
				pParam->Comment+=_T(")");
			}
			else
			{
				//comments
				pParam->Comment=_T("     ");
				pParam->Comment+=pParam->PinMethod[0];
				pParam->Comment+=_T("(");
				pParam->Comment+=pParam->PinMark[0];
				pParam->Comment+=_T(")");
				pParam->Comment+=_T("\nR = -------------------------\n");
				pParam->Comment+=_T("     ");
				pParam->Comment+=pParam->PinMethod[1];
				pParam->Comment+=_T("(");
				pParam->Comment+=pParam->PinMark[1];
				pParam->Comment+=_T(")");
			}
		}
	}
	else if(0==_tcscmp(szUnit,_T("mhz"))||0==_tcscmp(szUnit,_T("khz")))
	{
		if(
			//if only 1 RecordCondition
			AI_RULE(1==DB.RecordCondition.Result&&this->DB.qtmumt.Result>0&&this->DB.QTMU_READFREQ.Result>0)
			//if only >1 RecordCondition
			AI_RULE(DB.RecordCondition.Result>1&&this->DB.qtmumt.Result>0&&this->DB.qtmumt.Line[0]>DB.RecordCondition.Line[1]&&this->DB.QTMU_READFREQ.Result>0&&this->DB.QTMU_READFREQ.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE_END
			)
		{
				//pin mark and method
				//tstring strMF=pParam->Text[this->DB.qtmumt.Line[0]];
				TCHAR szMF[MAX_TMP_STR_LENTH];
				zerocopystr(szMF,MAX_TMP_STR_LENTH,pParam->Text[this->DB.qtmumt.Line[0]].c_str());
				trimblank(szMF);
				trimlof(szMF,DB.LeftBracket.Symbl.c_str());
				trimrof(szMF,DB.Seperator.Symbl.c_str());
				trimblank(szMF);
				if(0!=_tcscmp(szMF,_T("")))
				{
					pParam->PinMark[0]=szMF;//may not exist this pin in rmthdl map
					pParam->PinMethod[0]=_T("measure frequency");
					pParam->Comment+=_T("using ");//using QTMU measure frequency
					pParam->Comment+=szMF;
					pParam->Comment+=_T(" measure frequency");
					pParam->PinType[0]=_T("qtmu");
				}
		}
		else
		{
			pParam->Comment+=_T("Calculated value\n ");//not find infomation
			//if(this->DB.DivisionSign.Result>0)
			//{
			//	pParam->Comment+=_T("but software find below code:\n ");
			//	pParam->Comment+=_T("/**source code start**/\n");
			//	//for(int i=0;this->DB.DivisionSign.Line[i]>0;i++)
			//	for(int i=this->DB.DivisionSign.Result-1;i>=0;i--)
			//	{
			//		pParam->Comment+=pParam->Text[this->DB.DivisionSign.Line[i]];
			//	}
			//	pParam->Comment+=_T("/**source code stop**/\n");
			//	pParam->Comment+=_T("please manually check this parameter");
			//}
		}
	}
	else if(0==_tcscmp(szUnit,_T("ns"))||0==_tcscmp(szUnit,_T("us"))||0==_tcscmp(szUnit,_T("s")))
	{
		if(
			//if only 1 RecordCondition
			AI_RULE(1==DB.RecordCondition.Result&&this->DB.qtmumt.Result>0&&this->DB.QTMU_READTIME.Result>0)
			//if only >1 RecordCondition
			AI_RULE(DB.RecordCondition.Result>1&&this->DB.qtmumt.Result>0&&this->DB.qtmumt.Line[0]>DB.RecordCondition.Line[1]&&this->DB.QTMU_READTIME.Result>0&&this->DB.QTMU_READTIME.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE_END
			)
		{
				//pin mark and method
				//tstring strMT=pParam->Text[this->DB.qtmumt.Line[0]];
				TCHAR szMT[MAX_TMP_STR_LENTH];
				zerocopystr(szMT,MAX_TMP_STR_LENTH,pParam->Text[this->DB.qtmumt.Line[0]].c_str());
				trimblank(szMT);
				trimlof(szMT,DB.LeftBracket.Symbl.c_str());
				trimrof(szMT,DB.Seperator.Symbl.c_str());
				trimblank(szMT);
				if(0!=_tcscmp(szMT,_T("")))
				{
					pParam->PinMark[0]=szMT;//may not exist this pin in rmthdl map
					pParam->PinMethod[0]=_T("measure time");
					pParam->Comment+=_T("using ");//using QTMU measure frequency
					pParam->Comment+=szMT;
					pParam->Comment+=_T(" measure time");
					pParam->PinType[0]=_T("qtmu");
				}
		}
		else
		{
			pParam->Comment+=_T("software can not find distinction for timing measurement\n ");//not find infomation
			if(this->DB.DivisionSign.Result>0)
			{
				pParam->Comment+=_T("but software find below code:\n ");
				pParam->Comment+=_T("/**source code start**/\n");
				//for(int i=0;this->DB.DivisionSign.Line[i]>0;i++)
				for(int i=this->DB.DivisionSign.Result-1;i>=0;i--)
				{
					pParam->Comment+=pParam->Text[this->DB.DivisionSign.Line[i]];
				}
				pParam->Comment+=_T("/**source code stop**/\n");
				pParam->Comment+=_T("please manually check this parameter");
			}
		}
	}
	return 0;
}
//get param by index 
ParamText* CEagleSrcCodeParser::getParamTextStructByIndex(CString& strindex)
{
	if(false==this->bIsAnalysisSuccess)
	{
		return &this->uAnalysisFailParam;
	}
	int index=0;
	for(;index<MAX_PARAMETER;index++)
	{
		if(strindex==this->Param[index].ParamIndex.c_str())
			break;
	}
	if(index==MAX_PARAMETER)
	{
		return &this->uAnalysisSuccessButNoSpecifiedParam;
	}
	return &this->Param[index];
}
//get parameter text into a cstring
CString CEagleSrcCodeParser::getParamText(CString& strIn)
{
	CString strRet;
	int index=0;
	for(;index<MAX_PARAMETER;index++)
	{
		//if(strIn==this->Param[index].ParamIndex.c_str())
		if(strIn==this->Param[index].ParamIndex.c_str())
			break;
	}
	if(index==MAX_PARAMETER){
		strRet=_T("can't find source code");
		return strRet;
	}
	for(int i=0;i<MAX_SAVE_LINE;i++)
		strRet+=this->Param[index].Text[i].c_str();
	return strRet;
}











