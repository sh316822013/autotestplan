#include "StdAfx.h"
#include "AccoSrcCodeParser.h"

//create a TextFIFO
CTextFIFO* pclassFIFOInst=new CTextFIFO;
//constructor
CAccoSrcCodeParser::CAccoSrcCodeParser(void)
{
	this->init();
}
//deconstructor
CAccoSrcCodeParser::~CAccoSrcCodeParser(void)
{
	this->init();
	delete(pclassFIFOInst);
}

LPCTSTR tagParamNum = _T("RecordCondition");
//init method
void CAccoSrcCodeParser::init(void)
{
	this->bIsAnalysisSuccess=0;
	ZeroMemory(&this->Param,sizeof(ParamText)*MAX_PARAMETER);
	ZeroMemory(&this->DB,sizeof(ACCOSYMBOLDATABASE));
	this->dParamCnt=0;
	ZeroMemory(&this->uAnalysisFailParam,sizeof(ParamText));
	this->uAnalysisFailParam.Comment=_T("failed to analysis source code");
	ZeroMemory(&this->uAnalysisSuccessButNoSpecifiedParam,sizeof(ParamText));
	this->uAnalysisSuccessButNoSpecifiedParam.Comment=_T("no infomation found in source code");
}

#define lpszRelayTypeAccoCbitClose					_T("cbitclose(")
#define lpszRelayTypeAccoCbitCloseEnd				_T(")")
#define lpszRelayTypeAccoCbitOpen					_T("cbitopen(")
#define lpszRelayTypeAccoCbitOpenEnd				_T(")")
#define lpszRelayTypeAccoCbitDotSetOn					_T(".SetOn(")
#define lpszRelayTypeAccoCbitDotSetOnEnd				_T("-1")

DWORD CAccoSrcCodeParser::analysis(FILE* pFile,CDataSheet* pDatasheetparser)
{
	//initialize FIFO
	pclassFIFOInst->Init();
	//init AI database
	this->initDataBase(1);
	this->dParamCnt=0;
	TCHAR rcvBuf[MAX_TMP_STR_LENTH];

	for(int i=0;i<MAX_PARAMETER;i++)
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
	FuncTrack uAccoFuncOrder;
	TraceFunction(&uAccoFuncOrder,pDatasheetparser,_T("DUT_API"),_T("return"),nLine);
	tstring strRelayRecord=_T("");
	int debugpiont=0;
	//find in each function
	for(int j=0;j<uAccoFuncOrder.nFunctionCnt;j++)
	{
		if(uAccoFuncOrder.nFuncLineStart[j]>=uAccoFuncOrder.nFuncLineStop[j]||0==uAccoFuncOrder.nFuncLineStart[j]||0==uAccoFuncOrder.nFuncLineStop[j])
			return 1;// fail return
		for(int k=uAccoFuncOrder.nFuncLineStart[j];k<uAccoFuncOrder.nFuncLineStop[j];k++)
		{
			//lpszRelayTypeEagleCbit
			if(_tcsstr(gszTextFileBuff[k],lpszRelayTypeAccoCbitClose))
			{
				_tcscpy_s(buff,MAX_FILEOP_BUFF,gszTextFileBuff[k]);
				if(!isccmt(buff))
				{
					//cbitclose(HB_K7,HB_K8);
					if(_tcsstr(buff,lpszRelayTypeAccoCbitClose))
					{
						trimlof(buff,lpszRelayTypeAccoCbitClose);
						trimrof(buff,lpszRelayTypeAccoCbitCloseEnd);
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
							strRelayRecord+=buff;
							strRelayRecord+=_T(",");
							debugpiont++;
						}
						for(int x=0;x<nBufLen;x++)
						{
							if(buff[x]==0)
							{
								strTmpRelay=&buff[x+1];
								int pos=strRelayRecord.find(strTmpRelay);
								if(pos==-1)
								{
									strRelayRecord+=strTmpRelay;
									strRelayRecord+=_T(",");
									debugpiont++;
								}
							}
						}
					}
				}
			}
			if(_tcsstr(gszTextFileBuff[k],lpszRelayTypeAccoCbitOpen))
			{
				_tcscpy_s(buff,MAX_FILEOP_BUFF,gszTextFileBuff[k]);
				if(!isccmt(buff))
				{
					//cbitopen(HB_K7);
					if(_tcsstr(buff,lpszRelayTypeAccoCbitOpen))
					{
						trimlof(buff,lpszRelayTypeAccoCbitOpen);
						trimrof(buff,lpszRelayTypeAccoCbitOpenEnd);
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
							strRelayRecord.erase(pos,_tcslen(buff)+1);
							debugpiont++;
						}
						for(int x=0;x<nBufLen;x++)
						{
							if(buff[x]==0)
							{
								int pos=strRelayRecord.find(&buff[x+1]);
								if(pos!=-1)
								{
									strRelayRecord.erase(pos,_tcslen(&buff[x+1])+1);
									debugpiont++;
								}
							}
						}
					}
				}
			}
			if(_tcsstr(gszTextFileBuff[k],lpszRelayTypeAccoCbitDotSetOn))
			{
				_tcscpy_s(buff,MAX_FILEOP_BUFF,gszTextFileBuff[k]);
				if(!isccmt(buff))
				{
					//cbitopen(HB_K7);
					if(_tcsstr(buff,lpszRelayTypeAccoCbitDotSetOn))
					{
						trimlof(buff,lpszRelayTypeAccoCbitDotSetOn);
						trimrof(buff,lpszRelayTypeAccoCbitDotSetOnEnd);
						trimstr(buff,_T(" "));
						trimstr(buff,_T("	"));
						strRelayRecord=buff;
						debugpiont++;
					}
				}
			}
			//insert fifo
			pclassFIFOInst->Insert(gszTextFileBuff[k]);
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
				pclassFIFOInst->GetOut(this->Param[this->dParamCnt].Text);
				if(buff[0]!=0)
					this->dParamCnt++;
			}
		}
	}
	this->bIsAnalysisSuccess=1;
	return 0;
}
void CAccoSrcCodeParser::initDataBase(BOOL type=0)
{
	ZeroMemory(&this->DB,sizeof(ACCOSYMBOLDATABASE));
	//Voltage and Current
	this->DB.MeasureVI.Symbl=_T(".MeasureVI(");
	this->DB.MVRET.Symbl=_T("MVRET");
	this->DB.MIRET.Symbl=_T("MIRET");
	this->DB.GetMesaResult.Symbl=_T(".GetMeasResult(");
	//timing
	this->DB.MeasFreq.Symbl=_T(".MeasFreq(");
	this->DB.Meas.Symbl=_T(".Meas(");
	this->DB.SinglePlsMeas.Symbl=_T(".SinglePlsMeas(");
	this->DB.QTMU_PLUS_COARSE.Symbl=_T("QTMU_PLUS_COARSE");
	this->DB.QTMU_PLUS_FINE.Symbl=_T("QTMU_PLUS_FINE");
	this->DB.QTMU_PLUS_TRNG_US.Symbl=_T("QTMU_PLUS_TRNG_US");
	this->DB.QTMU_PLUS_TRNG_NS.Symbl=_T("QTMU_PLUS_TRNG_NS");
	//resistor
	this->DB.DivisionSign.Symbl=_T("/");
	//AWG
	if(1==type)
	this->DB.STSAWGCreateRampData.Symbl=_T("STSAWGCreateRampData(");
	this->DB.STSEnableAWG.Symbl=_T("STSEnableAWG(");
	this->DB.STSEnableMeas.Symbl=_T("STSEnableMeas(");
	this->DB.SetMeasVTrig.Symbl=_T(".SetMeasVTrig(");
	this->DB.SetMeasITrig.Symbl=_T(".SetMeasITrig(");
	this->DB.AwgLoader.Symbl=_T(".AwgLoader(");
	//general
	this->DB.RecordCondition.Symbl=_T("RecordCondition(");
	this->DB.Space.Symbl=_T(" ");
	this->DB.Tab.Symbl=_T("	");
	this->DB.Quote.Symbl=_T("&");
	this->DB.Equal.Symbl=_T("=");
	this->DB.Seperator.Symbl=_T(",");
	
}
//search line
void CAccoSrcCodeParser::SearchLine(tstring& str,int line)
{
/*tpp-2*/
	int dPos;
//MV MI
	//MeasureVI
	dPos=-1;
	dPos=str.find(this->DB.MeasureVI.Symbl);
	if(dPos>=0)
	{
		DB.MeasureVI.Pos[DB.MeasureVI.Result]=dPos;
		DB.MeasureVI.Line[DB.MeasureVI.Result++]=line;
	}
	//MIRET
	dPos=-1;
	dPos=str.find(this->DB.MIRET.Symbl);
	if(dPos>=0)
	{
		DB.MIRET.Pos[DB.MIRET.Result]=dPos;
		DB.MIRET.Line[DB.MIRET.Result++]=line;
	}
	//MVRET
	dPos=-1;
	dPos=str.find(this->DB.MVRET.Symbl);
	if(dPos>=0)
	{
		DB.MVRET.Pos[DB.MVRET.Result]=dPos;
		DB.MVRET.Line[DB.MVRET.Result++]=line;
	}
	//GetMesaResult
	dPos=-1;
	dPos=str.find(this->DB.GetMesaResult.Symbl);
	if(dPos>=0)
	{
		DB.GetMesaResult.Pos[DB.GetMesaResult.Result]=dPos;
		DB.GetMesaResult.Line[DB.GetMesaResult.Result++]=line;
	}
//timing
	//MeasFreq
	dPos=-1;
	dPos=str.find(this->DB.MeasFreq.Symbl);
	if(dPos>=0)
	{
		DB.MeasFreq.Pos[DB.MeasFreq.Result]=dPos;
		DB.MeasFreq.Line[DB.MeasFreq.Result++]=line;
	}
	//Meas
	dPos=-1;
	dPos=str.find(this->DB.Meas.Symbl);
	if(dPos>=0)
	{
		DB.Meas.Pos[DB.Meas.Result]=dPos;
		DB.Meas.Line[DB.Meas.Result++]=line;
	}
	//SinglePlsMeas
	dPos=-1;
	dPos=str.find(this->DB.SinglePlsMeas.Symbl);
	if(dPos>=0)
	{
		DB.SinglePlsMeas.Pos[DB.SinglePlsMeas.Result]=dPos;
		DB.SinglePlsMeas.Line[DB.SinglePlsMeas.Result++]=line;
	}
	//QTMU_PLUS_COARSE
	dPos=-1;
	dPos=str.find(this->DB.QTMU_PLUS_COARSE.Symbl);
	if(dPos>=0)
	{
		DB.QTMU_PLUS_COARSE.Pos[DB.QTMU_PLUS_COARSE.Result]=dPos;
		DB.QTMU_PLUS_COARSE.Line[DB.QTMU_PLUS_COARSE.Result++]=line;
	}
	//QTMU_PLUS_FINE
	dPos=-1;
	dPos=str.find(this->DB.QTMU_PLUS_FINE.Symbl);
	if(dPos>=0)
	{
		DB.QTMU_PLUS_FINE.Pos[DB.QTMU_PLUS_FINE.Result]=dPos;
		DB.QTMU_PLUS_FINE.Line[DB.QTMU_PLUS_FINE.Result++]=line;
	}
	//QTMU_PLUS_TRNG_NS
	dPos=-1;
	dPos=str.find(this->DB.QTMU_PLUS_TRNG_NS.Symbl);
	if(dPos>=0)
	{
		DB.QTMU_PLUS_TRNG_NS.Pos[DB.QTMU_PLUS_TRNG_NS.Result]=dPos;
		DB.QTMU_PLUS_TRNG_NS.Line[DB.QTMU_PLUS_TRNG_NS.Result++]=line;
	}
	//QTMU_PLUS_TRNG_US
	dPos=-1;
	dPos=str.find(this->DB.QTMU_PLUS_TRNG_US.Symbl);
	if(dPos>=0)
	{
		DB.QTMU_PLUS_TRNG_US.Pos[DB.QTMU_PLUS_TRNG_US.Result]=dPos;
		DB.QTMU_PLUS_TRNG_US.Line[DB.QTMU_PLUS_TRNG_US.Result++]=line;
	}
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
//awg
	//STSAWGCreateRampData
	dPos=-1;
	dPos=str.find(this->DB.STSAWGCreateRampData.Symbl);
	if(dPos>=0)
	{
		DB.STSAWGCreateRampData.Pos[DB.STSAWGCreateRampData.Result]=dPos;
		DB.STSAWGCreateRampData.Line[DB.STSAWGCreateRampData.Result++]=line;
	}
	//STSEnableAWG
	dPos=-1;
	dPos=str.find(this->DB.STSEnableAWG.Symbl);
	if(dPos>=0)
	{
		DB.STSEnableAWG.Pos[DB.STSEnableAWG.Result]=dPos;
		DB.STSEnableAWG.Line[DB.STSEnableAWG.Result++]=line;
	}
	//STSEnableMeas
	dPos=-1;
	dPos=str.find(this->DB.STSEnableMeas.Symbl);
	if(dPos>=0)
	{
		DB.STSEnableMeas.Pos[DB.STSEnableMeas.Result]=dPos;
		DB.STSEnableMeas.Line[DB.STSEnableMeas.Result++]=line;
	}
	//SetMeasVTrig
	dPos=-1;
	dPos=str.find(this->DB.SetMeasVTrig.Symbl);
	if(dPos>=0)
	{
		DB.SetMeasVTrig.Pos[DB.SetMeasVTrig.Result]=dPos;
		DB.SetMeasVTrig.Line[DB.SetMeasVTrig.Result++]=line;
	}
	//SetMeasITrig
	dPos=-1;
	dPos=str.find(this->DB.SetMeasITrig.Symbl);
	if(dPos>=0)
	{
		DB.SetMeasITrig.Pos[DB.SetMeasITrig.Result]=dPos;
		DB.SetMeasITrig.Line[DB.SetMeasITrig.Result++]=line;
	}
	//AwgLoader
	dPos=-1;
	dPos=str.find(this->DB.AwgLoader.Symbl);
	if(dPos>=0)
	{
		DB.AwgLoader.Pos[DB.AwgLoader.Result]=dPos;
		DB.AwgLoader.Line[DB.AwgLoader.Result++]=line;
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
}
int CAccoSrcCodeParser::GetAccuracy(tstring& strOut,ParamText* pstrParamText)
{
	tstring accy;

	for(int i=0;i<4&&!pstrParamText->PinMark[i].empty();i++)
	{
		tstring strRng=pstrParamText->PinStats[i];
		tstring strMethod=pstrParamText->PinMethod[i];
		myString::StrToLowerCase(strRng);
		tstring strVrng=strRng;
		tstring strIrng=strRng;
		myString::TrimRofFF(strVrng,_T("/"));
		myString::TrimLofFF(strIrng,_T("/"));
		myString::TrimRofFF(strIrng,_T("\n"));
		myString::StrToLowerCase(strMethod);
		if(strMethod==_T("mv"))
		{
			if(!accy.empty()) accy+=_T("/");
			if(strVrng==_T("50v")) accy+=_T("?5mV");
			if(strVrng==_T("20v")) accy+=_T("?0mV");
			if(strVrng==_T("10v")) accy+=_T("?mV");
			if(strVrng==_T("5v")) accy+=_T("?.5mV");
			if(strVrng==_T("2v")) accy+=_T("?mV");
			if(strVrng==_T("1v")) accy+=_T("?.5mV");
			if(strVrng==_T("100mv")) accy+=_T("?.4mV");
		}
		if(strMethod==_T("mi"))
		{
			if(!accy.empty()) accy+=_T("/");
			if(strIrng==_T("10a")) accy+=_T("?0mA");
			if(strIrng==_T("1a")) accy+=_T("?mA");
			if(strIrng==_T("100ma")) accy+=_T("?00uA");
			if(strIrng==_T("10ma")) accy+=_T("?0uA");
			if(strIrng==_T("1ma")) accy+=_T("?uA");
			if(strIrng==_T("100ua")) accy+=_T("?00nA");
			if(strIrng==_T("10ua")) accy+=_T("?0nA");
			if(strIrng==_T("1ua")) accy+=_T("?0nA");
		}
	}
	strOut+=accy;
	strOut+=_T("\n");
	return 0;
}
//parser parameter condition
int CAccoSrcCodeParser::AI_ParserParameter(CString& strIn,CString* cstrUnit)
{
/*tpp-3*/
	//init database result
	this->initDataBase();
	//convert unit to lowercase using _tcslwr_s
	tstring strUnit=*cstrUnit;
	myString::StrToLowerCase(strUnit);
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
		if(myString::IsComment(pParam->Text[i]))
			continue;
		else
		{
			this->SearchLine(pParam->Text[i],i);
		}
	}
	//if unit is current 
	if(strUnit==_T("na")||strUnit==_T("ua")||strUnit==_T("ma")||strUnit==_T("a"))
	{
		if(	AI_RULE(DB.RecordCondition.Result<2&&this->DB.MIRET.Result>0/*&&this->DB.GetMesaResult.Result>0*/&&this->DB.MeasureVI.Result>0)//if only 1 RecordCondition
			AI_RULE(DB.RecordCondition.Result>=2&&this->DB.MIRET.Result>0&&this->DB.MIRET.Line[0]>DB.RecordCondition.Line[1]/*&&this->DB.GetMesaResult.Result>0&&this->DB.GetMesaResult.Line[0]>DB.RecordCondition.Line[1]*/&&this->DB.MeasureVI.Result>0&&this->DB.MeasureVI.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE_END)
		{

			tstring tmpstr=pParam->Text[DB.MeasureVI.Line[0]];
			myString::TrimSpaceAndTab(tmpstr);
			int mesaPos=tmpstr.find(this->DB.MeasureVI.Symbl);
			if(mesaPos!=-1)//if find Measure
			{
				pParam->PinMark[0]+=tmpstr.substr(0,mesaPos);
				pParam->PinMethod[0]=_T("MI");
			}
		}
		else if(	
			AI_RULE(DB.RecordCondition.Result<2&&this->DB.STSEnableAWG.Result>0)//if only 1 RecordCondition
			AI_RULE(DB.RecordCondition.Result>=2&&this->DB.STSEnableAWG.Result>0&&this->DB.STSEnableAWG.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE_END)
		{
			if(DB.AwgLoader.Result>0)
			{
				//AWG pin
				tstring strAwgPin=pParam->Text[DB.AwgLoader.Line[0]];
				myString::TrimSpaceAndTab(strAwgPin);
				myString::TrimRofFF(strAwgPin,DB.AwgLoader.Symbl);
				//AWG mode
				tstring strAwgMode;
				int iPosFV=pParam->Text[DB.AwgLoader.Line[0]].find(_T("FV"));
				int iPosFI=pParam->Text[DB.AwgLoader.Line[0]].find(_T("FI"));
				if(iPosFV>=0&&iPosFI==-1)
					strAwgMode=_T("Ramp V");
				else if(iPosFI>=0&&iPosFV==-1)
					strAwgMode=_T("Ramp I");
				//mesaure pin
				tstring strMesaPin,strMesaMethod;
				if(DB.SetMeasVTrig.Result>0)
				{
					strMesaPin=pParam->Text[DB.SetMeasVTrig.Line[0]];
					myString::TrimSpaceAndTab(strMesaPin);
					myString::TrimRofFF(strMesaPin,DB.SetMeasVTrig.Symbl);
					strMesaMethod=_T("Monitor V");
				}
				else	if(DB.SetMeasITrig.Result>0)
				{
					strMesaPin=pParam->Text[DB.SetMeasITrig.Line[0]];
					myString::TrimSpaceAndTab(strMesaPin);
					myString::TrimRofFF(strMesaPin,DB.SetMeasITrig.Symbl);
					strMesaMethod=_T("Monitor I");
				}
				else
				{
					strMesaPin=_T("can't find mesaure pin.");
				}
				if(strAwgPin==strMesaPin)
				{
					pParam->PinMark[0]=strAwgPin;
					pParam->PinMethod[0]=strAwgMode+_T("\n")+strMesaMethod;
				}
				else
				{
					pParam->PinMark[0]=strAwgPin;
					pParam->PinMethod[0]=strAwgMode;
					pParam->PinMark[1]=strMesaPin;
					pParam->PinMethod[1]=strMesaMethod;
				}
				pParam->Comment=_T("Can't analysis start/stop point, step and accuracy infomation In AWG test");
			}
		}
	}
	else if(strUnit==_T("v")||strUnit==_T("mv")||strUnit==_T("uv"))
	{
		if(	AI_RULE(DB.RecordCondition.Result<2&&this->DB.MVRET.Result>0/*&&this->DB.GetMesaResult.Result>0*/&&this->DB.MeasureVI.Result>0)//if only 1 RecordCondition
			AI_RULE(DB.RecordCondition.Result>=2&&this->DB.MVRET.Result>0&&this->DB.MVRET.Line[0]>DB.RecordCondition.Line[1]/*&&this->DB.GetMesaResult.Result>0&&this->DB.GetMesaResult.Line[0]>DB.RecordCondition.Line[1]*/&&this->DB.MeasureVI.Result>0&&this->DB.MeasureVI.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE_END)
		{
			tstring tmpstr=pParam->Text[DB.MeasureVI.Line[0]];
			myString::TrimSpaceAndTab(tmpstr);
			int mesaPos=tmpstr.find(this->DB.MeasureVI.Symbl);
			if(mesaPos!=-1)//if find Measure
			{
				pParam->PinMark[0]+=tmpstr.substr(0,mesaPos);
				pParam->PinMethod[0]=_T("MV");
			}
		}
		else if(	
			AI_RULE(DB.RecordCondition.Result<2&&this->DB.STSEnableAWG.Result>0)//if only 1 RecordCondition
			AI_RULE(DB.RecordCondition.Result>=2&&this->DB.STSEnableAWG.Result>0&&this->DB.STSEnableAWG.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE_END)
		{
			if(DB.AwgLoader.Result>0)
			{
				//AWG pin
				tstring strAwgPin=pParam->Text[DB.AwgLoader.Line[0]];
				myString::TrimSpaceAndTab(strAwgPin);
				myString::TrimRofFF(strAwgPin,DB.AwgLoader.Symbl);
				//AWG mode
				tstring strAwgMode;
				if(pParam->Text[DB.AwgLoader.Line[0]].find(_T("FV"))>=0)
					strAwgMode=_T("Ramp V");
				else if(pParam->Text[DB.AwgLoader.Line[0]].find(_T("FI"))>=0)
					strAwgMode=_T("Ramp I");
				//mesaure pin
				tstring strMesaPin,strMesaMethod;
				if(DB.SetMeasVTrig.Result>0)
				{
					strMesaPin=pParam->Text[DB.SetMeasVTrig.Line[0]];
					myString::TrimSpaceAndTab(strMesaPin);
					myString::TrimRofFF(strMesaPin,DB.SetMeasVTrig.Symbl);
					strMesaMethod=_T("Monitor V");
				}
				else	if(DB.SetMeasITrig.Result>0)
				{
					strMesaPin=pParam->Text[DB.SetMeasITrig.Line[0]];
					myString::TrimSpaceAndTab(strMesaPin);
					myString::TrimRofFF(strMesaPin,DB.SetMeasITrig.Symbl);
					strMesaMethod=_T("Monitor I");
				}
				else
				{
					strMesaPin=_T("can't find mesaure pin.");
				}
				if(strAwgPin==strMesaPin)
				{
					pParam->PinMark[0]=strAwgPin;
					pParam->PinMethod[0]=strAwgMode+_T("\n")+strMesaMethod;
				}
				else
				{
					pParam->PinMark[0]=strAwgPin;
					pParam->PinMethod[0]=strAwgMode;
					pParam->PinMark[1]=strMesaPin;
					pParam->PinMethod[1]=strMesaMethod;
				}
				pParam->Comment=_T("Can't analysis start/stop point, step and accuracy infomation In AWG test");
			}
		}
	}
	else if(-1!=strUnit.find(_T("ohm")))
	//else if(strUnit==_T("ohm")||strUnit==_T("kohm")||strUnit==_T("mohm"))
	{
		//acco condition
		if(	
			AI_RULE(//if only 1 RecordCondition
				DB.RecordCondition.Result<2&&this->DB.MVRET.Result>0&&this->DB.MIRET.Result>0/*&&this->DB.GetMesaResult.Result>0*/&&this->DB.MeasureVI.Result>0
			)
			AI_RULE(
				this->DB.RecordCondition.Result>=2&&\
				this->DB.MVRET.Result>0&&this->DB.MVRET.Line[0]>DB.RecordCondition.Line[1]&&\
				this->DB.MIRET.Result>0&&this->DB.MIRET.Line[0]>DB.RecordCondition.Line[1]&&\
				/*this->DB.GetMesaResult.Result>0&&this->DB.GetMesaResult.Line[0]>DB.RecordCondition.Line[1]&&\*/
				this->DB.MeasureVI.Result>0&&this->DB.MeasureVI.Line[0]>DB.RecordCondition.Line[1]
			)
			AI_RULE_END
		)
		{
			tstring strV=pParam->Text[DB.MVRET.Line[0]];
			tstring strI=pParam->Text[DB.MIRET.Line[0]];
			//find MV pin
			myString::TrimSpaceAndTab(strV);
			myString::TrimLofFF(strV,DB.Equal.Symbl);
			myString::TrimRofFF(strV,DB.GetMesaResult.Symbl);
			//find MI pin
			myString::TrimSpaceAndTab(strI);
			myString::TrimLofFF(strI,DB.Equal.Symbl);
			myString::TrimRofFF(strI,DB.GetMesaResult.Symbl);
			if(pParam->PinMark[0]==pParam->PinMark[1])
			{
				pParam->PinMark[0]+=strV;
				pParam->PinMethod[0]=_T("MV/MI");
			}
			else
			{
				pParam->PinMark[0]+=strV;
				pParam->PinMethod[0]=_T("MV");
				pParam->PinMark[1]+=strI;
				pParam->PinMethod[1]=_T("MI");
			}
			pParam->Comment=_T("Can't analysis accuracy of aculated value");
		}
	}
	else if(strUnit==_T("mhz")||strUnit==_T("khz"))
	{
		if(	AI_RULE(//if only 1 RecordCondition
				DB.RecordCondition.Result<2&&this->DB.MeasFreq.Result>0)
			AI_RULE(
				this->DB.RecordCondition.Result>=2&&\
				this->DB.MeasFreq.Result>0&&this->DB.MeasFreq.Line[0]>DB.RecordCondition.Line[1]
			)AI_RULE_END
		)
		{
			if(this->DB.QTMU_PLUS_COARSE.Result>0)
				pParam->Accuracy=_T("?0nS?.1% of reading");
			if(this->DB.QTMU_PLUS_FINE.Result>0)
				pParam->Accuracy=_T("?nS?.1% of reading");
			pParam->Comment=_T("Can't analysis which pin to mark in timing test");
		}
	}
	else if(strUnit==_T("ns")||strUnit==_T("us")||strUnit==_T("s"))
	{
		if(	AI_RULE(//if only 1 RecordCondition
				this->DB.RecordCondition.Result<2&&this->DB.Meas.Result>0)
			AI_RULE(
				this->DB.RecordCondition.Result<2&&this->DB.SinglePlsMeas.Result>0)
			AI_RULE(
				this->DB.RecordCondition.Result>=2&&\
				this->DB.Meas.Result>0&&this->DB.Meas.Line[0]>DB.RecordCondition.Line[1])
			AI_RULE(
				this->DB.RecordCondition.Result>=2&&\
				this->DB.SinglePlsMeas.Result>0&&this->DB.SinglePlsMeas.Line[0]>DB.RecordCondition.Line[1]
			)AI_RULE_END
		)
		{
			if(this->DB.QTMU_PLUS_COARSE.Result>0)
				pParam->Accuracy=_T("?0nS?.1% of reading");
			if(this->DB.QTMU_PLUS_FINE.Result>0)
				pParam->Accuracy=_T("?nS?.1% of reading");
			pParam->Comment=_T("Can't analysis which pin to mark in timing test");
		}
	}
	return 0;
}
//get param by index 
ParamText* CAccoSrcCodeParser::getParamTextStructByIndex(CString& strindex)
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
CString CAccoSrcCodeParser::getParamText(CString& strIn)
{
	CString strRet;
	int index=0;
	for(;index<MAX_PARAMETER;index++)
	{
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




