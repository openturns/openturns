; 
; Setup script in order to create a windows auto-installer for OpenTURNS and its dependancies.
;
; To lauch the creation of the installer :
;   makensis  -DOPENTURNS_PREFIX=/absolute/path -DPRODUCT_VERSION=0.13.2  openturns.nsi
;

!define PRODUCT_NAME "OpenTURNS"

!include "common.nsi"

; deactivate some sections to speed up compilation
;!define DEBUG

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "openturns.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "OpenTURNS.bmp"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "OpenTURNS_header.bmp"
BrandingText "OpenTURNS ${PRODUCT_VERSION}"

; Language Selection Dialog Settings
!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"
!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"

!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; Welcome page
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of ${PRODUCT_NAME} ${PRODUCT_VERSION}.\r\rThis installer has been tested on Windows 2000, XP and Vista. Although OpenTURNS may work on it, other operating systems are not supported."
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "COPYING.txt"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!define MUI_DIRECTORYPAGE_TEXT_TOP "Setup will install ${PRODUCT_NAME} ${PRODUCT_VERSION} in the following folder. To install in a different folder, click Browse and select another folder.$\r$\rOpenTURNS must be installed in an already installed Python directory (e.g. ${Python_default_INSTALL_PATH})."
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_TEXT "${PRODUCT_NAME} ${PRODUCT_VERSION} has been installed on your computer.\r\n\r\n${PRODUCT_NAME} can be launched from the start menu."
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "openturns-${PRODUCT_VERSION}-py${PYBASEVER}-${ARCH}.exe"
InstallDir "${Python_default_INSTALL_PATH}\"
ShowInstDetails show
ShowUnInstDetails show

Var Python_INSTALL_PATH 
Var R_EXE

; set $Python_INSTALL_PATH to python dir found
Function CheckPython
  ClearErrors

  ; user set the python path
  ${If} $INSTDIR != "${Python_default_INSTALL_PATH}"
    StrCpy $Python_INSTALL_PATH "$INSTDIR"
  ${Else}
    ; search the prog in the Windows registry
    ReadRegStr $Python_INSTALL_PATH HKLM "Software\Python\PythonCore\${PYBASEVER}\InstallPath" ""
    ${If} $Python_INSTALL_PATH == ""
      !insertmacro PRINT "! Python not found in machine registry, try user registry."
      ReadRegStr $Python_INSTALL_PATH HKCU "Software\Python\PythonCore\${PYBASEVER}\InstallPath" ""
    ${EndIf}

    ${If} $Python_INSTALL_PATH == ""
      !insertmacro PRINT "! Python not found in registry, try default directory (${Python_default_INSTALL_PATH}) ."
      StrCpy $Python_INSTALL_PATH "${Python_default_INSTALL_PATH}"
    ${EndIf}
  ${EndIf}

  ; Check that the python exe is there
  IfFileExists "$Python_INSTALL_PATH\python.exe" 0 python_not_found_error
    !insertmacro PRINT "=> Python found here: $Python_INSTALL_PATH."
  Goto python_not_found_error_end
  python_not_found_error:
    StrCpy $Python_INSTALL_PATH ""
    !insertmacro PRINT "! Python not found !"
  python_not_found_error_end:

FunctionEnd



; Launch before the section are displayed
Function .onInit
  !insertmacro CHECK_REG_VIEW

  !insertmacro MUI_LANGDLL_DISPLAY

  !insertmacro CHECK_USER_INSTALL "Installation switched to user install."


  !insertmacro PRINT "CheckPython."
  Call CheckPython

  ${If} $Python_INSTALL_PATH == ""
    MessageBox MB_OK|MB_ICONEXCLAMATION "Python ${PYBASEVER} installation directory not found!$\rEnter manually the Python installation directory." /SD IDOK
    ; abort if silent install and not FORCE flag
    IfSilent 0 end_abort
    ${GetParameters} $R1
    ClearErrors
    ${GetOptions} $R1 '/FORCE' $R0
    IfErrors 0 +2
    Abort
    end_abort:
  ${Else} 
    ; if there is a \ at the end of Python_INSTALL_PATH: remove it
    StrCpy $0 "$Python_INSTALL_PATH" "" -1
    ${if} $0 == "\" 
      StrCpy $0 "$Python_INSTALL_PATH" -1
      StrCpy $Python_INSTALL_PATH "$0"
    ${EndIf}

    StrCpy $INSTDIR "$Python_INSTALL_PATH"
    StrCpy $OT_INSTALL_PATH "$Python_INSTALL_PATH\Lib\site-packages\openturns"
    ; MessageBox MB_OK|MB_ICONEXCLAMATION "Python found in $Python_INSTALL_PATH." /SD IDOK
    ; !insertmacro PRINT "Python $PYBASEVER found in directory $Python_INSTALL_PATH."
  ${EndIf}


  ; if OT already installed, uninstall previous OT.
  ; search first in python directory
  IfFileExists "$OT_INSTALL_PATH\uninst.exe" 0 search_elsewhere
  StrCpy $R0 "$OT_INSTALL_PATH"
  Goto end_search_elsewhere
  search_elsewhere:
  ; search too elsewhere
  Call CheckOpenTURNS
  end_search_elsewhere:

  ${If} $R0 != ""
    IfFileExists "$R0\uninst.exe" 0 continue_anyway

    MessageBox MB_YESNO|MB_ICONEXCLAMATION "OpenTURNS $R1 is already installed in directory $R0.$\rDo you want to uninstall this installed version (recommended)?" /SD IDYES IDNO continue_anyway

    ; copy uninstaller to temp dir in order to erase the whole ot dir
    ; _? option permit to avoid uninstaller to copy itself to tempdir. it permit too to make ExecWait work
    CopyFiles "$R0\uninst.exe" $TEMP
    IfSilent 0 +3
    ; silent uninstall
    ExecWait '"$TEMP\uninst.exe" /S _?=$R0'
    Goto +2
    ExecWait '"$TEMP\uninst.exe" _?=$R0'

    continue_anyway:
  ${EndIf}


  ; Avoid huge logs of File function. PRINT macro permit to put user log.
  SetDetailsPrint none
  SetAutoClose false
FunctionEnd


Section "!OpenTURNS" SEC01
  SetOverwrite on

  ; reread $INSTDIR in case user change it.
  StrCpy $Python_INSTALL_PATH "$INSTDIR"
  StrCpy $OT_INSTALL_PATH "$Python_INSTALL_PATH\Lib\site-packages\openturns"

  SetDetailsPrint both
  ClearErrors
  CreateDirectory "$OT_INSTALL_PATH"
  IfErrors permisssion_nok permission_ok
  permisssion_nok:
  MessageBox MB_OK|MB_ICONEXCLAMATION "Failed to create OpenTURNS directory $OT_INSTALL_PATH!$\rCheck directory permission.$\rInstallation aborted." /SD IDOK
  Abort
  permission_ok:
  SetDetailsPrint none

  !insertmacro PRINT "Install OT to $OT_INSTALL_PATH."
  SetOutPath "$OT_INSTALL_PATH"
  File "COPYING.txt"
  File "README.txt"
  File openturns.ico
  File "OpenTURNSDoc.url"

!ifndef DEBUG
  !insertmacro PRINT "Install binary files."
  File /r "${OPENTURNS_PREFIX}\bin\*.*"

  !insertmacro PRINT "Install python files."
  File /r "${OPENTURNS_PREFIX}\Lib\site-packages\openturns\*.*"

  !insertmacro PRINT "Install conf files."
  File /r "${OPENTURNS_PREFIX}\etc\openturns\*.*"
!endif

  # create a version file
  ClearErrors
  FileOpen $0 $OT_INSTALL_PATH\VERSION.txt w
  IfErrors versionfile_fail
  FileWrite $0 "OpenTURNS ${PRODUCT_VERSION}"
  FileClose $0
  versionfile_fail:

  ${If} "$UserInstall" == "0"
    !insertmacro PRINT "Put OpenTURNS in windows registry."
    WriteRegStr ${PRODUCT_INST_ROOT_KEY} ${PRODUCT_DIR_REGKEY} "Current Version" "${PRODUCT_VERSION}"
    WriteRegStr ${PRODUCT_INST_ROOT_KEY} ${PRODUCT_DIR_REGKEY} "InstallPath" "$OT_INSTALL_PATH"
  ${Else}
    !insertmacro CREATE_USER_INSTALL_FILE "$OT_INSTALL_PATH\UserInstall"
  ${EndIf}

  !insertmacro PRINT "Install uninstaller in $OT_INSTALL_PATH\uninst.exe."
  ; Change instdir for uninstaller
  StrCpy $INSTDIR "$OT_INSTALL_PATH"
  WriteUninstaller "$OT_INSTALL_PATH\uninst.exe"
  ${If} "$UserInstall" == "0"
    !insertmacro PRINT "Put ${PRODUCT_NAME} uninstaller in registry."
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$OT_INSTALL_PATH\uninst.exe"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  ${EndIf}
SectionEnd


Section "OT python examples" SEC02
  SetOverwrite on
!ifndef DEBUG

  !insertmacro PRINT "Install Python examples in $OT_INSTALL_PATH\examples."
  SetOutPath "$OT_INSTALL_PATH\examples"
  File "${OPENTURNS_PREFIX}\share\openturns\examples\*.py"
  File "_drag_to_me_to_start_it.bat"
  ${ConfigWrite} "_drag_to_me_to_start_it.bat" "set PYTHON_INSTALL_PATH=" "$\"$Python_INSTALL_PATH$\"" $R0
!endif
SectionEnd


Section -AdditionalIcons
  !insertmacro PRINT "Create OpenTURNS menu."
  ; install shortcuts on every accounts
  !insertmacro SET_MENU_CONTEXT
  CreateDirectory "$SMPROGRAMS\OpenTURNS"
  CreateShortCut "$SMPROGRAMS\OpenTURNS\Python.lnk" "$Python_INSTALL_PATH\python.exe" "" "" 0
  CreateShortCut "$SMPROGRAMS\OpenTURNS\README.lnk"   "$OT_INSTALL_PATH\README.txt" "" "" 1
  CreateShortCut "$SMPROGRAMS\OpenTURNS\Uninstall.lnk" "$OT_INSTALL_PATH\uninst.exe" "" "" 2
  CreateShortCut "$SMPROGRAMS\OpenTURNS\Documentation.lnk" "$OT_INSTALL_PATH\OpenTURNSDoc.url" "" "" 3

  ; ipython if present
  StrCpy $0 "$Python_INSTALL_PATH\Scripts\ipython.exe"
  IfFileExists "$0" 0 ipython_not_found
  !insertmacro PRINT "Create shortcut for IPython (found here: $0)."
  CreateShortCut "$SMPROGRAMS\OpenTURNS\IPython.lnk" "$0" "" "" 3
  ;Goto ipython_not_found_end
  ipython_not_found:
  ;!insertmacro PRINT "Do not create IPython shortcut."
  ;ipython_not_found_end:
SectionEnd


; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "OpenTURNS python libraries. Mandatory."
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC02} "OpenTURNS python examples."
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE

  ; Get previous installation mode
  !insertmacro CHECK_USER_INSTALL_FILE "$INSTDIR\UserInstall"

  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Do you want to remove $(^Name) (directory $\"$INSTDIR$\" will be erased)?" /SD IDYES IDYES +2
  Abort
FunctionEnd


Section Uninstall
  ; nsis can't delete current directory
  SetOutPath $TEMP

  SetDetailsPrint textonly

  ; do not delete if $INSTDIR not set
  !insertmacro PRINT "Delete $\"$INSTDIR$\" directory."
  StrCmp "$INSTDIR" "" +2
  RMDir /r "$INSTDIR"

  !insertmacro SET_MENU_CONTEXT
  SetDetailsPrint both

  !insertmacro PRINT "Delete ${PRODUCT_NAME} menu."
  RMDir /r "$SMPROGRAMS\OpenTURNS"

  ${If} "$UserInstall" == "0"
    !insertmacro PRINT "Delete ${PRODUCT_NAME} registry entry."
    DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
    DeleteRegKey ${PRODUCT_INST_ROOT_KEY} "${PRODUCT_DIR_REGKEY}"
  ${EndIf}

  SetAutoClose false
SectionEnd
