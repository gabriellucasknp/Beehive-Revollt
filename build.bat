@echo off
setlocal

set PATH=C:\msys64\mingw64\bin;%PATH%
set GCC=C:\msys64\mingw64\bin\gcc.exe
set INC=-IC:\msys64\mingw64\include
set LIB=-LC:\msys64\mingw64\lib -lraylib -lopengl32 -lgdi32 -lwinmm -lm
set FLAGS=-Wall -Wextra -std=c99 -O2
set SRC=src\main.c src\assets.c src\player.c src\bullet.c src\enemy.c src\stage.c src\collision.c src\quiz.c src\ui.c src\game_state.c
set OUT=build\beehive_revolt.exe

if not exist build mkdir build

%GCC% %FLAGS% %INC% %SRC% -o %OUT% %LIB%

if %errorlevel% == 0 (
    echo.
    echo  Build OK: %OUT%
) else (
    echo.
    echo  Build FAILED.
)
