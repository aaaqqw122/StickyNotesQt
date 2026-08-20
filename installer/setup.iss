; StickyNotes Qt 安装脚本 — Inno Setup
; 使用前先运行 build.cmd 编译 Release

#define MyAppName "便签"
#define MyAppVersion "1.0"
#define MyAppPublisher "MyCompany"
#define MyAppURL "https://github.com/yourusername/stickynotes-qt"
#define MyAppExeName "note.exe"

#define ProjectDir "D:\stickynotes-qt"
#define QtDir "D:\QT\6.11.1\msvc2022_64"

[Setup]
AppId={{B8F7A3D2-5E1C-4A9B-9D0F-6C8E7A5B3D1F}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
DisableDirPage=no
OutputDir={#ProjectDir}\installer
OutputBaseFilename=StickyNotesQt_Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=lowest
UninstallDisplayIcon={app}\note.exe

[Languages]
Name: "chinesesimplified"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "创建桌面快捷方式"; GroupDescription: "附加任务:"; Flags: checkedonce
Name: "autostart"; Description: "开机自启动"; GroupDescription: "附加任务:"; Flags: checkedonce

[Files]
; 主程序
Source: "{#ProjectDir}\build-msvc\Release\note.exe"; DestDir: "{app}"; Flags: ignoreversion

; Qt6 运行时 DLL
Source: "{#QtDir}\bin\Qt6Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt6Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\bin\Qt6Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion

; Qt 平台插件
Source: "{#QtDir}\plugins\platforms\qwindows.dll"; DestDir: "{app}\platforms"; Flags: ignoreversion

; Qt 图片格式插件（支持 jpg/png/gif/bmp/webp/ico 等）
Source: "{#QtDir}\plugins\imageformats\qgif.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtDir}\plugins\imageformats\qico.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtDir}\plugins\imageformats\qjpeg.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtDir}\plugins\imageformats\qtga.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtDir}\plugins\imageformats\qtiff.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtDir}\plugins\imageformats\qwbmp.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtDir}\plugins\imageformats\qwebp.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion

; 资源文件
Source: "{#ProjectDir}\resources\icon\favicon.ico"; DestDir: "{app}\resources\icon"; Flags: ignoreversion

[UninstallRun]
Filename: "{sys}\taskkill"; Parameters: "/f /im note.exe"; Flags: runhidden

[Registry]
; 开机自启动（与程序内的开关共用同一注册表位置）
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; \
    ValueType: string; ValueName: "StickyNotesQt"; \
    ValueData: "{app}\note.exe"; Tasks: autostart; Flags: uninsdeletevalue

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; \
    IconFilename: "{app}\resources\icon\favicon.ico"
Name: "{group}\卸载 {#MyAppName}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; \
    IconFilename: "{app}\resources\icon\favicon.ico"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "启动 {#MyAppName}"; Flags: nowait postinstall skipifsilent
