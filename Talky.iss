; Talky.iss - Inno Setup Script
[Setup]
AppName=Talky AI
AppVersion=1.0
DefaultDirName={autopf}\Talky
DefaultGroupName=Talky
OutputDir=./dist/installer
OutputBaseFilename=TalkySetup
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64
SetupIconFile=logo.ico
UninstallDisplayIcon={app}\bin\Talky.exe

[Files]
; Main Application
Source: "dist\bin\*"; DestDir: "{app}\bin"; Flags: ignoreversion recursesubdirs
; AI Models
Source: "dist\models\*"; DestDir: "{app}\models"; Flags: ignoreversion recursesubdirs
; Virtual Audio Cable Driver
Source: "drivers\VBCABLE_Setup_x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall

[Icons]
Name: "{group}\Talky"; Filename: "{app}\bin\Talky.exe"
Name: "{autodesktop}\Talky"; Filename: "{app}\bin\Talky.exe"

[Run]
; Install VAC silently if selected
Filename: "{tmp}\VBCABLE_Setup_x64.exe"; Parameters: "/silent"; StatusMsg: "Installing Virtual Audio Cable..."; Flags: runascurrentuser

[Code]
// Example: Check for NVIDIA GPU before finishing
function IsNvidiaGPUPresent(): Boolean;
begin
  // Add logic to check registry or system info
  Result := True; 
end;
