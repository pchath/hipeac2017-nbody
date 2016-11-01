@ECHO OFF
setlocal

del *.o *.obj *.exe *.pdb *.ilk > nul 2>&1
if ""%1"" == ""clean"" exit /b

set CFLAGS=/O2 /Qstd=c99 /fp:fast /I. /Zi /Qopenmp /Qvec /Qsimd /Qopt-report:5 /DEBUG /DMKL_ILP64 -I"%MKLROOT%"\include  mkl_intel_ilp64.lib mkl_sequential.lib mkl_core.lib /DWIN_32

if ""%1"" == ""orig"" (
icl /nologo %CFLAGS% /c timer.cpp nbody-seq-original.cpp
icl %CFLAGS% /Fe:nbody-seq-orig *.obj
)

if ""%1"" == ""align"" (
icl /nologo %CFLAGS% /c timer.cpp nbody-seq-opt-align.cpp
icl %CFLAGS% /Fe:nbody-seq-opt-align *.obj
)

if ""%1"" == ""tile"" (
icl /nologo %CFLAGS% /c timer.cpp nbody-seq-opt-align-tile.cpp
icl %CFLAGS% /Fe:nbody-seq-opt-align-tile *.obj
)

if ""%1"" == ""par"" (
icl /nologo %CFLAGS% /c timer.cpp nbody-par.cpp
icl %CFLAGS% /Fe:nbody-par *.obj
)

if ""%1"" == ""par-opt"" (
icl /nologo %CFLAGS% /c timer.cpp nbody-par-opt.cpp
icl %CFLAGS% /Fe:nbody-par-opt *.obj
)

@ECHO ON



@ECHO OFF
endlocal