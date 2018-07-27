#include "StdAfx.h"
#include "MapManage.h"

namespace __uMAPMANAGE
{
//Test Plan map file handle
HANDLE TestPlan_MapFile=NULL; //this is for testplan map file handle
void* pTestPlanBaseMapFile=NULL; 
//OpenTestplanMap
void* OpenTestplanMap(LPCTSTR lpszMapName)
{
	if(NULL==pTestPlanBaseMapFile&&NULL==TestPlan_MapFile)
	{
		TestPlan_MapFile=OpenFileMapping(FILE_MAP_WRITE,FALSE,lpszMapName);
		pTestPlanBaseMapFile=MapViewOfFile(TestPlan_MapFile,FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);
	}
	if(NULL==TestPlan_MapFile||NULL==pTestPlanBaseMapFile)
	{
		return 0;
	}
	return pTestPlanBaseMapFile;
}

}
