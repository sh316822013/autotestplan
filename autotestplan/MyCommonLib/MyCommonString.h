#pragma once

#include <string>
#ifdef _UNICODE
using namespace std;
#define tstring wstring
#else
using namespace std;
#define tstring string
#endif
#define MAX_STRINGBUFF_LEN 1024
namespace myString
{
//check if string is comments
bool IsComment(tstring str);
//trim space and tab
void TrimSpaceAndTab(tstring& str);
//trim <=left of first finded symbol
void TrimLofFF(tstring& str,tstring symbol);
//trim >=right of first finded symbol
void TrimRofFF(tstring& str,tstring symbol);
//trim <=left of last finded symbol
void TrimLofLF(tstring& str,tstring symbol);
//trim <=right of last finded symbol
void TrimRofLF(tstring& str,tstring symbol);
//transform upper case to lower case
void StrToLowerCase(tstring& str);
}
