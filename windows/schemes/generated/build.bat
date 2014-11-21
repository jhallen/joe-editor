@echo off
setlocal

cd %~dp0

set vimdir=.\vimschemes
set outdir=.\output
set ovrdir=.\vimschemes

mkdir %outdir%

FOR /F "tokens=*" %%f IN (schemelist.txt) DO IF EXIST %ovrdir%\%%f.override (python convertvimcolor.py %vimdir%\%%f.vim %outdir%\%%f.joecolor %ovrdir%\%%f.override) ELSE (python convertvimcolor.py %vimdir%\%%f.vim %outdir%\%%f.joecolor)
