# Microsoft Developer Studio Project File - Name="GenCN" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=GenCN - Win32 Release
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "NNsiMona.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "NNsiMona.mak" CFG="GenCN - Win32 Release"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "GenCN - Win32 Debug" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "GenCN - Win32 Release" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows Software/HotSync/GenCn", MORBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GenCN - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(CDKDIR)\C++\pfc\include" /I ".\include" /I "$(CDKDIR)\C++\include" /I ".\\" /D "_DEBUG" /D "_NOMFC" /D "WIN32" /D "_WINDOWS" /D "_CONDUIT_" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 sync20.lib hslog20.lib palmcmn.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept /libpath:"$(CDKDIR)\C++\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "GenCN - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "$(CDKDIR)\C++\pfc\include" /I ".\include" /I ".\\" /I "$(CDKDIR)\C++\include" /D "NDEBUG" /D "_NOMFC" /D "WIN32" /D "_WINDOWS" /D "_CONDUIT_" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 sync20.lib hslog20.lib palmcmn.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /dll /machine:I386 /libpath:"$(CDKDIR)\C++\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "GenCN - Win32 Debug"
# Name "GenCN - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "*.cpp,*.c,*.rc"
# Begin Source File

SOURCE=.\Src\archive.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\bckupmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\CLog.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\gencat.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\gensync.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\hhmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\pcmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PostSync.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SyncActn.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "*.h,*.hh"
# Begin Source File

SOURCE=.\Include\bckupmgr.h
# End Source File
# Begin Source File

SOURCE=.\Include\CLog.h
# End Source File
# Begin Source File

SOURCE=.\Include\GenericConduitHeaders.h
# End Source File
# Begin Source File

SOURCE=.\Include\GenSync.h
# End Source File
# Begin Source File

SOURCE=.\Include\hhmgr.h
# End Source File
# Begin Source File

SOURCE=.\Include\pcmgr.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Palm_FC"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE="$(CDKDIR)\C++\Pfc\Src\CPalmRec.cpp"
# End Source File
# Begin Source File

SOURCE="$(CDKDIR)\C++\Pfc\Src\CPCatgry.cpp"
# End Source File
# Begin Source File

SOURCE="$(CDKDIR)\C++\Pfc\Src\CPDbBMgr.cpp"
# End Source File
# Begin Source File

SOURCE="$(CDKDIR)\C++\Pfc\Src\CPString.cpp"
# End Source File
# End Group
# Begin Group "NNsiMona"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\NNsiHHMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\NNsiHHMgr.h
# End Source File
# Begin Source File

SOURCE=.\NNsiMona.CPP
# End Source File
# Begin Source File

SOURCE=.\NNsiMona.H
# End Source File
# Begin Source File

SOURCE=.\res\NNsiMona.rc
# End Source File
# Begin Source File

SOURCE=.\NNsiSync.CPP
# End Source File
# Begin Source File

SOURCE=.\NNsiSync.H
# End Source File
# Begin Source File

SOURCE=.\NNsiSyncExt.cpp
# End Source File
# Begin Source File

SOURCE=.\symbols.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\bmp\DoNothing.bmp
# End Source File
# Begin Source File

SOURCE=.\res\DoNothing.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\HhToPc.bmp
# End Source File
# Begin Source File

SOURCE=.\res\HhToPc.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\PcToHh.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PcToHh.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp\Synchronize.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Synchronize.bmp
# End Source File
# End Target
# End Project
