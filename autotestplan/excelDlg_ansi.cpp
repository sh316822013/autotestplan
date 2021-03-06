// excelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "excel.h"
#include "excelDlg.h"
#include "ExcelOperate.h"

#include "FileManage.h"
#include "CpgsParser.h"
#include "CpdsParser.h"
#include "DataSheet.h"
#include "MapManage.h"
#include "testplanformat.h"
#include "TestProgramParser.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//user MACRO and global variable
#define MAX_LOG_LENTH 65536
//global variable to record initial path
TCHAR szInitDir[MAX_PATH];
//global variable to print log file
CString mstr_Log;
//Excel class instance
Range m_ExlRge;
_Worksheet m_ExlSheet;
Worksheets m_ExlSheets; 
_Workbook m_ExlBook; 
Workbooks m_ExlBooks; 
_Application m_ExlApp; 
VARIANT vtMissing;
COperateExcel* pOE=NULL;
ParamStruct* pParamMap=NULL;//Testplan map pointer
//CpgsParser instance 
CpgsParser* pPgsParser=new CpgsParser;
//CpdsParser instance 
CpdsParser* pPdsParser=new CpdsParser;
//CDataSheet instance 
CDataSheet* pDataSheetParser=new CDataSheet;
//instance of CTestProgramParser
CTestProgramParser* pTestProgramParser=new CTestProgramParser;
//open file name
OPENFILENAME opfn;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CexcelDlg dialog
CexcelDlg::CexcelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CexcelDlg::IDD, pParent)
	, mStrEditInfo(_T(""))
	, m_Progress(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CexcelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_GLOBAL, m_ctrlProgressGlobal);
}

BEGIN_MESSAGE_MAP(CexcelDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_LOADPGS, &CexcelDlg::OnBnClickedBtnLoadds)
	ON_BN_CLICKED(IDC_BTN_GENTESTPLAN, &CexcelDlg::OnBnClickedBtnGentestplan)
	ON_BN_CLICKED(IDC_BTN_SAVECLOSE, &CexcelDlg::OnBnClickedBtnSaveclose)
	ON_BN_CLICKED(ID_BTN_QUIT, &CexcelDlg::OnBnClickedBtnQuit)
//	ON_NOTIFY(BCN_HOTITEMCHANGE, IDC_CHECK_SETTOP, &CexcelDlg::OnBnHotItemChangeCheckSettop)
//	ON_NOTIFY(NM_THEMECHANGED, IDC_CHECK_SETTOP, &CexcelDlg::OnNMThemeChangedCheckSettop)
	ON_BN_CLICKED(IDC_CHECK_SETTOP, &CexcelDlg::OnBnClickedCheckSettop)
	ON_BN_CLICKED(IDC_BTN_LOADDS, &CexcelDlg::OnBnClickedBtnLoadds)
END_MESSAGE_MAP()


// CexcelDlg message handlers

BOOL CexcelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	// TODO: Add extra initialization here
	{
		//initialize progress bar
		this->m_ctrlProgressGlobal.SetRange(0,100);
		this->m_ctrlProgressGlobal.SetStep(1);
		this->m_ctrlProgressGlobal.SetPos(0);
		this->m_ctrlProgressGlobal.ShowWindow(FALSE);
		//get initial path
		GetCurrentDirectory(MAX_PATH,szInitDir);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CexcelDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CexcelDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CexcelDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//ClrEditboxInfo to clear infomation of specfied edit box
void ClrEditboxInfo(CexcelDlg* pDlg,DWORD uID)
{
	CString mStrEditText=_T("");
	pDlg->SetDlgItemText(uID,mStrEditText);
}

//AddEditboxInfo to output infomation to specfied edit box
void AddEditboxInfo(CexcelDlg* pDlg,DWORD uID,LPCTSTR szInfo,INT ifOutputTime=0)
{
	SaveLogs(szInfo);
	CString mStrEditText;
	pDlg->GetDlgItemText(uID,mStrEditText);//get original content
	if(ifOutputTime)
	{
		CTime m_time;  
		m_time=CTime::GetCurrentTime();             //get current date and time  
		CString m_strDateTime=m_time.Format(_T("%Y-%m-%d %H:%M:%S"));   //format date and time  
		mStrEditText+=m_strDateTime;//add date and time	
		mStrEditText+=_T("\r\n");
	}
	mStrEditText+=szInfo;
	mStrEditText+=_T("\r\n");
	//scroll to bottom
	pDlg->SetDlgItemText(uID,mStrEditText);//add new infomation
	pDlg->SendDlgItemMessage(uID, WM_VSCROLL, SB_BOTTOM, 0);
}
//UpdateProgress
void UpdateProgress(CexcelDlg* pDlg,short pos)
{
	pDlg->UpdateData(TRUE);
	if(pos<=100&&pos>=0)
	{
		//initialize progress bar
		pDlg->m_ctrlProgressGlobal.SetPos(pos);
		pDlg->m_ctrlProgressGlobal.ShowWindow(TRUE);
	}
	else
	{
		//initialize progress bar
		pDlg->m_ctrlProgressGlobal.SetPos(0);
		pDlg->m_ctrlProgressGlobal.ShowWindow(FALSE);
		throw (CString*)_T("attempt to write invalid progress to progress bar");
	}
}

//OnBnClickedBtnLoadds will be excuted after click IDC_BTN_LOADPGS button
void CexcelDlg::OnBnClickedBtnLoadds()
{
	// TODO: Add your control notification handler code here
	ClrEditboxInfo(this,IDC_EDIT_INFO);
	AddEditboxInfo(this,IDC_EDIT_INFO,_T("loading datasheet file..."),1);
	//init opfn
	ZeroMemory(&opfn,sizeof(OPENFILENAME));
	if(GetFileName(&opfn,_T("PDS file(*.pds)\0*.pds\0PGS file(*.pgs)\0*.pgs\0All files(*.*)\0*.*\0")))
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("file not selected."));
		return;// not get right file
	}
	TCHAR path[MAX_PATH];
	_tcscpy_s(path,MAX_PATH,opfn.lpstrFile);
	FILE* pDataSheetFile;
	if(_taccess_s(path,4))//0 if exist
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("Invalid file path."));
		return;
	}
	else
	{
		if(_tfopen_s(&pDataSheetFile,path,_T("r")))
		{
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("failed to open datasheet file."));
			return;
		}
		else
		{
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("load datasheet file success."));
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("file: "));
			AddEditboxInfo(this,IDC_EDIT_INFO,path);
		}
	}
	//init parser
	pPgsParser->init();
	pPdsParser->init();
	pDataSheetParser->init();
	//pds or pgs
	try
	{
		CString mstr_Path=path;
		mstr_Path.MakeLower();
		if(mstr_Path.Find(_T(".pgs"))>0)//pgs file
		{
			if(pPgsParser->analysis(pDataSheetFile))//0=parse success
			{
				fclose(pDataSheetFile);
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("failed to analysis pgs file."));
				return;
			}
			else
				pDataSheetParser->loadFromClass(pPgsParser);
		}
		else if(mstr_Path.Find(_T(".pds"))>0)
		{
			if(pPdsParser->analysis(pDataSheetFile))//0=parse success
			{
				fclose(pDataSheetFile);
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("failed to analysis pds file."));
				return;
			}
			else
				pDataSheetParser->loadFromClass(pPdsParser);
		}
		else
		{
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("invalid datasheet file. the extend file name should end with (*.pgs) or (*.pds)."));
		}
	}
	catch(LPCTSTR lpException)
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,lpException);
	}
	fclose(pDataSheetFile);
	CString cstrTmp,cstrPgsInfo(_T("analysis success.\r\n"));
	TCHAR cBuf[MAX_PATH];
	ZeroMemory(cBuf,MAX_PATH*sizeof(TCHAR));
	pDataSheetParser->getProductProgramName(cBuf);
	cstrTmp.Format(_T("project name:%s.\r\n"),cBuf);
	cstrPgsInfo+=cstrTmp;
	cstrTmp.Format(_T("total function:%d.\r\n"),pDataSheetParser->getFuncCnt());
	cstrPgsInfo+=cstrTmp;
	cstrTmp.Format(_T("total parameter:%d."),pDataSheetParser->getAllParamCnt());
	cstrPgsInfo+=cstrTmp;
	AddEditboxInfo(this,IDC_EDIT_INFO,cstrPgsInfo);
	{//parse test.cpp
		tstring strTmpPath=path;
		int pos=strTmpPath.find_last_of(_T("\\"));
		if(pos<0)
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("broken project."));
		tstring strTPpath=strTmpPath.substr(0,pos+1);
		strTPpath+=_T("source\\test.cpp");
		FILE* pTestProgram;
		if(_taccess_s(strTPpath.c_str(),4))//0 if exist
		{
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("can't find source file."));
			return;
		}
		else
		{
			if(_tfopen_s(&pTestProgram,strTPpath.c_str(),_T("r")))
			{
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("failed to open source file."));
			}
			else
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("load source file success."));
		}
		if(pTestProgramParser->init(pTestProgram))//0=parse success
		{
			fclose(pTestProgram);
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("failed to analysis source file."));
			return;
		}
	}
}

void CexcelDlg::OnBnClickedBtnGentestplan()
{
//for debug
	//TCHAR buffer[128];
	//wsprintf(buffer,_T("%d"),sizeof(CpgsParser));
	//AddEditboxInfo(this,IDC_EDIT_INFO,_T("sizeof CpgsParser is"));
	//AddEditboxInfo(this,IDC_EDIT_INFO,buffer);
	//wsprintf(buffer,_T("%d"),sizeof(CDataSheet));
	//AddEditboxInfo(this,IDC_EDIT_INFO,_T("sizeof CDataSheet is"));
	//AddEditboxInfo(this,IDC_EDIT_INFO,buffer);
	//wsprintf(buffer,_T("%d"),sizeof(CTestProgramParser));
	//AddEditboxInfo(this,IDC_EDIT_INFO,_T("sizeof CTestProgramParser is"));
	//AddEditboxInfo(this,IDC_EDIT_INFO,buffer);
	//wsprintf(buffer,_T("%d"),sizeof(FUNC_STR));
	//wsprintf(buffer,_T("%d"),sizeof(PARAM_STR));
//for debug
	ClrEditboxInfo(this,IDC_EDIT_INFO);
	DWORD m_CurrentParam=0;
	//if exist map file
	void* vpOpenMap=__uMAPMANAGE::OpenTestplanMap(_T("Testplan_MapFile"));
#define _DEBUG_WITHOUT_MAP
#ifdef _DEBUG_WITHOUT_MAP
	vpOpenMap=(void*)malloc(65536*1024*10);//640MB
	ZeroMemory(vpOpenMap,65536*1024*10);
	ParamStruct* puDebugPS=(ParamStruct*)vpOpenMap;
	strcpy_s(puDebugPS->Parameter,TESTPLAN_MAX_STRING_LENTH,"0.0");
#endif
	if(!vpOpenMap)
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("can't find map file"),1);
		pParamMap=NULL;
		return;
	}
	else
	{
		pParamMap=(ParamStruct*)vpOpenMap;
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("find map file"));
	}
	vtMissing.vt=VT_NULL;
	if(!pDataSheetParser->getInitState())//check if initiate pgs successful,1:successful,0:fail
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("need select datasheet file first"));
		//ErrorLog(_T("pgs not initialize,"));
		return;
	}
	//if already exist excel application
	if(pOE)
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("instance is already exist"));
		int iUserSelect=::MessageBox(GetSafeHwnd(),   _T("Testplan exist, create new?"),   _T("Warning"),   MB_SYSTEMMODAL   |   MB_OKCANCEL);   
		if(1==iUserSelect)
		{
			try
			{
				pOE->book.SetSaved(TRUE);
				pOE->app.SetVisible(FALSE);
				COleVariant oleFalse;
				oleFalse.vt=VT_BOOL;
				oleFalse.boolVal=FALSE;
				VARIANT vtMissing;
				vtMissing.vt=VT_NULL;
				pOE->book.Close(oleFalse,vtMissing,oleFalse);
				pOE->Release();
				delete(pOE);
				pOE=NULL;
				pOE=new COperateExcel;	
			}
			catch(...)
			{					
				SaveLogs(_T("exception, force save to log"),1);
			}
		}
		else
			return;
	}
	else
	{
		pOE=new COperateExcel;	
	}
	if(!pOE->app.CreateDispatch(_T("Excel.Application"),NULL))
	{ 
		AfxMessageBox(_T("Create Excel Server failed!")); 
		return; 
	} 
	
	pOE->Init();
/*
	if(!pOE)
	{
		pOE=new COperateExcel;	
		if(!pOE->app.CreateDispatch(_T("Excel.Application"),NULL))
		{ 
			AfxMessageBox(_T("Create Excel Server failed!")); 
			return; 
		} 
		pOE->Init();
	}
*/
#pragma region "Write into excel"
	try
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("Export testplan data..."));
		TPF_all uFmtOfFunction;
		TPF_col uFmtCol;
		ZeroMemory(&uFmtOfFunction,sizeof(TPF_all));
		ZeroMemory(&uFmtCol,sizeof(TPF_col));
		{//init uFmtOfFunction parameter
			ExcelCoord pdinfo_s={1,1};
			uFmtOfFunction.pProdInfo_start=pdinfo_s;
			ExcelCoord pdinfo_e={5,1};
			uFmtOfFunction.pProdInfo_end=pdinfo_e;
			{//caculating how many pin used
				int k=0;//k is the column of pin
				TCHAR tszPinName[TESTPLAN_MAX_STRING_LENTH];
				MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[0].PinInfo[k].Name,-1,tszPinName,TESTPLAN_MAX_STRING_LENTH);
				while(tszPinName[0]!=0)//write pin to row(wFuncNameRow+j+1) and column(k)
				{
					k++;
					MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[0].PinInfo[k].Name,-1,tszPinName,TESTPLAN_MAX_STRING_LENTH);
				}
				uFmtOfFunction.pinCnt=k;
			}
		}
		{//init TPF_col parameter
			uFmtCol._pincnt=uFmtOfFunction.pinCnt;
			uFmtCol.FT_Test_num=1;
			uFmtCol.Function_num=uFmtCol.FT_Test_num;
			uFmtCol.WS_Test_num=2;
			uFmtCol.Test_type=3;

			uFmtCol.Test_name=4;
			uFmtCol.Function_name=uFmtCol.Test_name;

			uFmtCol.First_pin=5;
			uFmtCol.Last_pin=uFmtCol.First_pin+uFmtCol._pincnt-1;

			uFmtCol.Typical_value=uFmtCol.Last_pin+1;
			uFmtCol.AccuracyAndRange=uFmtCol.Typical_value+1;
			uFmtCol.iLimitStart=uFmtCol.AccuracyAndRange+1;
			if(pDataSheetParser->iLimitSets>0)
				uFmtCol.iLimitStop=uFmtCol.iLimitStart+pDataSheetParser->iLimitSets*2-1;
			else
				uFmtCol.iLimitStop=uFmtCol.iLimitStart;
			uFmtCol.Unit=uFmtCol.iLimitStop+1;
			uFmtCol.Comments=uFmtCol.Unit+1;

			uFmtCol._totalcolumn=uFmtCol.Comments-uFmtCol.FT_Test_num+1;
			uFmtCol._firstcolumn=uFmtCol.FT_Test_num;
			uFmtCol._lastcolumn=uFmtCol.Comments;	
		}
		//edit testplan here
		pOE->SetVisible(FALSE);
		pOE->DeleteSheet(3);
		pOE->DeleteSheet(2);
		pOE->ChangeSheetName(1,_T("ExportTestPlan"));

		//write product name and MAX MIN TYP voltage
		{
			pOE->SetRange(Int2Rng(uFmtOfFunction.pProdInfo_start),Int2Rng(uFmtOfFunction.pProdInfo_end));
			TCHAR szPdName[MAX_TMP_STR_LENTH];
			pDataSheetParser->getProductProgramName(szPdName);
			pOE->WriteStringToCell(1,1,szPdName);		
			pOE->WriteStringToCell(3,1,_T("Abs voltage:"));		
			pOE->WriteStringToCell(4,1,_T("Max voltage:"));		
			pOE->WriteStringToCell(5,1,_T("Min voltage:"));		
			pOE->range.SetWrapText(COleVariant((short)FALSE));
			//set not auto wrap text
		}

		//write testplan data
		{
			int iParamCnt=pDataSheetParser->getAllParamCnt();
			int iFuncCnt=pDataSheetParser->getFuncCnt();

			//pOE->range.SetWrapText(COleVariant((short)TRUE));
			//pOE->range.SetRowHeight(COleVariant((long)30));
			//pOE->range.SetColumnWidth(COleVariant((long)15));
			//pOE->range.BorderAround(COleVariant((long)1),2,1,vtMissing);
			
			//set A column width to 10
			//CString cstrRngParamNum_Top;
			//CString cstrRngParamNum_Bottom;
			//cstrRngParamNum_Top.Format(_T("A%d"),RNG_PARAM_ROW_START);
			//cstrRngParamNum_Bottom.Format(_T("A%d"),iParamCnt+iFuncCnt+6);
			//pOE->SetRange(cstrRngParamNum_Top,cstrRngParamNum_Bottom);//based on how many parameter
			//pOE->range.SetColumnWidth(COleVariant((long)8));

			//select all param range
			//pOE->SetRange(cstrRngParam_TopLeft,cstrRngParam_BottomRight);//based on how many parameter

			for(int i=0;i<iFuncCnt;i++)
			{
				FUNC_STR* pFunc= pDataSheetParser->getFunc(i);
				//write function index and function name into cell
				COleVariant oleFuncName(COleVariant((LPCTSTR)pFunc->Name));
				DWORD wParamCnt=pFunc->TotalParam;
				PARAM_STR* pParam=pFunc->Parameter;

				//caculating function range
				if(i==0)
				{
					ExcelCoord func_s={uFmtOfFunction.pProdInfo_end.row+1/*one blank rows*/+1,1};
					uFmtOfFunction.pFS[i]=func_s;
					ExcelCoord func_e={0,0};
					func_e.row=func_s.row+wParamCnt+1/*function line*/+1/*pin line*//*blank line*/;

					//func_e.col=1/*FT Test #*/+1/*WS Test #*/+1/*TestType*/+1/*Test Name*/+uFmtOfFunction.pinCnt/*total pin*/+1/*Typical Value*/+\
					//	1/*Accuracy(Range)*/+1/*"WS Limits Min/Max"*/+1/*""QA Limits Min/Max""*/+1/*FT LimitsMin/Max*/+1/*Unit*/+1/*comments*/;
					func_e.col=uFmtCol._lastcolumn;
					uFmtOfFunction.pFE[i]=func_e;
				}
				else
				{
					uFmtOfFunction.pFS[i].row=uFmtOfFunction.pFE[i-1].row+2;
					uFmtOfFunction.pFS[i].col=uFmtCol._firstcolumn;
					uFmtOfFunction.pFE[i].row=uFmtOfFunction.pFS[i].row+wParamCnt+1/*function line*/+1/*pin line*//*blank line*/;
					uFmtOfFunction.pFE[i].col=uFmtCol._lastcolumn;
				}

				//select all param range of function
				pOE->SetRange(Int2Rng(uFmtOfFunction.pFS[i]),Int2Rng(uFmtOfFunction.pFE[i]));//based on how many parameter
				//pOE->range.SetNumberFormat(COleVariant((long)_T("@"))); //set number format to text"@"

				//set all row height to autofit
				//pOE->range.Group(COleVariant(Int2Rng(uFmtOfFunction.pFS[i])),COleVariant(Int2Rng(uFmtOfFunction.pFE[i])),COleVariant((short)TRUE),COleVariant((short)TRUE));
				//Range FuncRows=pOE->range.GetEntireRow();
				//FuncRows.Group();

				pOE->WriteToCell(1,uFmtCol.Function_num,COleVariant((long)(i+1)));//function number			
				pOE->WriteToCell(1,uFmtCol.Function_name/*func name column*/,COleVariant((LPCTSTR)pFunc->Name));//function name	
				//write test type and limit type
				pOE->WriteToCell(2,uFmtCol.FT_Test_num/*FT Test #*/,COleVariant((LPCTSTR)_T("FT Test #")));//FT Test
				pOE->WriteToCell(2,uFmtCol.WS_Test_num/*WS Test #*/,COleVariant((LPCTSTR)_T("WS Test #")));//WS Test	
				pOE->WriteToCell(2,uFmtCol.Test_type/*Test type*/,COleVariant((LPCTSTR)_T("Test type")));//Test type		
				pOE->WriteToCell(2,uFmtCol.Test_name/*Test name*/,COleVariant((LPCTSTR)_T("Test name")));//Test type		
				{//write pin name			
					int k=0;//k is the column of pin
					TCHAR tszPinName[TESTPLAN_MAX_STRING_LENTH];
					MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[0].PinInfo[k].Name,-1,tszPinName,TESTPLAN_MAX_STRING_LENTH);
					while(tszPinName[0]!=0)//write pin to row(wFuncNameRow+j+1) and column(k)
					{
						pOE->WriteToCell(2,uFmtCol.First_pin+k,COleVariant((CString)tszPinName));		
						k++;
						MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[0].PinInfo[k].Name,-1,tszPinName,TESTPLAN_MAX_STRING_LENTH);
					}
				}
				pOE->WriteToCell(2,uFmtCol.Typical_value/*Typical Value*/,COleVariant((LPCTSTR)_T("Typical Value")));//Test type		
				pOE->WriteToCell(2,uFmtCol.AccuracyAndRange/*Accuracy(Range)*/,COleVariant((LPCTSTR)_T("Accuracy(Range)")));//Accuracy(Range)
				//write in limit sets
				for(int iIndex=0;iIndex<pDataSheetParser->iLimitSets;iIndex++)
				{
					pOE->WriteToCell(2,uFmtCol.iLimitStart+iIndex*2,COleVariant(pDataSheetParser->uLimitSetName[iIndex].LowLimitName));
					pOE->WriteToCell(2,uFmtCol.iLimitStart+iIndex*2+1,COleVariant(pDataSheetParser->uLimitSetName[iIndex].HighLimitName));
				}
				pOE->WriteToCell(2,uFmtCol.Unit/*Unit*/,COleVariant((LPCTSTR)_T("Unit")));//Unit
				pOE->WriteToCell(2,uFmtCol.Comments/*COMMENTS*/,COleVariant((LPCTSTR)_T("COMMENTS")));//COMMENTS
				//set menu bold
				pOE->SetRange(Int2Rng(uFmtOfFunction.pFS[i]),Int2Rng(uFmtOfFunction.pFS[i].row+1,uFmtOfFunction.pFE[i].col));//based on how many parameter
				Font ft;
				ft.AttachDispatch(pOE->range.GetFont());
				ft.SetBold(COleVariant((long)1));
				//set menu interior color
				pOE->SetRange(Int2Rng(uFmtOfFunction.pFS[i].row+1,uFmtOfFunction.pFS[i].col),Int2Rng(uFmtOfFunction.pFS[i].row+1,uFmtOfFunction.pFE[i].col));//based on how many parameter
				Interior it; 
				it.AttachDispatch(pOE->range.GetInterior()); 
				it.SetColorIndex(COleVariant((long)48));  //color 
				//set column width and border
				pOE->SetRange(Int2Rng(uFmtOfFunction.pFS[i].row+1/*title bar's row*/,uFmtCol._firstcolumn),Int2Rng(uFmtOfFunction.pFE[i].row-1,uFmtOfFunction.pFE[i].col));//based on how many parameter
				pOE->range.SetWrapText(COleVariant((short)TRUE));
				pOE->range.SetColumnWidth(COleVariant((long)13));
				pOE->range.BorderAround(COleVariant((long)1),3,1,vtMissing);
				//set pinstatus and pin name border
				pOE->SetRange(Int2Rng(uFmtOfFunction.pFS[i].row+1,uFmtCol.First_pin),Int2Rng(uFmtOfFunction.pFE[i].row-1,uFmtCol.Last_pin));//based on how many parameter
				pOE->range.BorderAround(COleVariant((long)1/*line style*/),3/*weight*/,1/*color index*/,vtMissing);
				//start to write pinstatus
				pOE->SetRange(Int2Rng(uFmtOfFunction.pFS[i]),Int2Rng(uFmtOfFunction.pFE[i]));//based on how many parameter
				for(int j=0;j<wParamCnt;j++)
				{
					//write Test_num index and Test_name into cell
					CString ParamNum,subNum;
					ParamNum.Format(_T("%d"),i);
					subNum.Format(_T("%d"),j);
					ParamNum+=_T(".");
					ParamNum+=subNum;
					pOE->WriteToCell(3+j,uFmtCol.FT_Test_num,COleVariant((CString)(ParamNum+_T(" "))));	//write Test_num of FT WS QA	
					pOE->WriteToCell(3+j,uFmtCol.Test_name,COleVariant((LPCTSTR)(pParam+j)->ParamName));//write Test_name
					//find corresponding parameter to write in
					TCHAR tszParamNum[TESTPLAN_MAX_STRING_LENTH];
					int iCnt=0;
					for(;pParamMap[iCnt].Parameter[0]!=0&&iCnt<TESTPLAN_MAX_PARAM;iCnt++)
					{
						MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[iCnt].Parameter,-1,tszParamNum,TESTPLAN_MAX_STRING_LENTH);
						if(ParamNum==tszParamNum)
						{
							//record how many params have been finded
							m_CurrentParam++;

							//write limit into cell
							CString strHiLimit,strLolimit,strUnit;
							for(int iIndex=0;iIndex<pDataSheetParser->iLimitSets;iIndex++)
							{
								strLolimit=pFunc->Parameter[j].LowLimit[iIndex][0]==0?_T(""):pFunc->Parameter[j].LowLimit[iIndex]/*[0] for acco use*/;
								strHiLimit=pFunc->Parameter[j].HighLimit[iIndex][0]==0?_T(""):pFunc->Parameter[j].HighLimit[iIndex]/*[0] for acco use*/;
								pOE->WriteToCell(3+j,uFmtCol.iLimitStart+iIndex*2,COleVariant(strLolimit));
								pOE->WriteToCell(3+j,uFmtCol.iLimitStart+iIndex*2+1,COleVariant(strHiLimit));
							}

							//strLolimit=pFunc->Parameter[j].LowLimit[0][0]==0?_T(""):pFunc->Parameter[j].LowLimit[0]/*[0] for acco use*/;
							//strHiLimit=pFunc->Parameter[j].HighLimit[0][0]==0?_T(""):pFunc->Parameter[j].HighLimit[0]/*[0] for acco use*/;
							//strUnit=pFunc->Parameter[j].Unit==0?_T(""):pFunc->Parameter[j].Unit;
							//CString strIntoCell=_T("(")+strLolimit+_T(", ")+strHiLimit+_T(")");
							//if(strIntoCell==_T("(, )"))	strIntoCell=_T("");//clear invalid limit
							//pOE->WriteToCell(3+j,uFmtCol.iLimitStop/*FT LimitsMin/Max*/,COleVariant(strIntoCell));		
							//write unit into cell
							CString strIntoCell=strUnit;
							pOE->WriteToCell(3+j,uFmtCol.Unit,COleVariant(strIntoCell));		
							//insert "hide" comment here
							Range CmtRng=pOE->range.GetRange(COleVariant(Int2Rng(2/*fun num and pin name*/+j+1,uFmtCol._firstcolumn)),COleVariant(Int2Rng(2/*fun num and pin name*/+j+1,uFmtCol._firstcolumn)));
							CmtRng.AddComment(COleVariant(pTestProgramParser->getParamText(ParamNum)));
							//parse method and pin
							pTestProgramParser->AI_ParserParameter(ParamNum,&strUnit);
							ParamText* pParamText=pTestProgramParser->getParamTextStructByIndex(ParamNum);//for find measure pin
							if(!pParamText)
							{
								throw (CString*)_T("Please choose .pgs or .pds file in project folder\nAnd make sure add \"EnableRecord()\" and \"RecordCondition()\" in to project!");
							}
							//write in accuracy except for voltage and current
							if(!pParamText->Accuracy.empty())
								pOE->WriteToCell(3+j,uFmtCol.AccuracyAndRange,COleVariant((CString)(pParamText->Accuracy.c_str())));		
							//write in comment 
							if(!pParamText->Comment.empty())
							{
								pOE->WriteToCell(3+j,uFmtCol.Comments,COleVariant((CString)(pParamText->Comment.c_str())));	
								//set comment font color to red
								Range CmtRng_last=pOE->range.GetRange(COleVariant(Int2Rng(3+j,uFmtCol.Comments)),COleVariant(Int2Rng(3+j,uFmtCol.Comments)));
								Font ft;
								ft.AttachDispatch(CmtRng_last.GetFont()); 
								ft.SetColorIndex(COleVariant((long)3));
							}
							//write pin infomation
							int k=0;//k is the column of pin
							TCHAR tszPinName[TESTPLAN_MAX_STRING_LENTH];
							MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[iCnt].PinInfo[k].Name,-1,tszPinName,TESTPLAN_MAX_STRING_LENTH);
							while(tszPinName[0]!=0)//write pin to row(wFuncNameRow+j+1) and column(k)
							{
								TCHAR tszPinStat[TESTPLAN_MAX_STRING_LENTH];
								MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[iCnt].PinInfo[k].Stat,-1,tszPinStat,TESTPLAN_MAX_STRING_LENTH);
								pOE->WriteToCell(3+j,uFmtCol.First_pin+k,COleVariant((CString)(tszPinStat)));
								//set measure pin color
								for(int iPMcnt=0;!pParamText->PinMark[iPMcnt].empty();iPMcnt++)
								{
									if(pParamText->PinMark[iPMcnt]==tszPinName)
									{
										//add measure method
										TCHAR tszPinStat[TESTPLAN_MAX_STRING_LENTH];
										MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[iCnt].PinInfo[k].Stat,-1,tszPinStat,TESTPLAN_MAX_STRING_LENTH);
										CString cstrPinstat=tszPinStat;
										cstrPinstat+=_T("\n");
										cstrPinstat+=pParamText->PinMethod[iPMcnt].c_str();
										pOE->WriteToCell(3+j,uFmtCol.First_pin+k,COleVariant((CString)(cstrPinstat)));		
										//write in accuracy
										pOE->WriteToCell(3+j,uFmtCol.AccuracyAndRange,COleVariant((CString)(pTestProgramParser->GetAccuracy(tszPinStat,pParamText->PinMethod[iPMcnt]))));		
										//set Interior
										Range itRng=pOE->range.GetRange(COleVariant(Int2Rng(3+j,uFmtCol.First_pin+k)),COleVariant(Int2Rng(3+j,uFmtCol.First_pin+k)));
										Interior it; 
										it.AttachDispatch(itRng.GetInterior()); 
										it.SetColorIndex(COleVariant((long)36));  //color 
										Font ft;
										ft.AttachDispatch(itRng.GetFont()); 
										ft.SetColorIndex(COleVariant((long)3));
									}
								}
								//next pin
								k++;
								MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[iCnt].PinInfo[k].Name,-1,tszPinName,TESTPLAN_MAX_STRING_LENTH);
							}
							break;
						}
					}
					if(pParamMap[iCnt].Parameter[0]==0)//not find corresponding parameter map
					{
						CString strProg=_T("can't find parameter map infomation\n");
						strProg+=_T("param in pgs:");
						strProg+=ParamNum;
						AddEditboxInfo(this,IDC_EDIT_INFO,strProg);
					}
					else
					{
						//update progress		
						pTestProgramParser->Param[0];
						float fProgress=(float)m_CurrentParam/pDataSheetParser->getAllParamCnt();
						fProgress*=100;
						UpdateProgress(this,(DWORD)fProgress);
						CString strProg,strTmp;
						strTmp.Format(_T("%d"),m_CurrentParam);
						strProg=strTmp;
						strProg+=_T("/");
						strTmp.Format(_T("%d"),pDataSheetParser->getAllParamCnt());
						strProg+=strTmp;
						AddEditboxInfo(this,IDC_EDIT_INFO,strProg);
					}
				}
			}
			//set global format
			{
				//find last row
				DWORD index=0;
				for(;index<1024;index++)
				{
					if(uFmtOfFunction.pFE[index].row==0)
						break;
				}
				if(index==1024)
					throw (CString*)_T("Fail to find last line");
				DWORD iLastRow=uFmtOfFunction.pFE[index-1].row;
				//set column 1-3 width
				pOE->SetRange(Int2Rng(1,1),Int2Rng(iLastRow,3));//based on how many parameter
				pOE->range.SetColumnWidth(COleVariant((long)6));
				//set column 4 width
				pOE->SetRange(Int2Rng(1,4),Int2Rng(iLastRow,4));//based on how many parameter
				pOE->range.SetColumnWidth(COleVariant((long)20));
				//pOE->range.SetRowHeight(COleVariant((long)0));
				//set all row height to autofit
				pOE->SetRange(Int2Rng(1,1),Int2Rng(iLastRow,uFmtOfFunction.pFE[0].col));//based on how many parameter
				Range allColumns=pOE->range.GetEntireRow();
				allColumns.AutoFit();
				//set all column and row to align middle
				pOE->range.SetHorizontalAlignment(COleVariant((long)-4108)); 
				pOE->range.SetVerticalAlignment(COleVariant((long)-4108)); 
				//show final sheet
				pOE->SetVisible(TRUE);
			}
		}
		{//show finished message		
			CString strFinishMsg=_T("finish");
			CString strFinded;
			strFinded.Format(_T("%d"),m_CurrentParam);
			CString strAllCnt;
			strAllCnt.Format(_T("%d"),pDataSheetParser->getAllParamCnt());
			strFinishMsg=strFinishMsg+strFinded+_T(" of ")+strAllCnt+_T(" parameters");
			AddEditboxInfo(this,IDC_EDIT_INFO,strFinishMsg);
			this->m_ctrlProgressGlobal.ShowWindow(FALSE);
		}
	}	
	//deal with exception
	catch(COleException   *e)   
	{   
		TCHAR   buf[1024];           //   For   the   Try...Catch   error   message.   
		_stprintf_s(buf,   _T("COleException.   SCODE:   %08lx."),   (long)e->m_sc);   
		::MessageBox(GetSafeHwnd(),   buf,   _T("COleException"),   MB_SETFOREGROUND   |   MB_OK);   
		SaveLogs(buf,1);
		SaveLogs(_T("exception type: COleException, force save"),1);
	}   
	catch(COleDispatchException   *e)   
	{   
		TCHAR   buf[1024];           //   For   the   Try...Catch   error   message.   
		_stprintf_s(buf,   
			_T("COleDispatchException.   SCODE:   %08lx,   Description:   \"%s\"."),   
			(long)e->m_wCode,(LPSTR)e->m_strDescription.GetBuffer(512));   
		::MessageBox(GetSafeHwnd(),   buf,   _T("COleDispatchException"),   
			MB_SETFOREGROUND   |   MB_OK);   
		SaveLogs(buf,1);
		SaveLogs(_T("exception type: COleDispatchException, force save"),1);
	}   
	catch(CString* e)   
	{   
		::MessageBox(GetSafeHwnd(),  (LPCTSTR)e,   _T("CString exception"),   
			MB_SETFOREGROUND   |   MB_OK);   
		SaveLogs( (LPCTSTR)e,1);
		SaveLogs(_T("exception type: CString, force save"),1);
	}   
	catch(...)   
	{   
		::MessageBox(GetSafeHwnd(),   _T("excel exception"),   _T("Excel error"),   
			MB_SETFOREGROUND   |   MB_OK);   
		SaveLogs(_T("exception type: all, force save"),1);
	}   
#pragma endregion "Write into excel"
#ifdef _DEBUG_WITHOUT_MAP
	free(vpOpenMap);
#endif

}
void CexcelDlg::OnBnClickedBtnSaveclose()
{
	ClrEditboxInfo(this,IDC_EDIT_INFO);
	AddEditboxInfo(this,IDC_EDIT_INFO,_T("save and close."),1);
	SaveLogs(_T("save and close force save"),1);
	if(!pOE)
		return;
	try
	{
		OPENFILENAME saveOpfn;
		if(!SaveFileName(&saveOpfn,_T("xlsx")))
		{
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("file not saved"));
			return;// not get right file
		}
		TCHAR szSaveFilePathName[MAX_PATH];
		_tcscpy_s(szSaveFilePathName,saveOpfn.lpstrFile);
		pOE->sheet.SaveAs(szSaveFilePathName,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing);
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("save file success"));
		pOE->book.SetSaved(TRUE);
		pOE->app.SetVisible(FALSE);
		COleVariant oleFalse;
		oleFalse.vt=VT_BOOL;
		oleFalse.boolVal=FALSE;
		VARIANT vtMissing;
		vtMissing.vt=VT_NULL;
		pOE->book.Close(oleFalse,vtMissing,oleFalse);
		pOE->Release();
		delete(pOE);
		pOE=NULL;
	}
	catch(...)   
	{   
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("file not saved"));
		SaveLogs(_T("exception type: all, force save"),1);
		pOE->book.SetSaved(TRUE);
		pOE->app.SetVisible(FALSE);
		COleVariant oleFalse;
		oleFalse.vt=VT_BOOL;
		oleFalse.boolVal=FALSE;
		VARIANT vtMissing;
		vtMissing.vt=VT_NULL;
		pOE->book.Close(oleFalse,vtMissing,oleFalse);
		pOE->Release();
		delete(pOE);
		pOE=NULL;
	}   
}

void nullfunc()
{
	if(!m_ExlApp.CreateDispatch(_T("Excel.Application"),NULL)) 
	{ 
		AfxMessageBox(_T("Create Excel Server failed!")); 
		return; 
	}
	//set visible 
	m_ExlApp.SetVisible(TRUE); 

	///////////////////get this program path/////////////////// 
	CString theAppPath,theAppName; 
	TCHAR Path[MAX_PATH]; 
	GetModuleFileName(NULL,Path,MAX_PATH);//get application path
	theAppName=AfxGetApp()->m_pszAppName; 
	theAppName+=_T(".exe"); 
	//delete file name 
	int length1,length2; 
	length1=theAppPath.GetLength(); 
	length2=theAppName.GetLength(); 

	theAppPath.Delete(length1-length2,length2); 
	//////////////////////////////////////////////////////////////// 

	CString TempPath=_T(""); 

	//TempPath=theAppPath+_T("Template.xls");//EXCEL template path 
	TempPath=theAppPath;
	//attach workbooks,workbook,worksheets
	m_ExlBooks.AttachDispatch(m_ExlApp.GetWorkbooks(),TRUE); 
	m_ExlBook.AttachDispatch(m_ExlBooks.Add((COleVariant)TempPath),TRUE);//load EXCEL template 
	m_ExlSheets.AttachDispatch(m_ExlBook.GetSheets(),TRUE);//load Sheet page 

	//change name of first Sheet page to TestSheet 
	m_ExlSheet.AttachDispatch(m_ExlSheets.GetItem(COleVariant((long)1)),TRUE); 
	m_ExlSheet.SetName(_T("Testplan")); 

	//write time int cell A1-F4
	//load cells need to merge 
	m_ExlRge.AttachDispatch(m_ExlSheet.GetRange(COleVariant(_T("A1")),COleVariant(_T("F1"))),TRUE); 
	m_ExlRge.Merge(COleVariant((long)0)); 
	m_ExlRge.AttachDispatch(m_ExlSheet.GetCells(),TRUE);//load all cells 
	CTime m_time;  
	m_time=CTime::GetCurrentTime();             //get current date and time  
	CString m_strDateTime=m_time.Format(_T("%Y-%m-%d %H:%M:%S"));   //format date and time  
	m_ExlRge.SetItem(COleVariant((long)1),COleVariant((long)1),COleVariant(m_strDateTime)); 
	m_ExlRge.SetHorizontalAlignment(COleVariant((long)-4131)); //horizontal alignment:left
	m_ExlRge.SetVerticalAlignment(COleVariant((long)-4108)); //vertical alignment:middle

	//write product name to A2-F2
	m_ExlRge.AttachDispatch(m_ExlSheet.GetRange(COleVariant(_T("A2")),COleVariant(_T("F2"))),TRUE); 
	//load cells need to merge 
	m_ExlRge.Merge(COleVariant((long)0)); 
	m_ExlRge.AttachDispatch(m_ExlSheet.GetCells(),TRUE);//load all cells 
	TCHAR szProductName[MAX_PATH];
	if(!pDataSheetParser->getProductProgramName(szProductName))
	{
		m_ExlRge.SetItem(COleVariant((long)2),COleVariant((long)1),COleVariant(szProductName)); 
		m_ExlRge.SetHorizontalAlignment(COleVariant((long)-4131)); //horizontal alignment:left
		m_ExlRge.SetVerticalAlignment(COleVariant((long)-4108)); //vertical alignment:middle
	}

	//write product process to A3-F3
	m_ExlRge.AttachDispatch(m_ExlSheet.GetRange(COleVariant(_T("A3")),COleVariant(_T("F3"))),TRUE); 
	//load cells need to merge 
	m_ExlRge.Merge(COleVariant((long)0)); 
	m_ExlRge.AttachDispatch(m_ExlSheet.GetCells(),TRUE);//load all cells 
	CString m_strProcess=_T("Process:  ");
	//m_strProcess+=_T("BCM09C  ");
	m_strProcess+=_T("ABS:");
	//m_strProcess+=_T("26V  ");
	m_strProcess+=_T("MAX:");
	//m_strProcess+=_T("24V  ");
	m_strProcess+=_T("TYP:");
	//m_strProcess+=_T("12V  ");
	m_strProcess+=_T("MIN:");
	//m_strProcess+=_T("14.2V  ");
	m_ExlRge.SetItem(COleVariant((long)3),COleVariant((long)1),COleVariant(m_strProcess)); 
	m_ExlRge.SetHorizontalAlignment(COleVariant((long)-4131)); //horizontal alignment:left
	m_ExlRge.SetVerticalAlignment(COleVariant((long)-4108)); //vertical alignment:middle

	//TCHAR szSaveFilePath[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH,szSaveFilePath);
	//CString szSaveFilePathName=szSaveFilePath;
	//TCHAR szSaveName[MAX_PATH];
	//pDataSheetParser->getProductProgramName(szSaveName);
	//szSaveFilePathName+=_T("\\");
	//szSaveFilePathName+=szSaveName;
	//szSaveFilePathName+=_T(".xls");
	
	OPENFILENAME saveOpfn;
	if(!SaveFileName(&saveOpfn,_T("xlsx")))
	{
		//MessageBox(_T("Please input file name, file not saved"),_T("Warnning"),MB_OK);
		return;// not get right file
	}
	TCHAR szSaveFilePathName[MAX_PATH];
	_tcscpy_s(szSaveFilePathName,saveOpfn.lpstrFile);
	m_ExlSheet.SaveAs(szSaveFilePathName,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing);
	//release resource (most importance!!!)
	m_ExlRge.ReleaseDispatch(); 
	m_ExlSheet.ReleaseDispatch(); 
	m_ExlSheets.ReleaseDispatch(); 
	m_ExlBook.ReleaseDispatch(); 
	m_ExlBooks.ReleaseDispatch(); 
	//m_ExlApp must release, otherwise there will be a excel process stays in ram, and when repeatly run the program, it will occur fatal error. 
	m_ExlApp.ReleaseDispatch(); 

	//quit program
	m_ExlApp.Quit(); 
}


void CexcelDlg::OnBnClickedBtnQuit()
{
	ClrEditboxInfo(this,IDC_EDIT_INFO);
	AddEditboxInfo(this,IDC_EDIT_INFO,_T("quit."),1);
	SaveLogs(_T("quit, force to save"),1);
	//release below resource to avoid memory leakage
	delete(pDataSheetParser);
	pDataSheetParser=NULL;
	delete(pPgsParser);
	pPgsParser=NULL;
	delete(pPdsParser);
	pPdsParser=NULL;
	delete(pTestProgramParser);
	pTestProgramParser=NULL;


	//release excel resource first
	if(pOE)
	{
		BOOL isSaved=0;
		try
		{
			//pOE->app.GetActiveChart();
			isSaved=pOE->book.GetSaved();
		}
		catch(COleException   *e)//if excel has been closed by manually
		{
			//e->ReportError();
			TCHAR szErrorInfo[MAX_PATH];
			e->GetErrorMessage(szErrorInfo,MAX_PATH);
			e->Delete();
			SaveLogs(szErrorInfo,1);
			SaveLogs(_T("exception type:COleException, force to save"),1);
			pOE->Release();

			delete(pOE);
			pOE=NULL;
			isSaved=1;
			PostMessage(WM_QUIT,NULL,NULL);
			return;
		}
		if(!isSaved)
		{
			int iUserSelect=::MessageBox(GetSafeHwnd(),   _T("Save file?"),   _T("File not saved"),   
				MB_SETFOREGROUND   |   MB_OKCANCEL);   
			if(1==iUserSelect)
			{
				try
				{
					OPENFILENAME saveOpfn;
					if(SaveFileName(&saveOpfn,_T("xlsx")))
					{
						TCHAR szSaveFilePathName[MAX_PATH];
						_tcscpy_s(szSaveFilePathName,saveOpfn.lpstrFile);
						pOE->sheet.SaveAs(szSaveFilePathName,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing);
					}
					else
					{
						::MessageBox(GetSafeHwnd(),   _T("quit without saving"),   _T("warnning"),   MB_SETFOREGROUND   |   MB_OK);   
					}
					pOE->book.SetSaved(TRUE);
					pOE->app.SetVisible(FALSE);
					COleVariant oleFalse;
					oleFalse.vt=VT_BOOL;
					oleFalse.boolVal=FALSE;
					VARIANT vtMissing;
					vtMissing.vt=VT_NULL;
					pOE->book.Close(oleFalse,vtMissing,oleFalse);
					pOE->Release();
					delete(pOE);
					pOE=NULL;
					PostMessage(WM_QUIT,NULL,NULL);
				}
				catch(...)
				{
					::MessageBox(GetSafeHwnd(),   _T("quit without saving"),   _T("warnning"),   MB_SETFOREGROUND   |   MB_OK);   
					SaveLogs(_T("exception type: all. quit without saving, force save"),1);
					pOE->book.SetSaved(TRUE);
					pOE->app.SetVisible(FALSE);
					COleVariant oleFalse;
					oleFalse.vt=VT_BOOL;
					oleFalse.boolVal=FALSE;
					VARIANT vtMissing;
					vtMissing.vt=VT_NULL;
					pOE->book.Close(oleFalse,vtMissing,oleFalse);
					pOE->Release();
					delete(pOE);
					pOE=NULL;
					PostMessage(WM_QUIT,NULL,NULL);
				}
			}
			else
			{
				pOE->app.SetVisible(FALSE);
				COleVariant oleFalse;
				oleFalse.vt=VT_BOOL;
				oleFalse.boolVal=FALSE;
				VARIANT vtMissing;
				vtMissing.vt=VT_NULL;
				pOE->book.Close(oleFalse,vtMissing,oleFalse);
				pOE->Release();
				delete(pOE);
				pOE=NULL;
				PostMessage(WM_QUIT,NULL,NULL);
			}
		}
		else
		{
			pOE->app.SetVisible(FALSE);
			COleVariant oleFalse;
			oleFalse.vt=VT_BOOL;
			oleFalse.boolVal=FALSE;
			VARIANT vtMissing;
			vtMissing.vt=VT_NULL;
			pOE->book.Close(oleFalse,vtMissing,oleFalse);
			pOE->Release();
			delete(pOE);
			pOE=NULL;
			PostMessage(WM_QUIT,NULL,NULL);
		}
	}
	else
	{
		PostMessage(WM_QUIT,NULL,NULL);
	}
}

void CexcelDlg::OnBnClickedCheckSettop()
{
	ClrEditboxInfo(this,IDC_EDIT_INFO);
	// TODO: Add your control notification handler code here
	if(this->IsDlgButtonChecked(IDC_CHECK_SETTOP))
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("set top."),1);
		::SetWindowPos(this->m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
	else
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("set not top."),1);
		::SetWindowPos(this->m_hWnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
}
//save log in AddEditboxInfo() and exception catch
#include <direct.h>
void SaveLogs(CString m_strInBuf,int forceSave)
{
	CTime m_time;  
	m_time=CTime::GetCurrentTime();             //get current date and time  
	CString m_strDate=m_time.Format(_T("%Y%m%d"));   //format date for log file name
	CString m_strDateTime=m_time.Format(_T("%Y-%m-%d %H:%M:%S"));   //format date and time  
	mstr_Log+=m_strInBuf;
	mstr_Log+=_T("\n");
	if(forceSave||mstr_Log.GetLength()>MAX_LOG_LENTH-1)
	{	
		FILE* pLog;
		CString filepath=szInitDir;
		filepath+=_T("\\log\\");
		int pathcheck=_taccess_s(filepath.GetBuffer(),6);//if path is R and W
		if(0==pathcheck)
		{
			CString filename=filepath+m_strDate+_T(".log");
			_tfopen_s(&pLog,filename.GetBuffer(),_T("a+"));	
		}
		else
		{
			int dircreate=_tmkdir(filepath.GetBuffer());
			if(0==dircreate)
			{
				CString filename=filepath+m_strDate+_T(".log");
				_tfopen_s(&pLog,filename.GetBuffer(),_T("a+"));	
			}
		}
		if(pLog)
		{
			mstr_Log+=_T("------------------time line:above content are saved to log at ");
			mstr_Log+=m_strDateTime;
			mstr_Log+=_T("\n");
			_ftprintf_s(pLog,mstr_Log.GetBuffer());
			mstr_Log+=_T("");
			fclose(pLog);
		}
	}
}
void SaveLogs(LPTSTR lpBuf,int forceSave)
{
	CString mstr_buf=lpBuf;
	SaveLogs(mstr_buf,forceSave);
}

