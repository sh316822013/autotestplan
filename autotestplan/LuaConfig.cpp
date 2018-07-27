#include "stdafx.h"
#include "luaconfig.h"
void error(lua_State* L, const TCHAR* fmt, ...)
{
	TCHAR szBuff[MAX_PATH]=_T("");
    va_list argp;
    va_start(argp, fmt);
    vsprintf(szBuff, fmt, argp);
    va_end(argp);
	_tprintf(szBuff);
    lua_close(L);
    //exit(EXIT_FAILURE);
}

bool load(char* filename, int* width, int* height)
{
    lua_State* L = lua_open();
    luaL_openlibs(L);
    if(luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0))
    {
        error(L, "cannot run configuration file: %s", lua_tostring(L, -1));
		return false;
    }
    lua_getglobal(L, "width");
    lua_getglobal(L, "height");
    if(!lua_isnumber(L, -2))
        error(L, "'width' should be a number\n");
    if(!lua_isnumber(L, -1))
        error(L, "'height' should be a number\n");
    *width = (int)lua_tonumber(L, -2);
    *height = (int)lua_tonumber(L, -1);
    lua_close(L);
	return true;
}
bool lua_loadTestIndexIncreaseKeywordsA(const char* filename,char* lpszOutBuf,int nCntOfCh)
{
	if(!filename||!lpszOutBuf||nCntOfCh<=0)
		return false;
    lua_State* L = lua_open();
    luaL_openlibs(L);
    if(luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0))
    {
        error(L, "cannot run IndexIncreaseKeywords configuration file: %s", lua_tostring(L, -1));
		return false;
    }
    lua_getglobal(L, "keywrods");
    if(!lua_isstring(L, -1))
        error(L, "keywrods should be a string\n");
    strcpy_s(lpszOutBuf,nCntOfCh,lua_tostring(L, -1));
	printf("\nload TestIndexIncreaseKeywords success:%s\n",lpszOutBuf);
    lua_close(L);
	return true;
}
bool LoadProjectParam(/*char* filename*/)
{
	char* filename="..\\config\\cfg.lua";
    lua_State* L = lua_open();
    luaL_openlibs(L);
    if(luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0))
    {
		printf("cannot run configuration file: %s\n",lua_tostring(L, -1));
		return false;
    }
    lua_getglobal(L, "RelayType");
    lua_getglobal(L, "RelayAssign");
	if(!lua_istable(L, -1))
	{
		printf("stack top is not a table");
	}
	char szKey[MAX_PATH]="";
	char szValue[MAX_PATH]="";
	//int nIndex=lua_gettop(L);
	lua_pushnil(L);
	while(lua_next(L,1))
	{
		strcpy_s(szKey,MAX_PATH,lua_tostring(L, -2));
		strcpy_s(szValue,MAX_PATH,lua_tostring(L, -1));
		lua_pop(L,1);
	}

	//lua_pushstring(L,"i2crelay64");

    lua_close(L);
	return true;
}

void testlua()
{
    int width, height;
    if(load(".\\config\\cfg.lua", &width, &height))
	{
		printf("load config file success\n");
		//printf("width = %d height = %d \n", width, height);
	}
    return;
}

bool NewConsole()
{
//_HDB_
#ifdef _HDB_
	bool isConGen=AllocConsole();
	int hConRedir=_open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE),_O_TEXT);
	FILE* fp=_fdopen(hConRedir,"w");
	*stdout=*fp;
	setvbuf(stdout,NULL,_IONBF,0);
	return isConGen;
#else
	return false;
#endif
//_HDB_	END
}