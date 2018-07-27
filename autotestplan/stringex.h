// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the STRINGEX_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// STRINGEX_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
//#ifdef STRINGEX_EXPORTS
//#define STRINGEX_API extern "C" __declspec(dllexport)
//#else
//#define STRINGEX_API extern "C" __declspec(dllimport)
//#endif
#pragma once
#ifdef STRINGEX_EXPORTS
#define STRINGEX_API __declspec(dllexport)
#else
#define STRINGEX_API __declspec(dllimport) //for all user
#endif

//below  are only for datasheetparser use
#ifdef DATASHEETPARSER_EXPORTS
#undef STRINGEX_API 
#define STRINGEX_API 
#endif

#define MAX_STRINGEX  1024



STRINGEX_API int MB2WC(CHAR* pszSrcMB,WCHAR* pszDstWC);
STRINGEX_API int WC2MB(WCHAR* pszSrcWC,CHAR* pszDstMB);


//check if string is comments
STRINGEX_API bool isccmtA(const char* lpszInput);
//trim space and tab
STRINGEX_API int trimblankA(char* lpszInputOutput);
//trim <=left of first finded symbol
STRINGEX_API int trimlofA(char* lpszInputOutput,const char* lpszSymbol);
//trim >=right of first finded symbol
STRINGEX_API int trimrofA(char* lpszInputOutput,const char* lpszSymbol);
//trim <=left of last finded symbol
STRINGEX_API int trimlolA(char* lpszInputOutput,const char* lpszSymbol);
//trim <=right of last finded symbol
STRINGEX_API int trimrolA(char* lpszInputOutput,const char* lpszSymbol);
//trim match str
STRINGEX_API int trimstrA(char* lpszInputOutput,const char* lpszSymbol);
//replace character in string
STRINGEX_API int strreplacechA(char* lpszInputOutput,const char chSrc,const char chDst);
//transform upper case to lower case
STRINGEX_API int s2lowerA(char* lpszInputOutput);
//zero and copy string
STRINGEX_API int zerocopystrA(char* lpszDst,int size_of_t,const char* lpszSrc);
//compute distance of two string
STRINGEX_API int strdistanceA(const char* lpszSrc,const char* lpszTgt);

//check if string is comments
STRINGEX_API bool isccmtW(const wchar_t* lpszInput);
//trim space and tab
STRINGEX_API int trimblankW(wchar_t* lpszInputOutput);
//trim <=left of first finded symbol
STRINGEX_API int trimlofW(wchar_t* lpszInputOutput,const wchar_t* lpszSymbol);
//trim >=right of first finded symbol
STRINGEX_API int trimrofW(wchar_t* lpszInputOutput,const wchar_t* lpszSymbol);
//trim <=left of last finded symbol
STRINGEX_API int trimlolW(wchar_t* lpszInputOutput,const wchar_t* lpszSymbol);
//trim <=right of last finded symbol
STRINGEX_API int trimrolW(wchar_t* lpszInputOutput,const wchar_t* lpszSymbol);
//trim match str
STRINGEX_API int trimstrW(wchar_t* lpszInputOutput,const wchar_t* lpszSymbol);
//replace character in string
STRINGEX_API int strreplacechW(wchar_t* lpszInputOutput,const wchar_t chSrc,const wchar_t chDst);
//transform upper case to lower case
STRINGEX_API int s2lowerW(wchar_t* lpszInputOutput);
//zero and copy string
STRINGEX_API int zerocopystrW(wchar_t* lpszDst,int size_of_t,const wchar_t* lpszSrc);
//compute distance of two string
STRINGEX_API int strdistanceW(const WCHAR* lpszSrc,const WCHAR* lpszTgt);


#ifdef _UNICODE
	#define			tstring										wstring

	#define			isccmt										isccmtW
	#define			trimblank									trimblankW
	#define			trimlof										trimlofW
	#define			trimrof										trimrofW
	#define			trimlol										trimlolW
	#define			trimrol										trimrolW
	#define			trimstr										trimstrW
	#define			s2lower									s2lowerW
	#define			zerocopystr								zerocopystrW
	#define			strdistance								strdistanceW
	#define			strreplacech							strreplacechW

#else
	#define			tstring										string

	#define			isccmt										isccmtA
	#define			trimblank									trimblankA
	#define			trimlof										trimlofA
	#define			trimrof										trimrofA
	#define			trimlol										trimlolA
	#define			trimrol										trimrolA
	#define			trimstr										trimstrA
	#define			s2lower									s2lowerA
	#define			zerocopystr								zerocopystrA
	#define			strdistance								strdistanceA
	#define			strreplacech							strreplacechA
#endif