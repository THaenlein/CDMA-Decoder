@echo off

cd compile
call "make_titelseite.bat"
call "make_bericht.bat"
REM call "clean.bat"
cd ..

pause