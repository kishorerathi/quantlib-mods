# Microsoft Developer Studio Project File - Name="ObjectHandler" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ObjectHandler - Win32 Debug SingleThread
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ObjectHandler.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ObjectHandler.mak" CFG="ObjectHandler - Win32 Debug SingleThread"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ObjectHandler - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ObjectHandler - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "ObjectHandler - Win32 Release SingleThread" (based on "Win32 (x86) Static Library")
!MESSAGE "ObjectHandler - Win32 Debug MTDLL" (based on "Win32 (x86) Static Library")
!MESSAGE "ObjectHandler - Win32 Debug SingleThread" (based on "Win32 (x86) Static Library")
!MESSAGE "ObjectHandler - Win32 Release MTDLL" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ObjectHandler - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "build\vc6\Release"
# PROP Intermediate_Dir "build\vc6\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "." /I "$(LOG4CXX_DIR)/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "LOG4CXX_STATIC" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\ObjectHandler-vc6-mt-s-0_1_1.lib"

!ELSEIF  "$(CFG)" == "ObjectHandler - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ObjectHandler___Win32_Debug"
# PROP BASE Intermediate_Dir "ObjectHandler___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "build\vc6\Debug"
# PROP Intermediate_Dir "build\vc6\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "." /I "$(LOG4CXX_DIR)/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "LOG4CXX_STATIC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\ObjectHandler-vc6-mt-sgd-0_1_1.lib"

!ELSEIF  "$(CFG)" == "ObjectHandler - Win32 Release SingleThread"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ObjectHandler___Win32_Release_SingleThread"
# PROP BASE Intermediate_Dir "ObjectHandler___Win32_Release_SingleThread"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "build\vc6\ReleaseST"
# PROP Intermediate_Dir "build\vc6\ReleaseST"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /I "." /I "$(LOG4CXX_DIR)/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "LOG4CXX_STATIC" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib\ObjectHandler.lib"
# ADD LIB32 /nologo /out:"lib\ObjectHandler-vc6-s-0_1_1.lib"

!ELSEIF  "$(CFG)" == "ObjectHandler - Win32 Debug MTDLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ObjectHandler___Win32_Debug_MTDLL"
# PROP BASE Intermediate_Dir "ObjectHandler___Win32_Debug_MTDLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "build\vc6\DebugMTDLL"
# PROP Intermediate_Dir "build\vc6\DebugMTDLL"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "C:\Program Files\QuantLib" /I "C:\boost_1_31_0" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "." /I "$(LOG4CXX_DIR)/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "LOG4CXX_STATIC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib\ObjectHandler_d.lib"
# ADD LIB32 /nologo /out:"lib\ObjectHandler-vc6-mt-gd-0_1_1.lib"

!ELSEIF  "$(CFG)" == "ObjectHandler - Win32 Debug SingleThread"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ObjectHandler___Win32_Debug_SingleThread"
# PROP BASE Intermediate_Dir "ObjectHandler___Win32_Debug_SingleThread"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "build\vc6\DebugST"
# PROP Intermediate_Dir "build\vc6\DebugST"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "C:\Program Files\QuantLib" /I "C:\boost_1_31_0" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "." /I "$(LOG4CXX_DIR)/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "LOG4CXX_STATIC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib\ObjectHandler_d.lib"
# ADD LIB32 /nologo /out:"lib\ObjectHandler-vc6-sgd-0_1_1.lib"

!ELSEIF  "$(CFG)" == "ObjectHandler - Win32 Release MTDLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ObjectHandler___Win32_Release_MTDLL"
# PROP BASE Intermediate_Dir "ObjectHandler___Win32_Release_MTDLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "build\vc6\ReleaseMTDLL"
# PROP Intermediate_Dir "build\vc6\ReleaseMTDLL"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "." /I "$(LOG4CXX_DIR)/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "LOG4CXX_STATIC" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib\ObjectHandler.lib"
# ADD LIB32 /nologo /out:"lib\ObjectHandler-vc6-mt-0_1_1.lib"

!ENDIF 

# Begin Target

# Name "ObjectHandler - Win32 Release"
# Name "ObjectHandler - Win32 Debug"
# Name "ObjectHandler - Win32 Release SingleThread"
# Name "ObjectHandler - Win32 Debug MTDLL"
# Name "ObjectHandler - Win32 Debug SingleThread"
# Name "ObjectHandler - Win32 Release MTDLL"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\oh\exception.cpp
# End Source File
# Begin Source File

SOURCE=.\oh\logger.cpp
# End Source File
# Begin Source File

SOURCE=.\oh\object.cpp
# End Source File
# Begin Source File

SOURCE=.\oh\objecthandler.cpp
# End Source File
# Begin Source File

SOURCE=.\oh\utilities.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\oh\autolink.hpp
# End Source File
# Begin Source File

SOURCE=.\oh\exception.hpp
# End Source File
# Begin Source File

SOURCE=.\oh\factory.hpp
# End Source File
# Begin Source File

SOURCE=.\oh\logger.hpp
# End Source File
# Begin Source File

SOURCE=.\oh\object.hpp
# End Source File
# Begin Source File

SOURCE=.\oh\objecthandler.hpp
# End Source File
# Begin Source File

SOURCE=.\oh\objhandler.hpp
# End Source File
# Begin Source File

SOURCE=.\oh\objhandlerdefines.hpp
# End Source File
# Begin Source File

SOURCE=.\oh\property.hpp
# End Source File
# Begin Source File

SOURCE=.\oh\singleton.hpp
# End Source File
# Begin Source File

SOURCE=.\oh\utilities.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Announce.txt
# End Source File
# Begin Source File

SOURCE=.\Authors.txt
# End Source File
# Begin Source File

SOURCE=.\autogen.sh
# End Source File
# Begin Source File

SOURCE=.\Contributors.txt
# End Source File
# Begin Source File

SOURCE=.\LICENSE.TXT
# End Source File
# Begin Source File

SOURCE=.\NEWS.txt
# End Source File
# Begin Source File

SOURCE=.\README.txt
# End Source File
# Begin Source File

SOURCE=.\TODO.txt
# End Source File
# End Target
# End Project
