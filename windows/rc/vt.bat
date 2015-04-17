@echo off

:: Default behavior -- comment out if you want to use Cygwin...
cmd /Q

:: Cygwin configuration:
::   Uncomment the following and replace the paths below with the correct
::   paths to your cygwin installation to use Cygwin in your Virtual Terminals.
::   Note that you'll need 'socat' in your Cygwin install in order for this
::   to work (it's used to get a pty without having to link against cygwin)

REM cd /d J:\cygwin\bin
REM socat "EXEC:bash -il,pty,ctty,login,stderr,setsid" STDIO
