setlocal

set mapdir=%~dp0\map
set repodir=%~dp0\repos
set rootdir=%~dp0

cd %repodir%\putty
svn cleanup
svn revert -R *

cd %repodir%\joe
FOR /F "delims=!" %%d IN (%mapdir%\joe.version) DO cvs up -C -D "%%d"
del /q .#*
