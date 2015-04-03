@echo off 

kdiff3 %~dp0\..\..\..\rc\joerc.in %~dp0\..\..\rc\joerc -o %~dp0\..\..\rc\joerc
kdiff3 %~dp0\..\..\..\rc\jmacsrc.in %~dp0\..\..\rc\jmacsrc -o %~dp0\..\..\rc\jmacsrc
kdiff3 %~dp0\..\..\..\rc\jpicorc.in %~dp0\..\..\rc\jpicorc -o %~dp0\..\..\rc\jpicorc
kdiff3 %~dp0\..\..\..\rc\jstarrc.in %~dp0\..\..\rc\jstarrc -o %~dp0\..\..\rc\jstarrc
kdiff3 %~dp0\..\..\..\rc\ftyperc %~dp0\..\..\rc\ftyperc -o %~dp0\..\..\rc\ftyperc
