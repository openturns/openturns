; 
; Setup script in order to create a windows auto-installer for OpenTURNS and its dependencies.
;
; To lauch the creation of the installer :
;   makensis  -DOPENTURNS_PREFIX=/absolute/path -DPRODUCT_VERSION=0.13.2  openturns_developer.nsi
;

!define PRODUCT_NAME "OpenTURNS for developer"

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

; Welcome page
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of ${PRODUCT_NAME} ${PRODUCT_VERSION}."
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "COPYING.txt"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
!define MUI_DIRECTORYPAGE_TEXT_TOP "Setup will install ${PRODUCT_NAME} ${PRODUCT_VERSION} in the following folder. To install in a different folder, click Browse and select another folder.$\r$\r${PRODUCT_NAME} must be installed in an already installed OpenTURNS directory (e.g. ${Default_OT_INSTALL_PATH})."
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_TEXT "${PRODUCT_NAME} ${PRODUCT_VERSION} has been installed on your computer.\r\n\r\nTo uninstall it, launch uninst-developer.exe."
!insertmacro MUI_PAGE_FINISH

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "openturns-developer-${PRODUCT_VERSION}-${ARCH}.exe"
InstallDir "${Default_OT_INSTALL_PATH}"
ShowInstDetails show
ShowUnInstDetails show


; Launch before the section are displayed
Function .onInit
  !insertmacro CHECK_REG_VIEW
  
  !insertmacro MUI_LANGDLL_DISPLAY

  !insertmacro CHECK_USER_INSTALL "Installation switched to user install."

  Call CheckOpenTURNS
  ${If} $R0 != ""

    StrCpy $OT_INSTALL_PATH $R0
    StrCpy $INSTDIR $R0
    ; Search the coresponding OpenTURNS version
    ${If} $R1 == "${PRODUCT_VERSION}"
      !insertmacro PRINT "=> OpenTURNS ${PRODUCT_VERSION} found here: $OT_INSTALL_PATH."
    ${Else}
      MessageBox MB_YESNO|MB_ICONEXCLAMATION "OpenTURNS has been found but with another version (found version $R1)!\
        $\rDo you want to install ${PRODUCT_NAME} anyway?" /SD IDNO IDYES openturns_install
      abort
      openturns_install:
    ${EndIf}

  ${Else}

    MessageBox MB_OK|MB_ICONEXCLAMATION "OpenTURNS installation directory not found!$\r$\rEnter manually the OpenTURNS installation directory." /SD IDOK
    ; abort if silent install
    IfSilent 0 end_abort
    Abort
    end_abort:

  ${EndIf}

  SetAutoClose false
  ; Avoid huge logs of File function
  SetDetailsPrint none
FunctionEnd



Var MinGW_INSTALL_PATH

Section "!OpenTURNS MinGW" SEC01

  ; reread $INSTDIR in case user change it. (fixme: OT_INSTALL_PATH should be INSTDIR)
  StrCpy $OT_INSTALL_PATH "$INSTDIR"
  StrCpy $MinGW_INSTALL_PATH "$OT_INSTALL_PATH\MinGW"


  ; Ot install and OT-dev install should have the same install mode
  IfFileExists "$OT_INSTALL_PATH\UserInstall" 0 ot_admin_install
    ${If} "$UserInstall" == "0"
      MessageBox MB_OK|MB_ICONINFORMATION "OpenTURNS was installed in user mode. You are installing ${PRODUCT_NAME} in admin mode.$\r$\rSome ${PRODUCT_NAME} fonctionality will perhaps be not available." /SD IDOK
    ${EndIf}
  Goto end_ot_admin_install
  ot_admin_install:
    ${If} "$UserInstall" == "1"
      MessageBox MB_OK|MB_ICONINFORMATION "OpenTURNS was installed in admin mode. You are installing ${PRODUCT_NAME} in user mode.$\r$\rSome ${PRODUCT_NAME} fonctionality will perhaps be not available." /SD IDOK
    ${EndIf}
  end_ot_admin_install:


  !insertmacro PRINT "Extracting MinGW to $MinGW_INSTALL_PATH."
  SetOutPath "$MinGW_INSTALL_PATH"
  File /r "${WINDEPS}\opt\MinGW\*.*"

  !insertmacro PRINT "Extracting OT deps headers to $MinGW_INSTALL_PATH\include."
  SetOutPath "$MinGW_INSTALL_PATH\include"
  ;File /r "${WINDEPS}\include\*.*"

  !insertmacro PRINT "Extracting OT deps libraries to $MinGW_INSTALL_PATH\lib."
  SetOutPath "$MinGW_INSTALL_PATH\lib"
  ;File /r "${WINDEPS}\lib\*.*"

  !insertmacro PRINT "Extracting OT headers to $OT_INSTALL_PATH\include."
  SetOutPath "$OT_INSTALL_PATH\include"
  File /r "${OPENTURNS_PREFIX}\include\openturns\*.*"

  !insertmacro PRINT "Install uninstaller in $OT_INSTALL_PATH\uninst-developer.exe."
  WriteUninstaller "$OT_INSTALL_PATH\uninst-developer.exe"

  ${If} "$UserInstall" != "0"
    !insertmacro CREATE_USER_INSTALL_FILE "$OT_INSTALL_PATH\UserInstallDev"
  ${EndIf}

SectionEnd


Section "OpenTURNS checktests" SEC02

  SetOverwrite on

  !insertmacro PRINT "Install checktests in $OT_INSTALL_PATH\examples."
  SetOutPath "$OT_INSTALL_PATH\examples"

!ifndef DEBUG
  ; delete the dead link used for 'make installcheck'
  File /r /x "libOT-0.dll" "${OPENTURNS_PREFIX}\share\openturns\examples\*.*"
!endif
  File "start_check_test.sh"
  File "start_pythoninstallcheck_test.sh"
  File "start_all_check_test.sh"
  ${ConfigWrite} "$OT_INSTALL_PATH\examples\start_all_check_test.sh" "OT_PREFIX=" '"$OT_INSTALL_PATH"' $R0

SectionEnd


Section -AdditionalIcons
  !insertmacro PRINT "Create a shortcut in OpenTURNS menu to start checktests."

  !insertmacro SET_MENU_CONTEXT

  CreateShortCut "$SMPROGRAMS\OpenTURNS\Uninstall-developer.lnk" "$OT_INSTALL_PATH\uninst-developer.exe"
  CreateShortCut "$SMPROGRAMS\OpenTURNS\start-checktests.lnk" "$MinGW_INSTALL_PATH\msys\1.0\bin\sh.exe" '"$OT_INSTALL_PATH\examples\start_all_check_test.sh"' "$OT_INSTALL_PATH\openturns.ico"
  SetOutPath "$MinGW_INSTALL_PATH\msys\1.0"
  CreateShortCut "$SMPROGRAMS\OpenTURNS\MSYS-Shell.lnk" "$MinGW_INSTALL_PATH\msys\1.0\msys.bat"
SectionEnd



; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN

  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "Install MinGW + OpenTURNS deps (pthread headers & dlls, ...). Recquired to compile with OT."
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC02} "OpenTURNS checktests.$\r$\rStart them from OT's menu."

!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE

  ; Get previous installation mode
  !insertmacro CHECK_USER_INSTALL_FILE "$INSTDIR\UserInstallDev"

  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Do you want to remove $(^Name)?" /SD IDYES IDYES +2
  Abort
FunctionEnd


Section Uninstall
  ; nsis can't delete current directory
  SetOutPath $TEMP

  SetDetailsPrint textonly

  !insertmacro SET_MENU_CONTEXT

  ; do not delete if $INSTDIR not set
  !insertmacro PRINT "Delete $\"$INSTDIR\MinGW$\" directory."
  StrCmp "$INSTDIR" "" abort_install
  RMDir /r "$INSTDIR\MinGW"

  RMDir /r "$INSTDIR\include"

  Delete "$INSTDIR\examples\*.exe"
  Delete "$INSTDIR\examples\start_all_check_test.sh"
  Delete "$INSTDIR\UserInstallDev"

  Delete "$SMPROGRAMS\OpenTURNS\Uninstall-developer.lnk" 
  Delete "$SMPROGRAMS\OpenTURNS\start-checktests.lnk"
  Delete "$SMPROGRAMS\OpenTURNS\MSYS-Shell.lnk"

  Goto abort_install_end
  abort_install:
  MessageBox MB_OK|MB_ICONEXCLAMATION "INSTDIR not found. Installation aborted." /SD IDOK
  abort_install_end:

  SetAutoClose false
SectionEnd


BrandingText "OpenTURNS ${PRODUCT_VERSION}"
