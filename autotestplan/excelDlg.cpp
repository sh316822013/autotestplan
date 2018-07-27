// excelDlg.cpp : implementation file
//

#include <stdio.h>
#include "stdafx.h"
#include "excel.h"
#include "excelDlg.h"
#include "ExcelOperate.h"

#include "FileManage.h"
#include "MapManage.h"
#include "testplanformat.h"
#include "AccoSrcCodeParser.h"
#include "EagleSrcCodeParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
CSrcCodeParserInterface* pTestProgramParser=0;
CAccoSrcCodeParser instAccoSrcCodeParser ;
CEagleSrcCodeParser instEagleSrcCodeParser ;
// CAboutDlg dialog used for App About
//get current exe directory
DWORD GetCurrentEXEDirectory(LPTSTR lpszOutBuf,DWORD dwSize_of_t);

//runcmd function
BOOL RunCMD(const TCHAR *lpcszCMD,const TCHAR* lpcszParam,DWORD dwMilliSeconds,int nShow);


void SetupComListenning()
{
	if(pOE->app.m_lpDispatch!=NULL)
		return; //already started
	TCHAR lpszBuff[MAX_PATH];
	COleException e;
	//if(!pOE->app.CreateDispatch(_T("Excel.Application"),&e))
	if(!pOE->app.CreateDispatch(_T("Word.Application.8"),&e))
	{
		_stprintf(lpszBuff,_T("Error on CreateDispatch():%ld(%08lx)"),e.m_sc,e.m_sc);
		return;
	}
	
	// {84255CA8-8D82-4da9-BCE3-6BFA69E23371}
	static const GUID IID_IExcelAppEvents = 
	{ 0x000209f7, 0x000, 0x0000, { 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46} };
	HRESULT hr;
	//Get server's IConnectionPointContainer interface
	IConnectionPointContainer* pConnPtContainer;
	IConnectionPoint* m_pConnectionPoint;
	hr=pOE->app.m_lpDispatch->QueryInterface(IID_IExcelAppEvents,(void**)&pConnPtContainer);
	ASSERT(!FAILED(hr));
	//find connection point for specified event
	hr=pConnPtContainer->FindConnectionPoint(IID_IExcelAppEvents,&m_pConnectionPoint);
}


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
	, blEnableVerboseMode(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CexcelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_GLOBAL, m_ctrlProgressGlobal);
	DDX_Control(pDX, IDC_Verbose, mctrl_Verbose);
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
	ON_BN_CLICKED(IDC_Verbose, &CexcelDlg::OnBnClickedVerbose)
	ON_COMMAND(ID_FILE_LOADDS, &CexcelDlg::OnFileLoadds)
	ON_COMMAND(ID_FILE_LOADIMAGE32782, &CexcelDlg::OnFileLoadimage32782)
	ON_COMMAND(ID_FILE_EXIT32783, &CexcelDlg::OnFileExit32783)
	ON_COMMAND(ID_HELP_INTRODUCTION, &CexcelDlg::OnHelpQuickStart)
	ON_COMMAND(ID_ABOUT_VERSION32784, &CexcelDlg::OnAboutVersion32784)
	ON_COMMAND(ID_FILE_ADDRMTHDL, &CexcelDlg::OnFileAddToolTo)
	ON_COMMAND(ID_FILE_SAVEAS, &CexcelDlg::OnFileSaveas)
END_MESSAGE_MAP()


// CexcelDlg message handlers
int ConvertEaglePragram(LPCTSTR lpszFilePath);
BOOL CexcelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//init com envirment
	HRESULT hRes=0;
	hRes=CoInitialize(NULL);
	if(hRes!=S_OK)
	{
		AfxMessageBox(_T("init COM environment failed"));
		return FALSE;
	}
	if(NewConsole())
	{
		_tprintf(_T("log system start running\n"));
		//testlua();
		//LoadProjectParam();
		_tprintf(_T("init COM environment success\n"));
	}
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
	this->pMenu=new CMenu();
	this->pMenu->LoadMenu(IDR_MENU1);
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
		//set verbose mode default
		this->mctrl_Verbose.SetCheck(BST_UNCHECKED);
		this->blEnableVerboseMode=false;
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
	CEdit* pEdit=(CEdit*)pDlg->GetDlgItem(IDC_EDIT_INFO);
	int nLength=pEdit->GetWindowTextLength();
	if(nLength>MAX_LOG_LENTH)
	{
		pEdit->SetWindowText(_T(""));
		nLength=pEdit->GetWindowTextLength();
	}
	pEdit->SetSel(nLength+1,nLength+1);

	CString mStrEditText=_T("");
	if(ifOutputTime&&pDlg->blEnableVerboseMode)
	{
		CTime m_time;  
		m_time=CTime::GetCurrentTime();             //get current date and time  
		CString m_strDateTime=m_time.Format(_T("%Y-%m-%d %H:%M:%S"));   //format date and time  
		mStrEditText+=m_strDateTime;//add date and time	
		mStrEditText+=_T("\r\n");
	}
	if(pDlg->blEnableVerboseMode)
	{
		mStrEditText+=szInfo;
		mStrEditText+=_T("\n");
	}
	else
	{
		CString cstrTmp=szInfo;
		if(-1!=cstrTmp.Find(_T("<<<<"))||-1!=cstrTmp.Find(_T(">>>>")))
		{
			mStrEditText+=szInfo;
			mStrEditText+=_T("\n");
		}
	}
	SaveLogs(szInfo);
	pEdit->ReplaceSel(mStrEditText);
	//scroll to bottom
	//pDlg->SetDlgItemText(uID,mStrEditText);//add new infomation
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
	AddEditboxInfo(this,IDC_EDIT_INFO,_T(">>>>loading datasheet file"),1);
	TCHAR szSelPath[MAX_PATH]=_T("");
	if(GetFileName(_T("PDS file(*.pds)\0*.pds\0PGS file(*.pgs)\0*.pgs\0All files(*.*)\0*.*\0"),szSelPath,MAX_PATH))
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("file not selected"));
		return;// not get right file
	}
	TCHAR path[MAX_PATH];
	_tcscpy_s(path,MAX_PATH,szSelPath);
	FILE* pDataSheetFile;
	if(_taccess_s(path,4))//0 if exist
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("Invalid file path"));
		return;
	}
	else
	{
		if(_tfopen_s(&pDataSheetFile,path,_T("r")))
		{
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
			return;
		}
		else
		{
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("datasheet file:"));
			AddEditboxInfo(this,IDC_EDIT_INFO,path);
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));
		}
	}
	//init parser
	pPgsParser->init();
	pPdsParser->init();
	pDataSheetParser->init();
	//pds or pgs
	try
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T(">>>>analysis datasheet file"));
		TCHAR szFilePath[MAX_PATH];
		ZeroMemory(szFilePath,MAX_PATH*sizeof(TCHAR));
		_tcscpy_s(szFilePath,MAX_PATH,path);
		s2lower(szFilePath);
		if(_tcsstr(szFilePath,_T(".pgs")))//pgs file
		{
			fclose(pDataSheetFile);
			if(pPgsParser->analysis(szFilePath))//0=parse success
			{
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
				return;
			}
			else
				pDataSheetParser->loadFromClass(pPgsParser,PROJTYPE_ACCO);
		}
		else if(_tcsstr(szFilePath,_T(".pds")))
		{
			fclose(pDataSheetFile);
			if(pPdsParser->analysis(szFilePath))//0=parse success
			{
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
				return;
			}
			else
				pDataSheetParser->loadFromClass(pPdsParser,PROJTYPE_EAGLE);
		}
		else
		{
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("invalid datasheet file. file type must be (*.pgs) or (*.pds)"));
		}
	}
	catch(LPCTSTR lpException)
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,lpException);
	}
	fclose(pDataSheetFile);
	{
		TCHAR szInfo[MAX_PATH];
		TCHAR szProj[MAX_PATH];
		ZeroMemory(szInfo,MAX_PATH*sizeof(TCHAR));
		ZeroMemory(szProj,MAX_PATH*sizeof(TCHAR));
		pDataSheetParser->getProductProgramName(szProj);
		_stprintf_s(szInfo,MAX_PATH,_T("program:%s"),szProj);
		AddEditboxInfo(this,IDC_EDIT_INFO,szInfo);
		_stprintf_s(szInfo,MAX_PATH,_T("function:%d"),pDataSheetParser->getFuncCnt());
		AddEditboxInfo(this,IDC_EDIT_INFO,szInfo);
		_stprintf_s(szInfo,MAX_PATH,_T("parameter:%d"),pDataSheetParser->getAllParamCnt());
		AddEditboxInfo(this,IDC_EDIT_INFO,szInfo);
		_stprintf_s(szInfo,MAX_PATH,_T("<<<<success"));
		AddEditboxInfo(this,IDC_EDIT_INFO,szInfo);
	}
	{//analysis source code
		AddEditboxInfo(this,IDC_EDIT_INFO,_T(">>>>loading source file"));
		instAccoSrcCodeParser.init();
		instEagleSrcCodeParser.init();
		pTestProgramParser=&instAccoSrcCodeParser;
		TCHAR szFolder[MAX_PATH];
		TCHAR szProjName[MAX_PATH];
		ZeroMemory(szFolder,MAX_PATH*sizeof(TCHAR));
		ZeroMemory(szProjName,MAX_PATH*sizeof(TCHAR));
		_tcscpy_s(szFolder,MAX_PATH,path);
		_tcscpy_s(szProjName,MAX_PATH,path);
		//find folder
		trimrol(szFolder,_T("\\"));
		trimlol(szFolder,_T("\\"));
		//find project name
		trimrol(szProjName,_T("."));
		trimlol(szProjName,_T("\\"));
		if(!_tcsstr(szProjName,szFolder))
		{
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("unable to find project folder by datasheet file name"));
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("source code analysis module will not work when generating testplan"));
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
			return;
		}
		else
		{
			if(0==_tcscmp(PROJTYPE_ACCO,pDataSheetParser->szProjectType))
			{
				TCHAR szSrcCodePath[MAX_PATH];
				ZeroMemory(szSrcCodePath,MAX_PATH*sizeof(TCHAR));
				_tcscpy_s(szSrcCodePath,MAX_PATH,path);
				trimrol(szSrcCodePath,_T("\\"));
				_tcscat_s(szSrcCodePath,MAX_PATH,_T("\\source\\test.cpp"));
				FILE* pTestProgram;
				if(_taccess_s(szSrcCodePath,4))//0 if exist
				{
					AddEditboxInfo(this,IDC_EDIT_INFO,_T("can't find source file"));
					AddEditboxInfo(this,IDC_EDIT_INFO,_T("source code analysis module will not work when generating testplan"));
					AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
					return;
				}
				else
				{
					AddEditboxInfo(this,IDC_EDIT_INFO,_T("find source file \"test.cpp\""));
					if(_tfopen_s(&pTestProgram,szSrcCodePath,_T("r")))
					{
						AddEditboxInfo(this,IDC_EDIT_INFO,_T("can't open source file "));
						AddEditboxInfo(this,IDC_EDIT_INFO,_T("source code analysis module will not work when generating testplan"));
						AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
						return;
					}
					else
					{
						AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));
					}
				}
				AddEditboxInfo(this,IDC_EDIT_INFO,_T(">>>>analysising source file"));
				if(!instAccoSrcCodeParser.analysis(pTestProgram,pDataSheetParser))//0=parse success
				{
					//pTestProgramParser=&instAccoSrcCodeParser;
					fclose(pTestProgram);
					AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));
				}
				else
				{
					fclose(pTestProgram);
					AddEditboxInfo(this,IDC_EDIT_INFO,_T("can't analysis source file"));
					AddEditboxInfo(this,IDC_EDIT_INFO,_T("source code analysis module will not work when generating testplan"));
					AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
					return;
				}
			}
			if(0==_tcscmp(PROJTYPE_EAGLE,pDataSheetParser->szProjectType))
			{
				TCHAR szSrcCodePath[MAX_PATH];
				ZeroMemory(szSrcCodePath,MAX_PATH*sizeof(TCHAR));
				_tcscpy_s(szSrcCodePath,MAX_PATH,path);
				trimrol(szSrcCodePath,_T("."));
				_tcscat_s(szSrcCodePath,MAX_PATH,_T(".cpp"));
				FILE* pTestProgram=0;
				if(_taccess_s(szSrcCodePath,4))//0 if exist
				{
					AddEditboxInfo(this,IDC_EDIT_INFO,_T("can't find source file"));
					AddEditboxInfo(this,IDC_EDIT_INFO,_T("source code analysis module will not work when generating testplan"));
					AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
					return;
				}
				else
				{
					AddEditboxInfo(this,IDC_EDIT_INFO,_T("find source file:"));
					AddEditboxInfo(this,IDC_EDIT_INFO,szSrcCodePath);
					if(_tfopen_s(&pTestProgram,szSrcCodePath,_T("r")))
					{
						AddEditboxInfo(this,IDC_EDIT_INFO,_T("can't open source file "));
						AddEditboxInfo(this,IDC_EDIT_INFO,_T("source code analysis module will not work when generating testplan"));
						AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
						return;
					}
					else
					{
						AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));
					}
				}
				AddEditboxInfo(this,IDC_EDIT_INFO,_T(">>>>analysising source file"));
				if(!instEagleSrcCodeParser.analysis(pTestProgram,pDataSheetParser))//0=parse success
				{
					pTestProgramParser=&instEagleSrcCodeParser;
					fclose(pTestProgram);
					pTestProgram=0;
					AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));
				}
				else
				{
					fclose(pTestProgram);
					pTestProgram=0;
					AddEditboxInfo(this,IDC_EDIT_INFO,_T("can't analysis source file"));
					AddEditboxInfo(this,IDC_EDIT_INFO,_T("source code analysis module will not work when generating testplan"));
					AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
					return;
				}
			}
		}
	}
}
void CexcelDlg::OnBnClickedBtnGentestplan()
{
	int dwTimeElapse=GetTickCount();

//for debug
		//pOE=new COperateExcel;	
		//SetupComListenning();
//for debug
	ClrEditboxInfo(this,IDC_EDIT_INFO);
	int iCurrentParamNum=0;
	int iSuccessParamNum=0;
	//if exist map file
	AddEditboxInfo(this,IDC_EDIT_INFO,_T(">>>>searching testplan data"),1);
	void* vpOpenMap=__uMAPMANAGE::OpenTestplanMap(_T("Testplan_MapFile"));
//#define _DEBUG_WITHOUT_MAP
#ifdef _DEBUG_WITHOUT_MAP
	vpOpenMap=(void*)malloc(65536*1024*10);//640MB
	ZeroMemory(vpOpenMap,65536*1024*10);
	ParamStruct* puDebugPS=(ParamStruct*)vpOpenMap;
	for(int i=0;i<100;i++)
	{
		string strNum="0.";
		char buf[10];
		strNum+=itoa(i,buf,10);
		strcpy_s(puDebugPS->Parameter,RMT_MAX_PARAMINDEX_STR_LENTH,strNum.c_str());	
		puDebugPS++;
	}
#endif
	if(!vpOpenMap)
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("no test plan data found"),1);
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"),1);
		pParamMap=NULL;
		return;
	}
	else
	{
		pParamMap=(ParamStruct*)vpOpenMap;
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"),1);
	}
	vtMissing.vt=VT_NULL;
	AddEditboxInfo(this,IDC_EDIT_INFO,_T(">>>>preparing datasheet info"));
	if(!pDataSheetParser->getInitState())//check if initiate pgs successful,1:successful,0:fail
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("no datasheet file selected"));
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
		return;
	}
	else
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));
/*
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
*/
	if(!pOE)
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T(">>>>creating excel server"));
		pOE=new COperateExcel;	
		COleException e;
		//deprecated, inilized in ondlginit() function
		//HRESULT hRes=0;
		//hRes=CoInitialize(NULL);
		//if(hRes!=S_OK)
		//{
		//	AddEditboxInfo(this,IDC_EDIT_INFO,_T("init COM environment failed"));
		//	AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
		//	return;
		//}
		if(!pOE->app.CreateDispatch(_T("excel.application"),&e))
		{ 
			TCHAR lpszBuff[MAX_PATH];
			_stprintf(lpszBuff,_T("Error on CreateDispatch():%ld(%08lx)"),e.m_sc,e.m_sc);
			AddEditboxInfo(this,IDC_EDIT_INFO,lpszBuff);
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("creat excel server failed, make sure this computer has installed excel software"));
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
			return; 
		} 
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("excel server process started"));
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));
		pOE->Init();
		pOE->DeleteSheet(3);
		pOE->DeleteSheet(2);
		pOE->ChangeSheetName(1,_T("TestPlanexported"));
	}
	else
	{
		pOE->sheets.Add(vtMissing,vtMissing,COleVariant((short)1),vtMissing); 
		pOE->app.SetDisplayAlerts(0);
		pOE->DeleteSheet(2);
		pOE->app.SetDisplayAlerts(1);
		pOE->ChangeSheetName(1,_T("TestPlanexported"));
	}
#pragma region "Write into excel"
	try
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T(">>>>exporting testplan data"));
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
				TCHAR tszPinName[RMT_MAX_PINNAME_STR_LENTH]=_T("");
#ifdef _UNICODE
				MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[0].PinInfo[k].Name,-1,tszPinName,RMT_MAX_PINNAME_STR_LENTH);
#else 
				_tcscpy_s(tszPinName,RMT_MAX_PINNAME_STR_LENTH,pParamMap[0].PinInfo[k].Name);
#endif	
				while(tszPinName[0]!=0)//write pin to row(wFuncNameRow+j+1) and column(k)
				{
					k++;
#ifdef _UNICODE
					MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[0].PinInfo[k].Name,-1,tszPinName,RMT_MAX_PINNAME_STR_LENTH);
#else 
					_tcscpy_s(tszPinName,RMT_MAX_PINNAME_STR_LENTH,pParamMap[0].PinInfo[k].Name);
#endif	
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

			uFmtCol.RelayStat=uFmtCol.Last_pin+1;

			uFmtCol.Typical_value=uFmtCol.RelayStat+1;
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

				//set data type to text
				pOE->SetRange(Int2Rng(uFmtOfFunction.pFS[i]),Int2Rng(uFmtOfFunction.pFE[i].row,uFmtCol.iLimitStart-1));//based on how many parameter
				pOE->range.SetNumberFormat(COleVariant(_T("@"))); 
				//select all param range of function
				pOE->SetRange(Int2Rng(uFmtOfFunction.pFS[i]),Int2Rng(uFmtOfFunction.pFE[i]));//based on how many parameter
				//set all row height to autofit
				//pOE->range.Group(COleVariant(Int2Rng(uFmtOfFunction.pFS[i])),COleVariant(Int2Rng(uFmtOfFunction.pFE[i])),COleVariant((short)TRUE),COleVariant((short)TRUE));
				//Range FuncRows=pOE->range.GetEntireRow();
				//FuncRows.Group();

				pOE->WriteToCell(1,uFmtCol.Function_num,COleVariant((LPCTSTR)pFunc->szFuncIndex));//function number			
				pOE->WriteToCell(1,uFmtCol.Function_name/*func name column*/,COleVariant((LPCTSTR)pFunc->Name));//function name	
				//write test type and limit type
				pOE->WriteToCell(2,uFmtCol.FT_Test_num/*FT Test #*/,COleVariant((LPCTSTR)_T("FT Test #")));//FT Test
				pOE->WriteToCell(2,uFmtCol.WS_Test_num/*WS Test #*/,COleVariant((LPCTSTR)_T("WS Test #")));//WS Test	
				pOE->WriteToCell(2,uFmtCol.Test_type/*Test type*/,COleVariant((LPCTSTR)_T("Test type")));//Test type		
				pOE->WriteToCell(2,uFmtCol.Test_name/*Test name*/,COleVariant((LPCTSTR)_T("Test name")));//Test type		
				{//write pin name
					int k=0;//k is the column of pin
					TCHAR tszPinName[RMT_MAX_PINNAME_STR_LENTH];
#ifdef _UNICODE
				MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[0].PinInfo[k].Name,-1,tszPinName,RMT_MAX_PINNAME_STR_LENTH);
#else 
				_tcscpy_s(tszPinName,RMT_MAX_PINNAME_STR_LENTH,pParamMap[0].PinInfo[k].Name);
#endif	
					while(tszPinName[0]!=0)//write pin to row(wFuncNameRow+j+1) and column(k)
					{
						pOE->WriteToCell(2,uFmtCol.First_pin+k,COleVariant((CString)tszPinName));		
						k++;
#ifdef _UNICODE
						MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[0].PinInfo[k].Name,-1,tszPinName,RMT_MAX_PINNAME_STR_LENTH);
#else 
						_tcscpy_s(tszPinName,RMT_MAX_PINNAME_STR_LENTH,pParamMap[0].PinInfo[k].Name);
#endif	
					}
				}
				pOE->WriteToCell(2,uFmtCol.RelayStat/*RelayStat*/,COleVariant((LPCTSTR)_T("RelayStat")));	
				pOE->WriteToCell(2,uFmtCol.Typical_value/*Typical Value*/,COleVariant((LPCTSTR)_T("Typical Value")));//Test type		
				pOE->WriteToCell(2,uFmtCol.AccuracyAndRange/*Accuracy(Range)*/,COleVariant((LPCTSTR)_T("Accuracy")));//Accuracy(Range)
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
				it.SetColorIndex(COleVariant((long)COLORINDEX_GREY));  //color 
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
					//record how many params have been finded
					iCurrentParamNum++;
					//write Test_num index and Test_name into cell
					//CString ParamNum,subNum;
					//ParamNum.Format(_T("%d"),i);
					//subNum.Format(_T("%d"),j);
					//ParamNum+=_T(".");
					//ParamNum+=subNum;
					//pOE->WriteToCell(3+j,uFmtCol.FT_Test_num,COleVariant((CString)(ParamNum+_T(" "))));	//write Test_num of FT WS QA	
					CString cstrParamNum=(pParam+j)->ParamIndex;
					pOE->WriteToCell(3+j,uFmtCol.FT_Test_num,COleVariant((LPCTSTR)cstrParamNum));	//write Test_num of parameter
					pOE->WriteToCell(3+j,uFmtCol.Test_name,COleVariant((LPCTSTR)(pParam+j)->ParamName));//write Test_name
					//find corresponding parameter to write in
					TCHAR tszParamNum[RMT_MAX_PARAMINDEX_STR_LENTH];
					int iCnt=0;
					for(;pParamMap[iCnt].Parameter[0]!=0&&iCnt<RMT_MAX_TESTPLAN_PARAM;iCnt++)
					{
#ifdef _UNICODE
						MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[iCnt].Parameter,-1,tszParamNum,RMT_MAX_PINNAME_STR_LENTH);
#else 
						_tcscpy_s(tszParamNum,RMT_MAX_PARAMINDEX_STR_LENTH,pParamMap[iCnt].Parameter);
#endif	
						if(cstrParamNum==tszParamNum)
						{
							iSuccessParamNum++;
							//write limit into cell
							CString strHiLimit,strLolimit,strUnit;
							for(int iIndex=0;iIndex<pDataSheetParser->iLimitSets;iIndex++)
							{
								strLolimit=pFunc->Parameter[j].LowLimit[iIndex][0]==0?_T(""):pFunc->Parameter[j].LowLimit[iIndex]/*[0] for acco use*/;
								strHiLimit=pFunc->Parameter[j].HighLimit[iIndex][0]==0?_T(""):pFunc->Parameter[j].HighLimit[iIndex]/*[0] for acco use*/;
								pOE->WriteToCell(3+j,uFmtCol.iLimitStart+iIndex*2,COleVariant(strLolimit));
								pOE->WriteToCell(3+j,uFmtCol.iLimitStart+iIndex*2+1,COleVariant(strHiLimit));
							}
							strUnit=pFunc->Parameter[j].Unit==0?_T(""):pFunc->Parameter[j].Unit;
							//write unit into cell
							CString strIntoCell=strUnit;
							pOE->WriteToCell(3+j,uFmtCol.Unit,COleVariant(strIntoCell));		
//1.insert "hide" comment here------------finish
							Range CmtRng=pOE->range.GetRange(COleVariant(Int2Rng(2/*fun num and pin name*/+j+1,uFmtCol._firstcolumn)),COleVariant(Int2Rng(2/*fun num and pin name*/+j+1,uFmtCol._firstcolumn)));
							CmtRng.AddComment(COleVariant(pTestProgramParser->getParamText(cstrParamNum)));
							//parse method and pin
							pTestProgramParser->AI_ParserParameter(cstrParamNum,&strUnit);
							ParamText* pParamText=pTestProgramParser->getParamTextStructByIndex(cstrParamNum);//for find measure pin
//1.5write RelayStat into cell
							CString strRelayStatIntoCell=pParamText->RelayStat.c_str();
							pOE->WriteToCell(3+j,uFmtCol.RelayStat,COleVariant(strRelayStatIntoCell));		
							//user write param in RecordCondition() have first priority
							{
								if(pParamMap[iCnt].HighLightPin[0]!=0)
								{
									TCHAR buff[RMT_MAX_HIGHLIGHTPIN];
#ifdef _UNICODE
									MB2WC(pParamMap[iCnt].HighLightPin,buff);
#else 
									_tcscpy_s(buff,RMT_MAX_HIGHLIGHTPIN,pParamMap[iCnt].HighLightPin);
#endif	
									int i=_tcslen(buff);
									TCHAR szSepBuff[4][RMT_MAX_HIGHLIGHTPIN];
									int dwIndexOfSep[3]={0,0,0};
									int dwCntOfSep=0;
									for(int j=0;j<i;j++)
									{
										if(buff[j]==_T(',')&&dwCntOfSep<3)
										{
											dwIndexOfSep[dwCntOfSep]=j;
											dwCntOfSep++;
											buff[j]=0;
										}
									}
									pParamText->PinMark[0]=&buff[0];
									for(int j=0;j<dwCntOfSep;j++)
										pParamText->PinMark[j+1]=&buff[dwIndexOfSep[j]+1];
								}
								if(pParamMap[iCnt].MeasMode[0]!=0)
								{
									TCHAR buff[RMT_MAX_MEASMODE];
#ifdef _UNICODE
									MB2WC(pParamMap[iCnt].MeasMode,buff);
#else 
									_tcscpy_s(buff,RMT_MAX_MEASMODE,pParamMap[iCnt].MeasMode);
#endif	
									int i=_tcslen(buff);
									TCHAR szSepBuff[4][RMT_MAX_HIGHLIGHTPIN];
									int dwIndexOfSep[3]={0,0,0};
									int dwCntOfSep=0;
									for(int j=0;j<i;j++)
									{
										if(buff[j]==_T(',')&&dwCntOfSep<3)
										{
											dwIndexOfSep[dwCntOfSep]=j;
											dwCntOfSep++;
											buff[j]=0;
										}
									}
									pParamText->PinMethod[0]=&buff[0];
									for(int j=0;j<dwCntOfSep;j++)
										pParamText->PinMethod[j+1]=&buff[dwIndexOfSep[j]+1];
								}
								if(pParamMap[iCnt].Comments[0]!=0)
								{
									TCHAR buff[RMT_MAX_COMMENTS];
#ifdef _UNICODE
									MB2WC(pParamMap[iCnt].Comments,buff);
#else 
									_tcscpy_s(buff,RMT_MAX_COMMENTS,pParamMap[iCnt].Comments);
#endif	
									pParamText->Comment=buff;
								}
							}
//////////////////////2.write in accuracy except for voltage and current------------not finish will be deleted
////////////////////							if(!pParamText->Accuracy.empty())
////////////////////								pOE->WriteToCell(3+j,uFmtCol.AccuracyAndRange,COleVariant((CString)(pParamText->Accuracy.c_str())));		
//3.write in comment------------finish
							if(!pParamText->Comment.empty())
							{
								if(pParamText->Comment.length()<MAX_EXCEL_TEXT_TO_SHOW)//if comment length <32, show directly
								{
									pOE->WriteToCell(3+j,uFmtCol.Comments,COleVariant((CString)(pParamText->Comment.c_str())));	
									////set comment font color to red
									//Range CmtRng_last=pOE->range.GetRange(COleVariant(Int2Rng(3+j,uFmtCol.Comments)),COleVariant(Int2Rng(3+j,uFmtCol.Comments)));
									//Font ft;
									//ft.AttachDispatch(CmtRng_last.GetFont()); 
									//ft.SetColorIndex(COleVariant((long)COLORINDEX_RED));
								}
								else
								{
									pOE->WriteToCell(3+j,uFmtCol.Comments,COleVariant((CString)(_T("..."))));	
									//set comment font color to red
									Range CmtRng_last=pOE->range.GetRange(COleVariant(Int2Rng(3+j,uFmtCol.Comments)),COleVariant(Int2Rng(3+j,uFmtCol.Comments)));
									Interior it; 
									it.AttachDispatch(CmtRng_last.GetInterior()); 
									it.SetColorIndex(COleVariant((long)COLORINDEX_ORANGE));  //color 
									Font ft;
									ft.AttachDispatch(CmtRng_last.GetFont()); 
									ft.SetColorIndex(COleVariant((long)COLORINDEX_BLUE));
									CmtRng_last.AddComment(COleVariant(pParamText->Comment.c_str()));
								}
							}
							//write pin infomation
							int k=0;//k is the column of pin
							TCHAR tszPinName[RMT_MAX_PINNAME_STR_LENTH];
#ifdef _UNICODE
							MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[iCnt].PinInfo[k].Name,-1,tszPinName,RMT_MAX_PINNAME_STR_LENTH);
#else 
							_tcscpy_s(tszPinName,RMT_MAX_PINNAME_STR_LENTH,pParamMap[iCnt].PinInfo[k].Name);
#endif	
							while(tszPinName[0]!=0)//write pin to row(wFuncNameRow+j+1) and column(k)
							{
								TCHAR tszPinStat[RMT_MAX_STAT_STR_LENTH];
#ifdef _UNICODE
								MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[iCnt].PinInfo[k].Stat,-1,tszPinStat,RMT_MAX_STAT_STR_LENTH);
#else 
								_tcscpy_s(tszPinStat,RMT_MAX_STAT_STR_LENTH,pParamMap[iCnt].PinInfo[k].Stat);
#endif	
								pOE->WriteToCell(3+j,uFmtCol.First_pin+k,COleVariant((CString)(tszPinStat)));
//4.set measure pin color------------finish
								CString cstrPinstat=_T("");
								for(int iPMcnt=0;!pParamText->PinMark[iPMcnt].empty();iPMcnt++)
								{
									if(pParamText->PinMark[iPMcnt]==tszPinName)
									{
//5.add measure method------------finish
										TCHAR tszPinStat[RMT_MAX_STAT_STR_LENTH];
#ifdef _UNICODE
								MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[iCnt].PinInfo[k].Stat,-1,tszPinStat,RMT_MAX_STAT_STR_LENTH);
#else 
								_tcscpy_s(tszPinStat,RMT_MAX_STAT_STR_LENTH,pParamMap[iCnt].PinInfo[k].Stat);
#endif	
										if(cstrPinstat.IsEmpty())
										{
											cstrPinstat=tszPinStat;
											cstrPinstat+=_T("\n");
										}
										else
										{
											cstrPinstat+=_T(",");
										}
										cstrPinstat+=pParamText->PinMethod[iPMcnt].c_str();
										pOE->WriteToCell(3+j,uFmtCol.First_pin+k,COleVariant((CString)(cstrPinstat)));		
//2.find and write in accuracy------------finish
										//fill in pinstat to pParamText
										pParamText->PinStats[iPMcnt]=tszPinStat;
										tstring strAccy;
										pTestProgramParser->GetAccuracy(strAccy,pParamText);
										if(!strAccy.empty())
										{
											int iTextLen=strAccy.length();
											if(iTextLen<MAX_EXCEL_TEXT_TO_SHOW)
											{
												pOE->WriteToCell(3+j,uFmtCol.AccuracyAndRange,COleVariant((CString)(strAccy.c_str())));		
											}
											else
											{
												pOE->WriteToCell(3+j,uFmtCol.AccuracyAndRange,COleVariant((CString)(_T("..."))));	
												//set comment font color to red
												Range CmtAccy=pOE->range.GetRange(COleVariant(Int2Rng(3+j,uFmtCol.AccuracyAndRange)),COleVariant(Int2Rng(3+j,uFmtCol.AccuracyAndRange)));
												LPDISPATCH cstrComment=CmtAccy.GetComment();
												if(!cstrComment)//if already has comment, then do not add comments
													CmtAccy.AddComment(COleVariant((CString)strAccy.c_str()));
											}
										}
										//set Interior
										Range itRng=pOE->range.GetRange(COleVariant(Int2Rng(3+j,uFmtCol.First_pin+k)),COleVariant(Int2Rng(3+j,uFmtCol.First_pin+k)));
										Interior it; 
										it.AttachDispatch(itRng.GetInterior()); 
										it.SetColorIndex(COleVariant((long)COLORINDEX_LIGHTYELLOW));  //color 
										Font ft;
										ft.AttachDispatch(itRng.GetFont()); 
										//ft.SetColorIndex(COleVariant((long)COLORINDEX_RED));
										//set border
										//itRng.BorderAround(COleVariant((long)1),2,1,vtMissing);//setborder
									}
								}
								//next pin
								k++;
#ifdef _UNICODE
								MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pParamMap[iCnt].PinInfo[k].Name,-1,tszPinName,RMT_MAX_PINNAME_STR_LENTH);
#else 
								_tcscpy_s(tszPinName,RMT_MAX_PINNAME_STR_LENTH,pParamMap[iCnt].PinInfo[k].Name);
#endif	
							}
							break;
						}
					}
					//update progress		
					{
						int  iAllParamCnt=pDataSheetParser->getAllParamCnt();
						int iProgress=iCurrentParamNum*100/iAllParamCnt;
						//iProgress*=100;
						UpdateProgress(this,iProgress);
						//Sleep(5);
						TCHAR szFinishInfo[MAX_PATH];
						//not find corresponding parameter map
						if(pParamMap[iCnt].Parameter[0]==0)
							_stprintf_s(szFinishInfo,MAX_PATH,_T("%d/%d:%s----fail"),iCurrentParamNum,iAllParamCnt,cstrParamNum.GetBuffer());
						else
							_stprintf_s(szFinishInfo,MAX_PATH,_T("%d/%d:%s----success"),iCurrentParamNum,iAllParamCnt,cstrParamNum.GetBuffer());
						AddEditboxInfo(this,IDC_EDIT_INFO,szFinishInfo);
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
				//set column 4-end width
				pOE->SetRange(Int2Rng(1,4),Int2Rng(iLastRow,uFmtOfFunction.pFE[0].col));//based on how many parameter
				pOE->range.SetColumnWidth(COleVariant((long)MAX_EXCEL_TEXT_TO_SHOW));
				//pOE->range.SetRowHeight(COleVariant((long)0));
				//set all row height to autofit
				pOE->SetRange(Int2Rng(1,1),Int2Rng(iLastRow,uFmtOfFunction.pFE[0].col));//based on how many parameter
				Range allRow=pOE->range.GetEntireRow();
				//allRow.SetRowHeight(COleVariant((short)1));
				//allRow.SetVerticalAlignment();
				allRow.AutoFit();
				//allRow.SetShrinkToFit(COleVariant((short)1));
				Range allColumns=pOE->range.GetEntireColumn();
				allColumns.AutoFit();
				//set all column and row to align middle
				pOE->range.SetHorizontalAlignment(COleVariant((long)-4108)); 
				pOE->range.SetVerticalAlignment(COleVariant((long)-4108)); 
				//show final sheet
				pOE->SetVisible(TRUE);
			}
		}
		{//show finished message
			TCHAR szFinishInfo[MAX_PATH];
			int  iAllParamCnt=pDataSheetParser->getAllParamCnt();
			_stprintf_s(szFinishInfo,MAX_PATH,_T("finish %d of %d paramenters.\r\nsuccess:%d\r\nfail:%d"),iCurrentParamNum,iAllParamCnt,iSuccessParamNum,iAllParamCnt-iSuccessParamNum);
			AddEditboxInfo(this,IDC_EDIT_INFO,szFinishInfo);
			dwTimeElapse=GetTickCount()-dwTimeElapse;
			double fSecond=dwTimeElapse/1000.0;
			_stprintf_s(szFinishInfo,MAX_PATH,_T("<<<<time elapsed:%f seconds"),fSecond);
			AddEditboxInfo(this,IDC_EDIT_INFO,szFinishInfo);
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));
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
		TCHAR   buf[1024]=_T("");           //   For   the   Try...Catch   error   message.   
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
		TCHAR lpFileName[MAX_PATH]=_T("");
		if(pDataSheetParser->getProductProgramName(lpFileName))
			_tcscpy_s(lpFileName,MAX_PATH,_T("default_testplan_export.xlsx"));
		else
			_stprintf_s(lpFileName,MAX_PATH,_T("%s_testplan_export.xlsx"),lpFileName);
		if(!SaveFileName(&saveOpfn,lpFileName,_T("xlsx")))
		{
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("file not saved"));
			return;// not get right file
		}
		TCHAR szSaveFilePathName[MAX_PATH];
		_tcscpy_s(szSaveFilePathName,MAX_PATH,saveOpfn.lpstrFile);
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

//void nullfunc()
//{
//	if(!m_ExlApp.CreateDispatch(_T("Excel.Application"),NULL)) 
//	{ 
//		AfxMessageBox(_T("Create Excel Server failed!")); 
//		return; 
//	}
//	//set visible 
//	m_ExlApp.SetVisible(TRUE); 
//
//	///////////////////get this program path/////////////////// 
//	CString theAppPath,theAppName; 
//	TCHAR Path[MAX_PATH]; 
//	GetModuleFileName(NULL,Path,MAX_PATH);//get application path
//	theAppName=AfxGetApp()->m_pszAppName; 
//	theAppName+=_T(".exe"); 
//	//delete file name 
//	int length1,length2; 
//	length1=theAppPath.GetLength(); 
//	length2=theAppName.GetLength(); 
//
//	theAppPath.Delete(length1-length2,length2); 
//	//////////////////////////////////////////////////////////////// 
//
//	CString TempPath=_T(""); 
//
//	//TempPath=theAppPath+_T("Template.xls");//EXCEL template path 
//	TempPath=theAppPath;
//	//attach workbooks,workbook,worksheets
//	m_ExlBooks.AttachDispatch(m_ExlApp.GetWorkbooks(),TRUE); 
//	m_ExlBook.AttachDispatch(m_ExlBooks.Add((COleVariant)TempPath),TRUE);//load EXCEL template 
//	m_ExlSheets.AttachDispatch(m_ExlBook.GetSheets(),TRUE);//load Sheet page 
//
//	//change name of first Sheet page to TestSheet 
//	m_ExlSheet.AttachDispatch(m_ExlSheets.GetItem(COleVariant((long)1)),TRUE); 
//	m_ExlSheet.SetName(_T("Testplan")); 
//
//	//write time int cell A1-F4
//	//load cells need to merge 
//	m_ExlRge.AttachDispatch(m_ExlSheet.GetRange(COleVariant(_T("A1")),COleVariant(_T("F1"))),TRUE); 
//	m_ExlRge.Merge(COleVariant((long)0)); 
//	m_ExlRge.AttachDispatch(m_ExlSheet.GetCells(),TRUE);//load all cells 
//	CTime m_time;  
//	m_time=CTime::GetCurrentTime();             //get current date and time  
//	CString m_strDateTime=m_time.Format(_T("%Y-%m-%d %H:%M:%S"));   //format date and time  
//	m_ExlRge.SetItem(COleVariant((long)1),COleVariant((long)1),COleVariant(m_strDateTime)); 
//	m_ExlRge.SetHorizontalAlignment(COleVariant((long)-4131)); //horizontal alignment:left
//	m_ExlRge.SetVerticalAlignment(COleVariant((long)-4108)); //vertical alignment:middle
//
//	//write product name to A2-F2
//	m_ExlRge.AttachDispatch(m_ExlSheet.GetRange(COleVariant(_T("A2")),COleVariant(_T("F2"))),TRUE); 
//	//load cells need to merge 
//	m_ExlRge.Merge(COleVariant((long)0)); 
//	m_ExlRge.AttachDispatch(m_ExlSheet.GetCells(),TRUE);//load all cells 
//	TCHAR szProductName[MAX_PATH];
//	if(!pDataSheetParser->getProductProgramName(szProductName))
//	{
//		m_ExlRge.SetItem(COleVariant((long)2),COleVariant((long)1),COleVariant(szProductName)); 
//		m_ExlRge.SetHorizontalAlignment(COleVariant((long)-4131)); //horizontal alignment:left
//		m_ExlRge.SetVerticalAlignment(COleVariant((long)-4108)); //vertical alignment:middle
//	}
//
//	//write product process to A3-F3
//	m_ExlRge.AttachDispatch(m_ExlSheet.GetRange(COleVariant(_T("A3")),COleVariant(_T("F3"))),TRUE); 
//	//load cells need to merge 
//	m_ExlRge.Merge(COleVariant((long)0)); 
//	m_ExlRge.AttachDispatch(m_ExlSheet.GetCells(),TRUE);//load all cells 
//	CString m_strProcess=_T("Process:  ");
//	//m_strProcess+=_T("BCM09C  ");
//	m_strProcess+=_T("ABS:");
//	//m_strProcess+=_T("26V  ");
//	m_strProcess+=_T("MAX:");
//	//m_strProcess+=_T("24V  ");
//	m_strProcess+=_T("TYP:");
//	//m_strProcess+=_T("12V  ");
//	m_strProcess+=_T("MIN:");
//	//m_strProcess+=_T("14.2V  ");
//	m_ExlRge.SetItem(COleVariant((long)3),COleVariant((long)1),COleVariant(m_strProcess)); 
//	m_ExlRge.SetHorizontalAlignment(COleVariant((long)-4131)); //horizontal alignment:left
//	m_ExlRge.SetVerticalAlignment(COleVariant((long)-4108)); //vertical alignment:middle
//
//	//TCHAR szSaveFilePath[MAX_PATH];
//	//GetCurrentDirectory(MAX_PATH,szSaveFilePath);
//	//CString szSaveFilePathName=szSaveFilePath;
//	//TCHAR szSaveName[MAX_PATH];
//	//pDataSheetParser->getProductProgramName(szSaveName);
//	//szSaveFilePathName+=_T("\\");
//	//szSaveFilePathName+=szSaveName;
//	//szSaveFilePathName+=_T(".xls");
//	
//	OPENFILENAME saveOpfn;
//	if(!SaveFileName(&saveOpfn,_T("xlsx")))
//	{
//		//MessageBox(_T("Please input file name, file not saved"),_T("Warnning"),MB_OK);
//		return;// not get right file
//	}
//	TCHAR szSaveFilePathName[MAX_PATH];
//	_tcscpy_s(szSaveFilePathName,MAX_PATH,saveOpfn.lpstrFile);
//	m_ExlSheet.SaveAs(szSaveFilePathName,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing);
//	//release resource (most importance!!!)
//	m_ExlRge.ReleaseDispatch(); 
//	m_ExlSheet.ReleaseDispatch(); 
//	m_ExlSheets.ReleaseDispatch(); 
//	m_ExlBook.ReleaseDispatch(); 
//	m_ExlBooks.ReleaseDispatch(); 
//	//m_ExlApp must release, otherwise there will be a excel process stays in ram, and when repeatly run the program, it will occur fatal error. 
//	m_ExlApp.ReleaseDispatch(); 
//
//	//quit program
//	m_ExlApp.Quit(); 
//}
//
//
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
	//instEagleSrcCodeParser.~CEagleSrcCodeParser();
	//instAccoSrcCodeParser.~CAccoSrcCodeParser();
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
					TCHAR lpFileName[MAX_PATH]=_T("");
					if(pDataSheetParser->getProductProgramName(lpFileName))
						_tcscpy_s(lpFileName,MAX_PATH,_T("default_testplan_export.xlsx"));
					else
						_stprintf_s(lpFileName,MAX_PATH,_T("%s_testplan_export.xlsx"),lpFileName);
					if(SaveFileName(&saveOpfn,lpFileName,_T("xlsx")))
					{
						TCHAR szSaveFilePathName[MAX_PATH];
						_tcscpy_s(szSaveFilePathName,MAX_PATH,saveOpfn.lpstrFile);
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
			mstr_Log=_T("");
			fclose(pLog);
		}
	}
#ifdef _HDB_
	//_tprintf(_T("%s\n"),m_strInBuf.GetBuffer());
#endif
}
void SaveLogs(LPTSTR lpBuf,int forceSave)
{
	CString mstr_buf=lpBuf;
	SaveLogs(mstr_buf,forceSave);
}
//enable verbose mode
void CexcelDlg::OnBnClickedVerbose()
{
	// TODO: Add your control notification handler code here
	if(this->IsDlgButtonChecked(IDC_Verbose))
	{
		this->blEnableVerboseMode=true;
	}
	else
	{
		this->blEnableVerboseMode=false;
	}
}
//get current exe directory
DWORD GetCurrentEXEDirectory(LPTSTR lpszOutBuf,DWORD dwSize_of_t)
{
	if(!lpszOutBuf)
		return -1;
	TCHAR szCurrentDirectory[MAX_PATH];
	DWORD dwResult=GetModuleFileName(NULL,szCurrentDirectory,MAX_PATH);
	if(!dwResult)
		return 0;
	trimrol(szCurrentDirectory,_T("\\"));
	_tcscpy_s(lpszOutBuf,_tcslen(szCurrentDirectory)+1,szCurrentDirectory);
	return _tcslen(lpszOutBuf);
}


void CexcelDlg::OnFileLoadds()
{
	this->OnBnClickedBtnLoadds();
	// TODO: Add your command handler code here
}

void CexcelDlg::OnFileLoadimage32782()
{
	// TODO: Add your control notification handler code here
	ClrEditboxInfo(this,IDC_EDIT_INFO);
	AddEditboxInfo(this,IDC_EDIT_INFO,_T(">>>>loading image file"),1);
	//init opfn
	TCHAR szSelPath[MAX_PATH]=_T("");
	if(GetFileName(_T("Image file(*.img)\0*.img\0All files(*.*)\0*.*\0"),szSelPath,MAX_PATH))
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("file not selected"));
		return;// not get right file
	}
	TCHAR path[MAX_PATH];
	_tcscpy_s(path,MAX_PATH,szSelPath);
	FILE* pFimage;
	if(_taccess_s(path,4))//0 if exist
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("Invalid file path"));
		return;
	}
	else
	{
		if(_tfopen_s(&pFimage,path,_T("r")))
		{
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
			return;
		}
		else
		{
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("image file:"));
			AddEditboxInfo(this,IDC_EDIT_INFO,path);
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));
		}
	}
	if(NULL==pParamMap)
	{
		HANDLE TestPlan_MapFile=OpenFileMapping(FILE_MAP_WRITE,FALSE,_T("Testplan_MapFile"));
		void* pTestPlanBaseMapFile=0;
		//create new filemap
		if(TestPlan_MapFile==NULL)
		{
			TestPlan_MapFile=CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,(RMT_MAX_TESTPLAN_PARAM)*sizeof(ParamStruct),_T("Testplan_MapFile"));  
			pTestPlanBaseMapFile=MapViewOfFile(TestPlan_MapFile,FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);
		}
		else
		{
			pTestPlanBaseMapFile=MapViewOfFile(TestPlan_MapFile,FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);
		}
		pParamMap=(ParamStruct*)pTestPlanBaseMapFile;
	}
	if(NULL!=pParamMap)
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("applying image file"));
		BYTE szBuff[MAX_PATH];int dwCnt=0;
		//dwCnt=fread_s(szBuff,MAX_PATH,1,MAX_PATH,pFimage);
		dwCnt=fread(szBuff,1,MAX_PATH,pFimage);
		int dwIndex=0;
		int dwBytesWriten=0;
		while(dwCnt>0)
		{
			dwBytesWriten+=dwCnt;
			memcpy_s((BYTE*)pParamMap+dwIndex*MAX_PATH,MAX_PATH,szBuff,MAX_PATH);
			dwIndex++;
			//dwCnt=fread_s(szBuff,MAX_PATH,1,MAX_PATH,pFimage);
			dwCnt=fread(szBuff,1,MAX_PATH,pFimage);
		}
		TCHAR szOutPutBuf[MAX_PATH];
		_stprintf_s(szOutPutBuf,_T("image file size:%d Bytes"),dwBytesWriten);
		AddEditboxInfo(this,IDC_EDIT_INFO,szOutPutBuf);
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));
	}
	else
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("applying image file failed"));
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
	}
}

void CexcelDlg::OnFileExit32783()
{
	// TODO: Add your command handler code here
	this->OnBnClickedBtnQuit();
}

void CexcelDlg::OnHelpQuickStart()
{
	// TODO: Add your command handler code here
	TCHAR szIndexPath[MAX_PATH];
	ZeroMemory(szIndexPath,MAX_PATH*sizeof(TCHAR));
	GetCurrentEXEDirectory(szIndexPath,MAX_PATH);
	_tcscat_s(szIndexPath,_T("\\doc\\QuickStart\\QuickStart.htm"));
	ShellExecute(NULL,NULL,szIndexPath,NULL,NULL,SW_SHOWNORMAL);
}

void CexcelDlg::OnAboutVersion32784()
{
	// TODO: Add your command handler code here

}

//get file version
#ifdef _UNICODE
#define GetFileVersion GetFileVersionW
#else
#define GetFileVersion GetFileVersionA
#endif
//int GetFileVersionW(const WCHAR* lpszFilePath,const WCHAR* lpszKeyWords);
//int GetFileVersionA(const CHAR* lpszFilePath,const CHAR* lpszKeyWords);

int GetFileVersionW(const WCHAR* lpszFilePath,const WCHAR* lpszKeyWords)
{
	FILE* pFile=0;
	WCHAR szLineBuffer[MAX_FILEOP_BUFF]=L"";
	WCHAR szTargetLine[MAX_FILEOP_BUFF]=L"";
	WCHAR szVersion[MAX_FILEOP_BUFF]=L"";
	WCHAR* pszLine=0;
	int dwTgtLen=0;
	int dwVersion=0;
	int dwVersionChCnt=0;
	//check file access
	if(0==lpszFilePath||0==lpszKeyWords) return -1;//invalid input param
	if(_waccess(lpszFilePath,4)) return -1;//file no read access
	_wfopen_s(&pFile,lpszFilePath,L"r");
	if(!pFile) return -1;//failed to open file with "r" access
	pszLine=fgetws(szLineBuffer,MAX_FILEOP_BUFF,pFile);
	while(pszLine)
	{
		if(wcsstr(szLineBuffer,lpszKeyWords))
			wcscpy_s(szTargetLine,MAX_FILEOP_BUFF,szLineBuffer);
		pszLine=fgetws(szLineBuffer,MAX_FILEOP_BUFF,pFile);
	}
	fclose(pFile);
	if(0==szTargetLine[0])
		return -1;//no version infomation
	dwTgtLen=wcslen(szTargetLine);
	for(int x=0;x<dwTgtLen;x++)
	{
		if(szTargetLine[x]>=L'0'&&szTargetLine[x]<=L'9')
			szVersion[dwVersionChCnt++]=szTargetLine[x];
	}
	szVersion[dwVersionChCnt]=0;
	if(0==dwVersionChCnt) return -1;
	dwVersion=_wtoi(szVersion);
	return dwVersion;
}
int GetFileVersionA(const CHAR* lpszFilePath,const CHAR* lpszKeyWords)
{
	if(0==lpszFilePath||0==lpszKeyWords)
		return -1;
	WCHAR buff[MAX_STRINGEX];
	char srcbuff[MAX_STRINGEX];
	ZeroMemory(srcbuff,sizeof(char)*MAX_STRINGEX);
	strcpy_s(srcbuff,MAX_STRINGEX,lpszFilePath);
	int dwCount=MB2WC(srcbuff,buff);
	if(0==dwCount)
		return -1;
	WCHAR buff2[MAX_STRINGEX];
	char srcbuff2[MAX_STRINGEX];
	ZeroMemory(srcbuff2,sizeof(char)*MAX_STRINGEX);
	strcpy_s(srcbuff2,MAX_STRINGEX,lpszKeyWords);
	dwCount=MB2WC(srcbuff2,buff2);
	if(0==dwCount)
		return -1;
	return GetFileVersionW(buff,buff2);
}


void CexcelDlg::OnFileAddToolTo()
{
	int dwDebugPoint=0;
	// TODO: Add your command handler code here
	{
		int dwOption=AfxMessageBox(_T("This operation will modify your source code, make sure you have backup the project!!!"),MB_YESNO);
		if(7==dwOption)
			return;
	}
	ClrEditboxInfo(this,IDC_EDIT_INFO);
	AddEditboxInfo(this,IDC_EDIT_INFO,_T(">>>>loading source file"),1);
	TCHAR szSelPath[MAX_PATH]=_T("");
	if(GetFileName(_T("pds file(*.pds)\0*.pds\0pgs file(*.pgs)\0*.pgs\0All files(*.*)\0*.*\0"),szSelPath,MAX_PATH))
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("file not selected"));
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
		return;// not get right file
	}
	AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));
	TCHAR szPath[MAX_PATH]=_T("");
	_tcscpy_s(szPath,MAX_PATH,szSelPath);
	AddEditboxInfo(this,IDC_EDIT_INFO,_T(">>>>check project type"));
	//check project type
	TCHAR szProjType[MAX_PATH];
	_tcscpy_s(szProjType,MAX_PATH,szPath);
	trimlol(szProjType,_T("."));
	if(_tcsstr(szProjType,_T("pgs")))
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("project type is ACCO test program"));
	}
	else if(_tcsstr(szProjType,_T("pds")))
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("project type is Eagle test program"));
	}
	else
	{
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("not supported project type"));
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
		return;
	}
	AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));

	//update acco project
	if(_tcsstr(szProjType,_T("pgs")))
	{
		TCHAR szErrorContent[MAX_TMP_STR_LENTH]=_T("");
		AddEditboxInfo(this,IDC_EDIT_INFO,_T(">>>>start to upgrade acco project"));
		if(UpgradeAcco(szPath,szErrorContent,MAX_TMP_STR_LENTH))
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));
		else
		{
			AddEditboxInfo(this,IDC_EDIT_INFO,szErrorContent);
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
		}
	}

#pragma region "update eagle project"
	if(_tcsstr(szProjType,_T("pds")))
	{
		bool bEverUpdated=false;
		bool bToNewVersion=false;
		TCHAR szEagleMainCppPath[MAX_PATH]=_T("");
		TCHAR szEagleProjPath[MAX_PATH]=_T("");
		TCHAR szEagleFileLineBuf[MAX_FILEOP_BUFF]=_T("");
		LPTSTR lpszEFLB=0;
		TCHAR szTargetRmthdlPath[MAX_PATH]=_T("");
		TCHAR szSourceRmthdlPath[MAX_PATH]=_T("");
		FILE* fpCppFile=0;
		AddEditboxInfo(this,IDC_EDIT_INFO,_T(">>>>check project status"));
		try
		{
			//analysis datasheet
			{
				pPdsParser->init();
				pDataSheetParser->init();
				FILE* pDataSheetFile=0;
				_tfopen_s(&pDataSheetFile,szPath,_T("r"));
				if(!pDataSheetFile)
				{
					fclose(pDataSheetFile);
					throw (const TCHAR*)_T("can not open datasheet file");
				}
				fclose(pDataSheetFile);
				if(pPdsParser->analysis(szPath))//0=parse success
				{
					throw (const TCHAR*)_T("can not analysis datasheet");
				}
				else
					pDataSheetParser->loadFromClass(pPdsParser,PROJTYPE_EAGLE);
				fclose(pDataSheetFile);
			}
			//check ever added tools
			{
				zerocopystr(szEagleMainCppPath,MAX_PATH,szPath);
				trimrol(szEagleMainCppPath,_T("."));
				_tcscat(szEagleMainCppPath,_T(".cpp"));
				if(_taccess_s(szEagleMainCppPath,6))//rw permission
				{
					throw (const TCHAR*)_T("can not find source file");
				}
				zerocopystr(szEagleProjPath,MAX_PATH,szPath);
				trimrol(szEagleProjPath,_T("\\"));
				_tfopen_s(&fpCppFile,szEagleMainCppPath,_T("r+"));
				if(!fpCppFile)
					throw (const TCHAR*)_T("read access violation when open source file");
				lpszEFLB=_fgetts(szEagleFileLineBuf,MAX_FILEOP_BUFF,fpCppFile);
				//check MS UTF-8
				if(0xFF==szEagleFileLineBuf[0]&&0xFE==szEagleFileLineBuf[1])
				{
					fclose(fpCppFile);
					fpCppFile=0;
					_tfopen_s(&fpCppFile,szEagleMainCppPath,_T("r+,ccs=utf-8"));
					if(!fpCppFile)
						throw (const TCHAR*)_T("read access violation when open source file");
					lpszEFLB=_fgetts(szEagleFileLineBuf,MAX_FILEOP_BUFF,fpCppFile);
				}
				while(lpszEFLB)
				{
					if(_tcsstr(szEagleFileLineBuf,_T("_USE_TESTPLAN_EXPORT_TOOL")))
					{
						bEverUpdated=true;
					}
					lpszEFLB=_fgetts(szEagleFileLineBuf,MAX_FILEOP_BUFF,fpCppFile);
				}
				fclose(fpCppFile);
				fpCppFile=0;
			}
			//check version
			{
				_stprintf_s(szTargetRmthdlPath,MAX_PATH,_T("%s\\eaglermthdl.h"),szEagleProjPath);
				int dwOldVersion=GetFileVersion(szTargetRmthdlPath,_T("EAGLERMTHDLVERSION"));
				GetCurrentEXEDirectory(szSourceRmthdlPath,MAX_PATH);
				_stprintf_s(szSourceRmthdlPath,MAX_PATH,_T("%s\\tool\\eaglermthdl.h"),szSourceRmthdlPath);
				int dwNewVersion=GetFileVersion(szSourceRmthdlPath,_T("EAGLERMTHDLVERSION"));
				if(dwNewVersion>dwOldVersion)
				{			
					bToNewVersion=true;
				}
			}
		}
		catch(const TCHAR* szExcep)
		{
			AddEditboxInfo(this,IDC_EDIT_INFO,szExcep);
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
			return;
		}
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("check finish"));
		if(bEverUpdated&&!bToNewVersion)
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("no need to update"));
		if(!bEverUpdated)
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("fresh project"));
		if(!bEverUpdated&&bToNewVersion)
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("need update to new version"));
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));
		//check result
		if(!bEverUpdated)
		{
			bToNewVersion=true;
			AddEditboxInfo(this,IDC_EDIT_INFO,_T(">>>>add code to project cpp file"));
			int dwSrcLine=1;
			TCHAR szLineBuff[MAX_FILEOP_BUFF]=_T("");
			int nLine=LoadPathFileToLineBuffer(szEagleMainCppPath);
			if(nLine<0)
			{
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("load source file fail"));
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
				return;
			}
			FuncTrack uEagleFuncOrder;
			if(!TraceFunction(&uEagleFuncOrder,pDataSheetParser,_T("ETS_PRGFLOW_FUNC"),_T("END_ETS_PRGFLOW_FUNC"),nLine))
			{
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("track test program call order fail"));
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
				return;
			}
			TCHAR szIncKeywords[MAX_PATH]=_T("");
			TCHAR szIncFilePath[MAX_PATH]=_T("");
			GetCurrentEXEDirectory(szIncFilePath,MAX_PATH);
			_tcscat_s(szIncFilePath,MAX_PATH,TestIndexIncreaseKeywordsFilePath);
			if(false==lua_loadTestIndexIncreaseKeywords(szIncFilePath,szIncKeywords,MAX_PATH))
			{
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("load TestIndexIncreaseKeywords fail"));
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
				return;
			}
			//find in each function
			for(int j=0;j<uEagleFuncOrder.nFunctionCnt;j++)
			{
				int nCountOfPlus=0;
				uEagleFuncOrder.nFuncLineStart[j]>=uEagleFuncOrder.nFuncLineStop[j]||0==uEagleFuncOrder.nFuncLineStart[j]||0==uEagleFuncOrder.nFuncLineStop[j];
				for(int k=uEagleFuncOrder.nFuncLineStart[j];k<uEagleFuncOrder.nFuncLineStop[j];k++)
				{
					//lpszRelayTypeEagleCbit
					if(_tcsstr(gszTextFileBuff[k],szIncKeywords))
					{
						nCountOfPlus++;
						if(!isccmt(gszTextFileBuff[k]))
						{
							strreplacech(gszTextFileBuff[k],_T('\n'),_T(' '));
							TCHAR szRecordCondition[MAX_PATH]=_T("");
							_stprintf_s(szRecordCondition,MAX_PATH,_T("RecordCondition(\"%d.%d\");\n"),j+1,nCountOfPlus);
							_tcscat_s(gszTextFileBuff[k],MAX_FILEOP_BUFF,szRecordCondition);
							dwDebugPoint++;
						}
					}
				}
				dwDebugPoint++;
			}
			//write back to cpp file
			_tfopen_s(&fpCppFile,szEagleMainCppPath,_T("w+"));
			//_tfopen_s(&fpCppFile,_T(".\\tmp.txt"),_T("w+"));
			if(!fpCppFile)
			{
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("no w+ access before wirte back"));
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
				return;
			}
			for(int i=0;i<nLine;i++)
			{
				_fputts(gszTextFileBuff[i],fpCppFile);
				//insert header
				if(_tcsstr(gszTextFileBuff[i],_T("<windows.h>")))
				{
					_fputts(_T("//Below code are added by autotestplan software\n"),fpCppFile);
					_fputts(_T("//comment \"#define	_USE_TESTPLAN_EXPORT_TOOL\" to disable autotestplan function\n"),fpCppFile);
					_fputts(_T("#define	_USE_TESTPLAN_EXPORT_TOOL\n"),fpCppFile);
					_fputts(_T("#ifdef		_USE_TESTPLAN_EXPORT_TOOL\n"),fpCppFile);
					_fputts(_T("#pragma comment(lib,\"EagleRmthdl.lib\")\n"),fpCppFile);
					_fputts(_T("#include\"eaglermthdl.h\"\n"),fpCppFile);
					_fputts(_T("#else\n"),fpCppFile);
					_fputts(_T("#define EnableRecord() \n"),fpCppFile);
					_fputts(_T("#define RecordCondition() \n"),fpCppFile);
					_fputts(_T("#define SaveImage() \n"),fpCppFile);
					_fputts(_T("#endif\n"),fpCppFile);
					_fputts(_T("//end of code added by autotestplan software\n"),fpCppFile);
				}
				//insert "EnableRecord() function"
				if(_tcsstr(gszTextFileBuff[i],_T("ETS_PRGFLOW_FUNC Init")))
				{
					if(!isccmt(gszTextFileBuff[i]))
					{
						//find {
						i++;
						while(!_tcsstr(gszTextFileBuff[i],_T("{"))&&gszTextFileBuff[i][0]!=0)
						{
							_fputts(gszTextFileBuff[i],fpCppFile);
							i++;
						}
						_fputts(gszTextFileBuff[i],fpCppFile);
						_fputts(_T("	EnableRecord();\n"),fpCppFile);
					}
				}
				//insert "SaveImage() function"
				if(_tcsstr(gszTextFileBuff[i],_T("::OnTestCompletion")))
				{
					if(!isccmt(gszTextFileBuff[i]))
					{
						//find {
						i++;
						while(!_tcsstr(gszTextFileBuff[i],_T("{"))&&gszTextFileBuff[i][0]!=0)
						{
							_fputts(gszTextFileBuff[i],fpCppFile);
							i++;
						}
						_fputts(gszTextFileBuff[i],fpCppFile);
						_fputts(_T("	SaveImage();\n"),fpCppFile);
					}
				}
			}
			fclose(fpCppFile);
			fpCppFile=0;
			pPdsParser->init();
			pDataSheetParser->init();
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));
/*
			//write back to source file
			_tfopen_s(&fpTmpFile,szTmpFileName,_T("r"));
			_tfopen_s(&fpCppFile,szEagleMainCppPath,_T("w+"));
			if(!fpCppFile||!fpTmpFile)
			{
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("an error occured before wirte back"));
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
				if(fpCppFile)
					fclose(fpCppFile);
				if(fpTmpFile)
					fclose(fpTmpFile);
				return;
			}
			lpszEFLB=_fgetts(szEagleFileLineBuf,MAX_FILEOP_BUFF,fpTmpFile);
			//record current function
			TCHAR szCurFunction[MAX_FILEOP_BUFF];
			ZeroMemory(szCurFunction,MAX_FILEOP_BUFF*sizeof(TCHAR));
			while(lpszEFLB)
			{
				if(_tcsstr(szEagleFileLineBuf,_T("ETS_PRGFLOW_FUNC"))&&_tcsstr(szEagleFileLineBuf,_T("(")))
				{
					if(!isccmt(szEagleFileLineBuf))
					{
						_tcscpy_s(szCurFunction,MAX_FILEOP_BUFF,szEagleFileLineBuf);
						trimlof(szCurFunction,_T("ETS_PRGFLOW_FUNC"));
						trimrof(szCurFunction,_T("("));
						trimblank(szCurFunction);
					}
				}
				if(_tcsstr(szEagleFileLineBuf,_T("<windows.h>")))
				{
					_fputts(_T("//Below code are added by autotestplan software\n"),fpCppFile);
					_fputts(_T("//comment \"#define	_USE_TESTPLAN_EXPORT_TOOL\" to disable autotestplan function\n"),fpCppFile);
					_fputts(_T("#define	_USE_TESTPLAN_EXPORT_TOOL\n"),fpCppFile);
					_fputts(_T("#ifdef		_USE_TESTPLAN_EXPORT_TOOL\n"),fpCppFile);
					_fputts(_T("#pragma comment(lib,\"EagleRmthdl.lib\")\n"),fpCppFile);
					_fputts(_T("#include\"eaglermthdl.h\"\n"),fpCppFile);
					_fputts(_T("#else\n"),fpCppFile);
					_fputts(_T("#define EnableRecord() \n"),fpCppFile);
					_fputts(_T("#define RecordCondition() \n"),fpCppFile);
					_fputts(_T("#define SaveImage() \n"),fpCppFile);
					_fputts(_T("#endif\n"),fpCppFile);
					_fputts(_T("//end of code added by autotestplan software\n"),fpCppFile);
				}
				_fputts(szEagleFileLineBuf,fpCppFile);
				//insert "EnableRecord() function"
				if(_tcsstr(szEagleFileLineBuf,_T("ETS_PRGFLOW_FUNC Init")))
				{
					if(!isccmt(szEagleFileLineBuf))
					{
						//find {
						while(!_tcsstr(szEagleFileLineBuf,_T("{"))&&lpszEFLB)
							lpszEFLB=_fgetts(szEagleFileLineBuf,MAX_FILEOP_BUFF,fpTmpFile);
						_fputts(szEagleFileLineBuf,fpCppFile);
						_fputts(_T("	EnableRecord();\n"),fpCppFile);
					}
				}
				//insert "SaveImage() function"
				if(_tcsstr(szEagleFileLineBuf,_T("::OnTestCompletion")))
				{
					if(!isccmt(szEagleFileLineBuf))
					{
						//find {
						while(!_tcsstr(szEagleFileLineBuf,_T("{"))&&lpszEFLB)
							lpszEFLB=_fgetts(szEagleFileLineBuf,MAX_FILEOP_BUFF,fpTmpFile);
						_fputts(szEagleFileLineBuf,fpCppFile);
						_fputts(_T("	SaveImage();\n"),fpCppFile);
					}
				}
				//insert "RecordCondition() function"
				if(_tcsstr(szEagleFileLineBuf,_T("groupgetresults")))
				{
					if(!isccmt(szEagleFileLineBuf))
					{
						TCHAR szWorking[MAX_FILEOP_BUFF];
						zerocopystr(szWorking,MAX_FILEOP_BUFF,szEagleFileLineBuf);
						trimlof(szWorking,_T("("));
						trimrol(szWorking,_T(","));
						trimblank(szWorking);
						int dwIndex=0;
						for(;dwIndex<pPdsParser->iDVcnt;dwIndex++)
						{
							TCHAR szTmp[MAX_FILEOP_BUFF]=_T("");
							zerocopystr(szTmp,MAX_FILEOP_BUFF,pPdsParser->szDatasheetVariable[dwIndex][0]);
							if(_tcsstr(szTmp,_T("=")))
							{
								trimlof(szTmp,_T("="));		
								trimrof(szTmp,_T("\n"));		
								trimblank(szTmp);
								if(!_tcscmp(szTmp,szCurFunction))
									break;
							}
						}
						for(int i=dwIndex+1;i<pPdsParser->iDVcnt;i++)
						{
							if(_tcsstr(pPdsParser->szDatasheetVariable[i][0],_T("Function=")))
								break;
							if(!_tcscmp(pPdsParser->szDatasheetVariable[i][2],szWorking))
							{
								TCHAR szCodeToAdd[MAX_PATH];
								int dwFuncNbr=0;
								int dwParamNbr=0;
								dwFuncNbr=_ttoi(pPdsParser->szDatasheetVariable[i][0]);
								dwParamNbr=_ttoi(pPdsParser->szDatasheetVariable[i][1]);							
								_stprintf_s(szCodeToAdd,_T("	RecordCondition(\"%d.%d\");\n"),dwFuncNbr,dwParamNbr);
								_fputts(szCodeToAdd,fpCppFile);
							}
						}
					}
				}
				lpszEFLB=_fgetts(szEagleFileLineBuf,MAX_FILEOP_BUFF,fpTmpFile);
			}
			fclose(fpTmpFile);
			fpTmpFile=0;
			fclose(fpCppFile);
			fpCppFile=0;
			pPdsParser->init();
			pDataSheetParser->init();
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));
*/		
		}
		if(bToNewVersion)
		{
			//copy files to source file path
			AddEditboxInfo(this,IDC_EDIT_INFO,_T(">>>>update to new version"));
			TCHAR szSrcFilePath[MAX_PATH];
			GetCurrentEXEDirectory(szSrcFilePath,MAX_PATH);
			TCHAR szSrcFileDLL[MAX_PATH];
			TCHAR szSrcFileLIB[MAX_PATH];
			TCHAR szSrcFileHEAD[MAX_PATH];
			_tcscpy_s(szSrcFileDLL,MAX_PATH,szSrcFilePath);
			_tcscpy_s(szSrcFileLIB,MAX_PATH,szSrcFilePath);
			_tcscpy_s(szSrcFileHEAD,MAX_PATH,szSrcFilePath);
			_tcscat(szSrcFileDLL,_T("\\tool\\eaglermthdl.dll"));
			_tcscat(szSrcFileLIB,_T("\\tool\\eaglermthdl.lib"));
			_tcscat(szSrcFileHEAD,_T("\\tool\\eaglermthdl.h"));
			if(_taccess_s(szSrcFileDLL,0)||_taccess_s(szSrcFileLIB,0)||_taccess_s(szSrcFileHEAD,0))//existance
			{
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("file missing"));
				AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<fail"));
				return;
			}
			//execute copy cmd
			TCHAR szCmd[MAX_PATH*3];
			_stprintf_s(szCmd,MAX_PATH*3,_T("copy \"%s\"  \"%s\""),szSrcFileDLL,szEagleProjPath);
			_tsystem(szCmd);
			_stprintf_s(szCmd,MAX_PATH*3,_T("copy \"%s\"  \"%s\""),szSrcFileLIB,szEagleProjPath);
			_tsystem(szCmd);
			_stprintf_s(szCmd,MAX_PATH*3,_T("copy \"%s\"  \"%s\""),szSrcFileHEAD,szEagleProjPath);
			_tsystem(szCmd);
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("3 files copied"));
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("<<<<success"));
		}
	}
#pragma endregion "update eagle project"

}


//runcmd function
BOOL RunCMD(const TCHAR *lpcszCMD,const TCHAR* lpcszParam,DWORD dwMilliSeconds,int nShow)
{
	SHELLEXECUTEINFO ShExeInfo={0};
	ShExeInfo.cbSize=sizeof(SHELLEXECUTEINFO);
	ShExeInfo.fMask=SEE_MASK_NOCLOSEPROCESS;
	ShExeInfo.hwnd=NULL;
	ShExeInfo.lpVerb=NULL;
	ShExeInfo.lpFile=lpcszCMD;
	ShExeInfo.lpParameters=lpcszParam;
	ShExeInfo.lpDirectory=NULL;
	ShExeInfo.nShow=nShow;
	ShExeInfo.hInstApp=NULL;
	BOOL suc=ShellExecuteEx(&ShExeInfo);
	WaitForSingleObject(ShExeInfo.hProcess,dwMilliSeconds);
	return suc;
}

void CexcelDlg::OnFileSaveas()
{
	ClrEditboxInfo(this,IDC_EDIT_INFO);
	AddEditboxInfo(this,IDC_EDIT_INFO,_T("save as."),1);
	SaveLogs(_T("save as force save"),1);
	if(!pOE)
		return;
	try
	{
		OPENFILENAME saveOpfn;
		TCHAR lpFileName[MAX_PATH]=_T("");
		if(pDataSheetParser->getProductProgramName(lpFileName))
			_tcscpy_s(lpFileName,MAX_PATH,_T("default_testplan_export.xlsx"));
		else
			_stprintf_s(lpFileName,MAX_PATH,_T("%s_testplan_export.xlsx"),lpFileName);
		if(!SaveFileName(&saveOpfn,lpFileName,_T("xlsx")))
		{
			AddEditboxInfo(this,IDC_EDIT_INFO,_T("file not saved"));
			return;// not get right file
		}
		TCHAR szSaveFilePathName[MAX_PATH];
		_tcscpy_s(szSaveFilePathName,MAX_PATH,saveOpfn.lpstrFile);
		pOE->sheet.SaveAs(szSaveFilePathName,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing);
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("save file success"));
		pOE->book.SetSaved(TRUE);
		pOE->app.SetVisible(FALSE);
	}
	catch(...)   
	{   
		AddEditboxInfo(this,IDC_EDIT_INFO,_T("file not saved"));
		SaveLogs(_T("exception type: all, force save"),1);
		pOE->book.SetSaved(FALSE);
		pOE->app.SetVisible(FALSE);
	}   
}
