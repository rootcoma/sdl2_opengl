@ECHO off
set PROG_NAME=sdl2.exe

set SRC_FILES=^
src\main.cpp ^
src\gui\window.cpp ^
src\gui\event.cpp ^
src\scene.cpp ^
src\graphics\shader_program.cpp ^
src\graphics\stl_parser.cpp ^
src\graphics\camera.cpp

set CXX_FLAGS=-m32 -O2 -Wall -Werror -std=c++14

set INC=^
-Isrc ^
-Ilib\glew-2.1.0\include ^
-Ilib\glm-0.9.9.5 ^
-Ilib\SDL2-2.0.9\include

set LIB=^
-Llib\SDL2-2.0.9\lib\x86 ^
-Llib\glew-2.1.0\lib\Release\Win32 ^
-L"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\lib" ^
-L"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10586.0\ucrt\x86" ^
-L"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10586.0\um\x86"

set LINK=^
-lSDL2 ^
-lglew32 ^
-lopengl32

@ECHO on
mkdir build
copy lib\SDL2-2.0.9\lib\x86\SDL2.dll build
copy lib\glew-2.1.0\bin\Release\Win32\glew32.dll build
mkdir build\models
copy res\*.stl build\models
mkdir build\shaders
copy /Y src\shaders\* build\shaders
clang++ -Xclang -flto-visibility-public-std -o build/%PROG_NAME% %CXX_FLAGS% %INC% %LIB% %LINK% %SRC_FILES% -Xlinker /subsystem:console
@ECHO off

echo "Done!~"

pause
