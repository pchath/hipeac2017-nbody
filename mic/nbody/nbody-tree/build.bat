@ECHO OFF
setlocal

del *.o *.obj *.exe *.pdb *.ilk > nul 2>&1
if ""%1"" == ""clean"" exit /b

set CFLAGS=/O2 /Qstd=c99 /fp:fast /I. /Zi /Qopenmp /Qvec /Qsimd /Qopt-report:5 /DEBUG /DMKL_ILP64 -I"%MKLROOT%"\include  mkl_intel_ilp64.lib mkl_sequential.lib mkl_core.lib /DWIN_32

if ""%1"" == ""orig"" (
icl /nologo %CFLAGS% /c quadtree.cpp timer.cpp nbody-tree-orig.cpp
icl %CFLAGS% /Fe:nbody-tree-orig *.obj
)


@ECHO ON



@ECHO OFF
endlocal