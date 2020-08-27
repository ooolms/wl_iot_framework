[Files]
Source: AzDevHelperTool.exe; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: .\vcredist_x86.2015.exe; DestDir: {app}; Flags: overwritereadonly replacesameversion
;libs
Source: C:\Qt\extlibs\icu-msvc\lib\icudt54.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\extlibs\icu-msvc\lib\icuin54.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\extlibs\icu-msvc\lib\icuuc54.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\resources\icudtl.dat; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\bin\Qt5Core.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\bin\Qt5Gui.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\bin\Qt5OpenGL.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\bin\Qt5Network.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\bin\Qt5SerialPort.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\bin\Qt5Sql.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\bin\Qt5Xml.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\bin\Qt5XmlPatterns.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\bin\Qt5Widgets.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\bin\opengl32sw.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
;qt plugins
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\plugins\imageformats\qgif.dll; DestDir: {app}\imageformats; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\plugins\imageformats\qico.dll; DestDir: {app}\imageformats; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\plugins\imageformats\qjpeg.dll; DestDir: {app}\imageformats; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\plugins\imageformats\qsvg.dll; DestDir: {app}\imageformats; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\plugins\imageformats\qtga.dll; DestDir: {app}\imageformats; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\plugins\imageformats\qtiff.dll; DestDir: {app}\imageformats; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\plugins\imageformats\qwbmp.dll; DestDir: {app}\imageformats; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\plugins\imageformats\qwebp.dll; DestDir: {app}\imageformats; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\plugins\sqldrivers\qsqlite.dll; DestDir: {app}\sqldrivers; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\plugins\platforms\qwindows.dll; DestDir: {app}\platforms; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\plugins\platforms\qminimal.dll; DestDir: {app}\platforms; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\plugins\platforms\qoffscreen.dll; DestDir: {app}\platforms; Flags: overwritereadonly replacesameversion
Source: C:\Qt\Qt5.6.2-msvc2015\5.6\msvc2015\plugins\printsupport\windowsprintersupport.dll; DestDir: {app}\printsupport; Flags: overwritereadonly replacesameversion
[Icons]
Name: {group}\AzDevHelperTool; Filename: {app}\AzDevHelperTool.exe; WorkingDir: {app}
Name: {group}\Uninstall; Filename: {app}\unins000.exe; WorkingDir: {app}
[UninstallDelete]
Name: {app}; Type: filesandordirs
[Setup]
InternalCompressLevel=ultra
Compression=lzma/ultra
AppCopyright=LGPLv3
AppName=AzDevHelperTool
AppVerName=0.1.0
RestartIfNeededByRun=false
AllowCancelDuringInstall=false
DefaultDirName={pf}\AzDevHelperTool
DefaultGroupName=AzDevHelperTool
UsePreviousGroup=false
ShowLanguageDialog=no
CreateUninstallRegKey=false
OutputBaseFilename=AzDevHelperTool
VersionInfoVersion=1
[Run]
Filename: "{app}\vcredist_x86.2015.exe"; Parameters: "/install /passive"; Flags: waituntilterminated
