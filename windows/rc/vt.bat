@echo off
setlocal

:: This script is run when a virtual terminal is started in JOE (F1-F4).  You can make modifications
:: here to change what to run when it is started.  This file should be located in the "vt" subdirectory
:: of either: 1) The path in which joe.exe resides or 2) %LOCALAPPDATA%\JoeEditor

:: If you want to use Cygwin, set CYGBIN to your installation's /bin directory and uncomment one
:: line below corresponding to which shell you would like to use.
:: Note: To use this, your Cygwin installation must include the "socat" package.
set CYGBIN=J:\cygwin\bin
REM goto :cygwin_bash
REM goto :cygwin_tcsh

:: If you want to use Powershell, uncomment the following line:
REM goto :powershell

:: -------------------------- You shouldn't need to edit below this line --------------------------

:: ----- Default behavior: Load standard command processor

call :findptyhelper
if exist %PTYHELPER% (
	echo {shell_winrawvt}
	%PTYHELPER% console %ComSpec%
	goto :eof
)

cmd /Q
goto :eof

:: ----- Powershell

:powershell
call :findptyhelper
if exist %PTYHELPER% (
	echo {shell_winrawvt}
	%PTYHELPER% console powershell
	goto :eof
)

echo Couldn't find joewinpty.exe.  Can't start powershell :-(
goto :eof

:: ----- Cygwin

:cygwin_bash

echo {shell_winrawvt}
for /f %%p in ('%CYGBIN%\cygpath %~dp0') do set MYPATH=%%p
%CYGBIN%\socat EXEC:'bash --init-file "%MYPATH%shell.sh" -i',pty,ctty,stderr,setsid,path=/bin STDIO
goto :eof

:cygwin_tcsh

echo {shell_winrawvt}
for /f %%p in ('%CYGBIN%\cygpath %~dp0') do set MYPATH=%%p
REM tcsh will not change default startup script from commandline, so feed command through stdin
echo {shell_txt,"source '%MYPATH%/shell.csh'\r",shell_rtn}
%CYGBIN%\socat EXEC:'tcsh',pty,ctty,stderr,setsid,path=/bin STDIO
goto :eof

:findptyhelper

if exist "%JOEDATA%\vt\joewinpty.exe" (
	set PTYHELPER="%JOEDATA%\vt\joewinpty.exe"
	goto :eof
)

if exist "%JOEHOME%\vt\joewinpty.exe" (
	set PTYHELPER="%JOEHOME%\vt\joewinpty.exe"
	goto :eof
)

echo Couldn't find joewinpty.exe in %JOEDATA% or %JOEHOME%.  Make sure your install is correct!
set PTYHELPER=
goto :eof

:eof
