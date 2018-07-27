#include "StdAfx.h"
#include "SrcCodeParserInterface.h"

//create a TextFIFO
CTextFIFO* fifoInst=new CTextFIFO;

CTextFIFO::CTextFIFO(){}
CTextFIFO::~CTextFIFO()
{
}

//init
void CTextFIFO::Init(void)
{
	this->pIn=0;
	this->iCnt=0;
	for(short i=0;i<MAX_SAVE_LINE;i++)
	{
		this->TEXT[i].clear();
	}
}

//insert
void CTextFIFO::Insert(LPCTSTR lpszInput)
{
	if(iCnt<MAX_SAVE_LINE)
		this->iCnt++;
	this->TEXT[this->pIn].clear();
	this->TEXT[this->pIn]=lpszInput;
	this->pIn++;
	if(this->pIn==MAX_SAVE_LINE)
	{
		this->pIn=0;
	}
}
//get
DWORD CTextFIFO::GetOut(tstring* out)
{
	DWORD iRetCode=0;
	if(this->iCnt<MAX_SAVE_LINE)
	{
		for(int i=0;i<this->iCnt;i++)
		{
			out[i]=this->TEXT[i];
			iRetCode++;
		}
	}
	else if(this->pIn==0)
	{
		for(int i=MAX_SAVE_LINE-1;i>=0;i--)
		{
			out[i]=this->TEXT[i];
			iRetCode++;
		}
	}
	else
	{
		int i=0;
		for(int j=this->pIn;j<MAX_SAVE_LINE;j++,i++)
		{
			out[i]=this->TEXT[j];
			iRetCode++;
		}
		for(int j=0;j<this->pIn;j++,i++)
		{
			out[i]=this->TEXT[j];
			iRetCode++;
		}
	}
	return iRetCode;
}