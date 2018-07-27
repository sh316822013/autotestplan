#pragma once

//_HDB_
#define _HDB_
#ifdef _HDB_
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#endif
//_HDB_	END

extern "C" 
{  
    #include <lua.h>  
    #include <lauxlib.h>  
    #include <lualib.h>  
} 
bool LoadProjectParam(/*char* filename*/);
bool NewConsole();
void testlua();
#ifdef _UNICODE
#define lua_loadTestIndexIncreaseKeywords	lua_loadTestIndexIncreaseKeywordsW
#else
#define lua_loadTestIndexIncreaseKeywords	lua_loadTestIndexIncreaseKeywordsA
#endif

bool lua_loadTestIndexIncreaseKeywordsA(const char* filename,char* lpszOutBuf,int nCntOfCh);
