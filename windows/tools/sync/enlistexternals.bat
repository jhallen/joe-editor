setlocal
mkdir %~dp0\repos
cd %~dp0\repos

svn co svn://svn.tartarus.org/sgt/putty putty

%~dp0\syncexternals.bat
