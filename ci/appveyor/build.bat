@echo off

rem https://github.com/Framstag/libosmscout/blob/master/ci/appveyor/build.bat

cd %APPVEYOR_BUILD_FOLDER%

echo Compiler: %COMPILER%
echo Architecture: %MSYS2_ARCH%
echo Platform: %PLATFORM%
echo MSYS2 directory: %MSYS2_DIR%
echo MSYS2 system: %MSYSTEM%
echo Configuration: %CONFIGURATION%
echo Bits: %BIT%
echo CPPUNIT_INCLUDE_DIR: %CPPUNIT_INCLUDE_DIR%
echo CPPUNIT_LIBRARY: %CPPUNIT_LIBRARY%

@echo on
SET "PATH=C:\%MSYS2_DIR%\%MSYSTEM%\bin;C:\%MSYS2_DIR%\usr\bin;%PATH%"
bash -lc "cd ${APPVEYOR_BUILD_FOLDER} && export CC="gcc.exe" && export CXX="g++.exe" && exec 0</dev/null && mkdir build && cd build && CXX=g++ CC=gcc cmake -G 'MSYS Makefiles' -DDOXYGEN_EXECUTABLE="%DOXYGEN_EXE%" -DCPPUNIT_INCLUDE_DIR="%CPPUNIT_INCLUDE_DIR%" -DCPPUNIT_LIBRARY="%CPPUNIT_LIBRARY%" .. && make -j2"

