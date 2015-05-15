@echo off

echo Starting shell...

:: Default behavior -- comment out if you want to use Cygwin...
cmd /Q

:: Cygwin configuration:
::   Uncomment the following and replace the paths below with the correct
::   paths to your cygwin installation to use Cygwin in your Virtual Terminals.
::   Note that you'll need 'socat' in your Cygwin install in order for this
::   to work (it's used to get a pty without having to link against cygwin)

REM echo {shell_winrawvt}
REM set JOEWIN=1
REM SET CYGBIN=J:\cygwin\bin
REM for /f %%p in ('%CYGBIN%\cygpath %~dp0') do set MYPATH=%%p
REM %CYGBIN%\socat EXEC:'bash --init-file %MYPATH%shell.sh -i',pty,ctty,stderr,setsid,path=/bin STDIO
