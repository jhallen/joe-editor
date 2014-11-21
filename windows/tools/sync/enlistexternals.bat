setlocal
mkdir %~dp0\repos
cd %~dp0\repos

cvs -z3 -d:pserver:anonymous@joe-editor.cvs.sourceforge.net:/cvsroot/joe-editor/ co -P joe-current -d joe
svn co svn://svn.tartarus.org/sgt/putty putty

%~dp0\syncexternals.bat
