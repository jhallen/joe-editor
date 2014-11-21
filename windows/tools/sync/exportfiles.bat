@echo off 
setlocal

set mapdir=%~dp0\map
set repodir=%~dp0\repos
set rootdir=%~dp0
set treedir=%~dp0\..\..\src

set xcmd=xcopy /Y /I

FOR /F "tokens=*" %%f IN (%mapdir%\joe.files) DO attrib -r %repodir%\joe\%%f && %xcmd% %treedir%\joe\joe\%%f %repodir%\joe\%%f
FOR /F "tokens=*" %%f IN (%mapdir%\putty.files) DO %xcmd% %treedir%\putty\putty\%%f %repodir%\putty\%%f
