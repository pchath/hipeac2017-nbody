@ECHO OFF
setlocal
Rem ****************************************************************************
Rem 
Rem  Copyright (C) 2015 Intel Corporation. All Rights Reserved.
Rem 
Rem  The source code contained or described herein and all
Rem  documents related to the source code ("Material") are owned by
Rem  Intel Corporation or its suppliers or licensors. Title to the
Rem  Material remains with Intel Corporation or its suppliers and
Rem  licensors. The Material is protected by worldwide copyright
Rem  laws and treaty provisions.  No part of the Material may be
Rem  used, copied, reproduced, modified, published, uploaded,
Rem  posted, transmitted, distributed,  or disclosed in any way
Rem  except as expressly provided in the license provided with the
Rem  Materials.  No license under any patent, copyright, trade
Rem  secret or other intellectual property right is granted to or
Rem  conferred upon you by disclosure or delivery of the Materials,
Rem  either expressly, by implication, inducement, estoppel or
Rem  otherwise, except as expressly provided in the license
Rem  provided with the Materials.
Rem 
Rem ****************************************************************************
Rem Content:
Rem      Windows Build Script for Sample vec_samples
Rem
Rem Part of the vec_samples tutorial. See "Tutorial: Auto-vectorization"
Rem in the Intel(R) C++ Compiler Tutorials document
Rem ****************************************************************************

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
@ECHO ON



@ECHO OFF
endlocal