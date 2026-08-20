call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cd /d D:\stickynotes-qt
cmake -B build-msvc -G "Visual Studio 17 2022" -A x64
cmake --build build-msvc --config Release
cmake --build build-msvc --config Debug
