@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cd /d D:\stickynotes-qt\build-msvc\Debug
PATH=D:\QT\6.11.1\msvc2022_64\bin;%PATH%
start note.exe
