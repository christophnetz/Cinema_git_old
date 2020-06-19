rem triggered by post build event

@echo off
set tmpd=cinema-%1%
if exist %tmpd% rd /S /Q %tmpd% 
mkdir %tmpd% 
xcopy build\media %tmpd%\media /i /s
copy build\*.ini %tmpd%
copy build\*.bat %tmpd%
copy build\*.exe %tmpd%
