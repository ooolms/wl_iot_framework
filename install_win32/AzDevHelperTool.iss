[Files]
Source: AzDevHelperTool.exe; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: .\vcredist_x64.2019.exe; DestDir: {app}; Flags: overwritereadonly replacesameversion
;libs
Source: C:\Qt\5.15.2\msvc2019_64\resources\icudtl.dat; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\bin\Qt5Core.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\bin\Qt5Gui.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\bin\Qt5OpenGL.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\bin\Qt5Network.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\bin\Qt5SerialPort.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\bin\Qt5Sql.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\bin\Qt5Xml.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\bin\Qt5XmlPatterns.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\bin\Qt5Widgets.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\bin\opengl32sw.dll; DestDir: {app}; Flags: overwritereadonly replacesameversion
;qt plugins
Source: C:\Qt\5.15.2\msvc2019_64\plugins\imageformats\qgif.dll; DestDir: {app}\imageformats; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\plugins\imageformats\qico.dll; DestDir: {app}\imageformats; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\plugins\imageformats\qjpeg.dll; DestDir: {app}\imageformats; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\plugins\imageformats\qsvg.dll; DestDir: {app}\imageformats; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\plugins\imageformats\qtga.dll; DestDir: {app}\imageformats; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\plugins\imageformats\qtiff.dll; DestDir: {app}\imageformats; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\plugins\imageformats\qwbmp.dll; DestDir: {app}\imageformats; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\plugins\imageformats\qwebp.dll; DestDir: {app}\imageformats; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\plugins\sqldrivers\qsqlite.dll; DestDir: {app}\sqldrivers; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\plugins\platforms\qwindows.dll; DestDir: {app}\platforms; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\plugins\platforms\qminimal.dll; DestDir: {app}\platforms; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\plugins\platforms\qoffscreen.dll; DestDir: {app}\platforms; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\plugins\styles\qwindowsvistastyle.dll; DestDir: {app}\styles; Flags: overwritereadonly replacesameversion
Source: C:\Qt\5.15.2\msvc2019_64\plugins\printsupport\windowsprintersupport.dll; DestDir: {app}\printsupport; Flags: overwritereadonly replacesameversion
[Icons]
Name: {group}\AzDevHelperTool; Filename: {app}\AzDevHelperTool.exe; WorkingDir: {app}
Name: {group}\Uninstall; Filename: {app}\unins000.exe; WorkingDir: {app}
[UninstallDelete]
Name: {app}; Type: filesandordirs
[Setup]
Compression=lzma2/normal
ArchitecturesInstallIn64BitMode=x64
AppCopyright=Copyright (C) 2011-2020 OOO "LMS"
AppName=AzDevHelperTool
AppVerName=0.1.1
RestartIfNeededByRun=false
AllowCancelDuringInstall=false
DefaultDirName={commonpf}\AzDevHelperTool
DefaultGroupName=AzDevHelperTool
UsePreviousGroup=false
ShowLanguageDialog=no
CreateUninstallRegKey=false
OutputBaseFilename=AzDevHelperTool
VersionInfoVersion=1
[Run]
Filename: "{app}\vcredist_x64.2019.exe"; Parameters: "/install /passive"; Flags: waituntilterminated
