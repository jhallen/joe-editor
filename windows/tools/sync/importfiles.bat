@echo off 
setlocal

set mapdir=%~dp0\map
set repodir=%~dp0\repos
set rootdir=%~dp0
set treedir=%~dp0\..\..\src

set xcmd=xcopy /Y /I

FOR /F "tokens=*" %%f IN (%mapdir%\putty.files) DO %xcmd% %repodir%\putty\%%f %treedir%\putty\putty\%%f
