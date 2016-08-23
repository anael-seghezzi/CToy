mkdir Build
cd Build
cmake -G "MinGW Makefiles" ../ -DCMAKE_INSTALL_PREFIX=../bin -DCMAKE_BUILD_TYPE=Release
mingw32-make
mingw32-make install
pause
