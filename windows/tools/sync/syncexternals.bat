setlocal

set mapdir=%~dp0\map
set repodir=%~dp0\repos
set rootdir=%~dp0

cd %repodir%\putty
FOR /F "delims=!" %%r IN (%mapdir%\putty.version) DO svn up -r %%r

cd %repodir%\joe
FOR /F "delims=!" %%d IN (%mapdir%\joe.version) DO cvs up -D "%%d"
