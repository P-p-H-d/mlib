REM Execute the test suite for Microsoft Visual Studio
REM
REM Load Micosoft Visual Studio environement file.
IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)
IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64
)

REM Remove previous results
DEL *.log *.dat

REM List the expected failure
set "expectedFailure=test-msnapshot.c"

REM Perform for each test
for %%f in (test-*.c) do (
    echo Testing %%f
    REM Copy the test file as a C++ file as CL.EXE only support C++ file
    copy %%f test.cpp
    if ERRORLEVEL 1 EXIT /B 1
    REM Compile the test suite
    REM /experimental:preprocessor is mandatory to have a compliant preprocessor
    REM /Zc:__cplusplus is needed to report the real value of __cplusplus, so that M*LIB uses the C++ atomic, and not its emulation.
    cl.exe /I.. /O2 /std:c++14 /Zc:__cplusplus /experimental:preprocessor test.cpp > %%f.log 2>&1 
    if ERRORLEVEL 1 ( 
        type %%f.log 
        echo BUILD ERROR for %%f
        if /i "%%f" NEQ "%expectedFailure%" EXIT /B 1
        )
    REM Execute it
    test.exe >> %%f.log 2>&1 
    if ERRORLEVEL 1 (
        type %%f.log 
        echo RUN ERROR for %%f
        if /i "%%f" NEQ "%expectedFailure%" EXIT /B 1
    )
    type %%f.log
)
exit /B 0
