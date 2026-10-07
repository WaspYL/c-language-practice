@echo off
REM C语言编译脚本 - 使用VS2026的cl.exe
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
cd /d "%~dp1"
cl /Zi /Od /W3 "%~nx1"
