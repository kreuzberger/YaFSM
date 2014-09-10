@echo off
rem adapt path to include tools requireed
rem set PATH=%PATH%; 

if "%1%" == "" (
  echo missing definition of fsm file
  goto end
) else (
 if not exist "%1%" (
   echo file %1% does not exist
   goto end
 )
)

set "YAFSM_PATH=%~dps0%"

rem xml.exe val -e -w -s %YAFSM_PATH%yafsm.xsd %1%

if %ERRORLEVEL% EQU 0 (
  echo generating fsm code for %1%
  perl -I %YAFSM_PATH% -f  %YAFSM_PATH%YaFsm.pl --fsm=%1% --gencode --genview
)

rem get short name
for %%i in (%1) do set BASENAME=%%~ni
rem echo %BASENAME%
echo copy source files from %YAFSM_PATH%codeimpl\cpp inc and qt4 to %BASENAME%\code
xcopy /D /Y  %YAFSM_PATH%codeimpl\cpp\inc\* %BASENAME%\code
xcopy /D /Y %YAFSM_PATH%codeimpl\cpp\qt4\* %BASENAME%\code

:end
