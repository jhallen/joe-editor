@echo off
setlocal

cd %~dp0

set vimdir=.\vimschemes
set outdir=.\palettes

mkdir %outdir%

FOR /F "tokens=*" %%f IN (schemelist.txt) DO python palette.py %vimdir%\%%f.vim %outdir%\%%f.html
