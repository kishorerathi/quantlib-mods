# Microsoft Developer Studio Project File - Name="AddinExcel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AddinExcel - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AddinExcel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AddinExcel.mak" CFG="AddinExcel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AddinExcel - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AddinExcel - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AddinExcel - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "build\vc6\Release"
# PROP Intermediate_Dir "build\vc6\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ADDINEXCEL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "..\.." /I "$(OBJECT_HANDLER_DIR)" /I "$(QL_DIR)" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ADDINEXCEL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib XLCALL32.LIB log4cxxs.lib Ws2_32.lib /nologo /dll /machine:I386 /out:"xll\QuantLibAddin-vc6-mt-s.xll" /libpath:"$(OBJECT_HANDLER_DIR)\lib" /libpath:"$(QL_DIR)\lib" /libpath:"..\..\lib" /libpath:".\lib" /libpath:"$(LOG4CXX_DIR)/msvc/lib/Release"

!ELSEIF  "$(CFG)" == "AddinExcel - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "build\vc6\Debug"
# PROP Intermediate_Dir "build\vc6\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ADDINEXCEL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\.." /I "$(OBJECT_HANDLER_DIR)" /I "$(QL_DIR)" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ADDINEXCEL_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib XLCALL32.LIB log4cxxs.lib Ws2_32.lib /nologo /dll /debug /machine:I386 /out:"xll\QuantLibAddin-vc6-mt-sgd.xll" /pdbtype:sept /libpath:"$(OBJECT_HANDLER_DIR)\lib" /libpath:"$(QL_DIR)\lib" /libpath:"..\..\lib" /libpath:".\lib" /libpath:"$(LOG4CXX_DIR)/msvc/lib/Debug"

!ENDIF 

# Begin Target

# Name "AddinExcel - Win32 Release"
# Name "AddinExcel - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\capfloor.cpp
# End Source File
# Begin Source File

SOURCE=.\framewrk.cpp
# End Source File
# Begin Source File

SOURCE=.\instruments.cpp
# End Source File
# Begin Source File

SOURCE=.\options.cpp
# End Source File
# Begin Source File

SOURCE=.\processes.cpp
# End Source File
# Begin Source File

SOURCE=.\qladdin.cpp
# End Source File
# Begin Source File

SOURCE=.\qlatest.cpp
# End Source File
# Begin Source File

SOURCE=.\shortratemodels.cpp
# End Source File
# Begin Source File

SOURCE=.\simpleswap.cpp
# End Source File
# Begin Source File

SOURCE=.\termstructures.cpp
# End Source File
# Begin Source File

SOURCE=.\utilities.cpp
# End Source File
# Begin Source File

SOURCE=.\volatilities.cpp
# End Source File
# Begin Source File

SOURCE=.\xibor.cpp
# End Source File
# Begin Source File

SOURCE=.\xlutils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\framewrk.hpp
# End Source File
# Begin Source File

SOURCE=.\XLCALL.H
# End Source File
# Begin Source File

SOURCE=.\xlutils.hpp
# End Source File
# End Group
# End Target
# End Project
