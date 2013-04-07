#define APPTITLE "SpeedCrunch"
#define SPEEDCRUNCHEXE "release\" + APPTITLE + ".exe"
#define NUMERICVERSION GetFileVersion(SPEEDCRUNCHEXE)
#define VERSION "trunk" ; GetFileVersionString(SPEEDCRUNCHEXE)
#define URL "http://speedcrunch.org"
#define COPYRIGHT "2004-2013 " + URL

[Setup]
AllowNoIcons=yes
AppName={#APPTITLE}
AppPublisher={#APPTITLE}
AppPublisherURL={#URL}
AppSupportURL={#URL}
AppUpdatesURL={#URL}
AppVerName={#APPTITLE} {#VERSION}
Compression=lzma/ultra
DefaultDirName={pf}\{#APPTITLE}
DefaultGroupName={#APPTITLE}
DisableProgramGroupPage=true
LicenseFile=..\doc\COPYING.rtf
InternalCompressLevel=ultra
OutputBaseFilename={#APPTITLE}-{#VERSION}
OutputDir=.
ShowLanguageDialog=no
SolidCompression=yes
VersionInfoCompany={#URL}
VersionInfoCopyright=Copyright (C) {#COPYRIGHT}
VersionInfoDescription=Fast, high precision and powerful desktop calculator
VersionInfoTextVersion={#VERSION}
VersionInfoVersion={#NUMERICVERSION}
WizardImageFile=compiler:WizModernImage-IS.bmp
WizardSmallImageFile=compiler:WizModernSmallImage-IS.bmp

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "basque"; MessagesFile: "compiler:Languages\Basque.isl"
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "catalan"; MessagesFile: "compiler:Languages\Catalan.isl"
Name: "czech"; MessagesFile: "compiler:Languages\Czech.isl"
Name: "danish"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "dutch"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "finnish"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
Name: "hungarian"; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "italian"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "norwegian"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "polish"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "portuguese"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "slovak"; MessagesFile: "compiler:Languages\Slovak.isl"
Name: "slovenian"; MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: {#SPEEDCRUNCHEXE}; DestDir: "{app}"; Flags: ignoreversion
Source: locale\*.qm; DestDir: "{app}\locale"; Flags: ignoreversion
Source: book\*; Excludes: "*.sh,ut3\*,templates\*,locale\*,*.mml,*.svgz"; DestDir: "{app}\book"; Flags: ignoreversion recursesubdirs

[Icons]
Name: "{group}\{#APPTITLE}"; Filename: "{app}\{#APPTITLE}.exe"
Name: "{group}\{cm:UninstallProgram,{#APPTITLE}}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\{#APPTITLE}"; Filename: "{app}\{#APPTITLE}.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#APPTITLE}"; Filename: "{app}\{#APPTITLE}.exe"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\{#APPTITLE}.exe"; Description: "{cm:LaunchProgram,{#APPTITLE}}"; Flags: nowait postinstall skipifsilent
