@echo off
cd src
echo compiling...
g++ main.cpp game.cpp gamestate.cpp ai.cpp gameui.cpp card.cpp -o ..\TensorConcord.exe -I C:\msys64\mingw64\include -L C:\msys64\mingw64\lib -lpdcurses
echo Build Successful!
cd ..