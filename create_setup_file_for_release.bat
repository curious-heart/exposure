@echo off

echo 开始创建安装文件...
set C_LOG=create.log
call :create_setup_file > %C_LOG%
echo 创建完成，请查看%C_LOG%文件确认结果。
pause
exit

:create_setup_file
set QT_INSTALL_PREFIX=D:\01.Prog\Qt5.15\5.15.2\mingw81_64
set qt_deploy_script_dir=.\setup
set temp_files_dir=packages\com.tester.root\data
set temp_cp_dest_dir=%qt_deploy_script_dir%\%temp_files_dir%
set src_code_dir=.\tarsier
set exe_file_dir=.\build-tarsier-Desktop_Qt_5_15_2_MinGW_64_bit-Release\release
set exe_main_name=tarsier
REM clear destination dir firstly.
@del %temp_cp_dest_dir%\* /S /Q
for /f "delims=" %%a in ('dir /ad/b %temp_cp_dest_dir%') do (rd /q /s "%temp_cp_dest_dir%\%%a")

REM copy files to the temp working folder.
REM copy main exe file.
xcopy %exe_file_dir%\%exe_main_name%.exe %temp_cp_dest_dir% /I /Y
REM copy data folder, which contains config files.
xcopy %src_code_dir%\data %temp_cp_dest_dir%\data /I /E /Y
REM copy work_dir for fpd.
xcopy %src_code_dir%\work_dir %temp_cp_dest_dir%\work_dir /I /E /Y /EXCLUDE:.exclude_copy_files
REM copy iRay detector related files (in iDetector folder)
xcopy .\02.iDetector\*.dll %temp_cp_dest_dir% /I /Y
xcopy .\02.iDetector\*.txt %temp_cp_dest_dir% /I /Y
xcopy .\02.iDetector\*.lic %temp_cp_dest_dir% /I /Y
xcopy .\02.iDetector\*.xml %temp_cp_dest_dir% /I /Y
xcopy .\02.iDetector\DetectorService.exe %temp_cp_dest_dir% /I /Y
xcopy .\02.iDetector\CountryChannels.db %temp_cp_dest_dir% /I /Y
REM PZM detector files has already been staiclly put into source files
xcopy .\%src_code_dir%\pzm\sdk_4.1.16\*.ini %temp_cp_dest_dir%\pzm\sdk_4.1.16 /I /Y
xcopy .\%src_code_dir%\pzm\sdk_4.1.16\ComApi\win32 %temp_cp_dest_dir%\pzm\sdk_4.1.16\ComApi\win32 /I /E /Y
xcopy .\%src_code_dir%\pzm\sdk_4.1.16\ComApi\x64 %temp_cp_dest_dir%\pzm\sdk_4.1.16\ComApi\x64 /I /E /Y
REM copy virtualkeyboard related files.
xcopy %QT_INSTALL_PREFIX%\bin\Qt5Qml.dll %temp_cp_dest_dir% /I /Y
xcopy %QT_INSTALL_PREFIX%\bin\Qt5QmlModels.dll %temp_cp_dest_dir% /I /Y
xcopy %QT_INSTALL_PREFIX%\bin\Qt5QmlWorkerScript.dll %temp_cp_dest_dir% /I /Y
xcopy %QT_INSTALL_PREFIX%\bin\Qt5Quick.dll %temp_cp_dest_dir% /I /Y
xcopy %QT_INSTALL_PREFIX%\bin\Qt5VirtualKeyboard.dll %temp_cp_dest_dir% /I /Y
xcopy %QT_INSTALL_PREFIX%\plugins\platforminputcontexts\qtvirtualkeyboardplugin.dll %temp_cp_dest_dir%\platforminputcontexts\ /I /Y
xcopy %QT_INSTALL_PREFIX%\plugins\virtualkeyboard\qtvirtualkeyboard_hangul.dll %temp_cp_dest_dir%\virtualkeyboard\ /I /Y
xcopy %QT_INSTALL_PREFIX%\plugins\virtualkeyboard\qtvirtualkeyboard_openwnn.dll %temp_cp_dest_dir%\virtualkeyboard\ /I /Y
xcopy %QT_INSTALL_PREFIX%\plugins\virtualkeyboard\qtvirtualkeyboard_pinyin.dll %temp_cp_dest_dir%\virtualkeyboard\ /I /Y
xcopy %QT_INSTALL_PREFIX%\plugins\virtualkeyboard\qtvirtualkeyboard_tcime.dll %temp_cp_dest_dir%\virtualkeyboard\ /I /Y
xcopy %QT_INSTALL_PREFIX%\plugins\virtualkeyboard\qtvirtualkeyboard_thai.dll %temp_cp_dest_dir%\virtualkeyboard\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\Qt\labs\folderlistmodel\plugins.qmltypes %temp_cp_dest_dir%\qml\Qt\labs\folderlistmodel\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\Qt\labs\folderlistmodel\qmldir %temp_cp_dest_dir%\qml\Qt\labs\folderlistmodel\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\Qt\labs\folderlistmodel\qmlfolderlistmodelplugin.dll %temp_cp_dest_dir%\qml\Qt\labs\folderlistmodel\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQml\plugins.qmltypes %temp_cp_dest_dir%\qml\QtQml\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQml\qmldir %temp_cp_dest_dir%\qml\QtQml\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQml\qmlplugin.dll %temp_cp_dest_dir%\qml\QtQml\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick\Layouts\plugins.qmltypes %temp_cp_dest_dir%\qml\QtQuick\Layouts\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick\Layouts\qmldir %temp_cp_dest_dir%\qml\QtQuick\Layouts\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick\Layouts\qquicklayoutsplugin.dll %temp_cp_dest_dir%\qml\QtQuick\Layouts\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick\VirtualKeyboard\plugins.qmltypes %temp_cp_dest_dir%\qml\QtQuick\VirtualKeyboard\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick\VirtualKeyboard\qmldir %temp_cp_dest_dir%\qml\QtQuick\VirtualKeyboard\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick\VirtualKeyboard\qtquickvirtualkeyboardplugin.dll %temp_cp_dest_dir%\qml\QtQuick\VirtualKeyboard\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick\VirtualKeyboard\Settings\plugins.qmltypes %temp_cp_dest_dir%\qml\QtQuick\VirtualKeyboard\Settings\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick\VirtualKeyboard\Settings\qmldir %temp_cp_dest_dir%\qml\QtQuick\VirtualKeyboard\Settings\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick\VirtualKeyboard\Settings\qtquickvirtualkeyboardsettingsplugin.dll %temp_cp_dest_dir%\qml\QtQuick\VirtualKeyboard\Settings\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick\VirtualKeyboard\Styles\plugins.qmltypes %temp_cp_dest_dir%\qml\QtQuick\VirtualKeyboard\Styles\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick\VirtualKeyboard\Styles\qmldir %temp_cp_dest_dir%\qml\QtQuick\VirtualKeyboard\Styles\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick\VirtualKeyboard\Styles\qtquickvirtualkeyboardstylesplugin.dll %temp_cp_dest_dir%\qml\QtQuick\VirtualKeyboard\Styles\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick\Window.2\plugins.qmltypes %temp_cp_dest_dir%\qml\QtQuick\Window.2\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick\Window.2\qmldir %temp_cp_dest_dir%\qml\QtQuick\Window.2\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick\Window.2\windowplugin.dll %temp_cp_dest_dir%\qml\QtQuick\Window.2\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick.2\plugins.qmltypes %temp_cp_dest_dir%\qml\QtQuick.2\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick.2\qmldir %temp_cp_dest_dir%\qml\QtQuick.2\ /I /Y
xcopy %QT_INSTALL_PREFIX%\qml\QtQuick.2\qtquick2plugin.dll %temp_cp_dest_dir%\qml\QtQuick.2\ /I /Y
REM copy nginx
xcopy .\%src_code_dir%\nginx-1.18.0 %temp_cp_dest_dir%\nginx-1.18.0 /I /E /Y /EXCLUDE:.exclude_copy_files
REM copy other related files
xcopy .\%src_code_dir%\otafile %temp_cp_dest_dir%\otafile /I /E /Y
xcopy .\%src_code_dir%\plugin %temp_cp_dest_dir%\plugin /I /E /Y
xcopy .\%src_code_dir%\res %temp_cp_dest_dir%\res /I /E /Y
xcopy .\%src_code_dir%\Mysql.ini %temp_cp_dest_dir%\ /I /Y
xcopy .\%src_code_dir%\StephenConfig.ini %temp_cp_dest_dir%\ /I /Y
REM copy files for debug.
xcopy .\%src_code_dir%\initTool.exe %temp_cp_dest_dir%\ /I /Y
xcopy .\%src_code_dir%\串口调试.mbp %temp_cp_dest_dir%\ /I /Y

REM deploy qt dependencies
set qt_deploy_cmd=deploy.bat
set create_setup_cmd=create.bat
set result_setup_file=Setup.exe
cd %qt_deploy_script_dir%
call %qt_deploy_cmd%
REM create setup file.
echo 正在创建安装文件，请稍后...
call %create_setup_cmd%

REM clear temp files.
echo -------------------- 
echo 清除临时文件和文件夹
@del %temp_files_dir%\* /S /Q
for /f "delims=" %%a in ('dir /ad/b %temp_files_dir%') do (rd /q /s "%temp_files_dir%\%%a")
REM restore the .gitkeep file to avoid git message.
cd . > %temp_files_dir%\.gitkeep
echo 安装文件创建完成：%qt_deploy_script_dir%\%result_setup_file%

echo -------------------- 
echo 提取版本号字符串...
REM rename result file: add version number.
set version_file_name=version_def.cpp
set version_var_name="APP_VER_STR"
for /F "delims=" %%l in ('findstr %version_var_name% ..\%src_code_dir%\%version_file_name%') do (set version_str_line=%%l)
REM extract the version string
set mark_c=#
REM the original version_str_line from .cpp is like this: const char* APP_VER_STR = "V2.2.3.1_adding_PZM_support";
REM firstly we replace the " with mark_c to avoid syntax error in later commands.
setlocal enabledelayedexpansion
set version_str_line=!version_str_line:"=%mark_c%!
:remove_prefix
if not "%version_str_line%" == "" (
REM 比较首字符是否为要求的字符，如果是则跳出循环
if "%version_str_line:~0,1%" == "%mark_c%" goto remove_postfix
set version_str_line=%version_str_line:~1%
goto remove_prefix
)
:remove_postfix
if not "%version_str_line%" == "" (
REM 比较尾字符是否为要求的字符，如果是则跳出循环
if "%version_str_line:~-1%" == "%mark_c%" goto remove_quotes
set version_str_line=%version_str_line:~0,-1%
goto remove_postfix
)
:remove_quotes
if "%version_str_line:~0,1%" == "%mark_c%" set version_str_line=%version_str_line:~1%
if "%version_str_line:~-1%" == "%mark_c%" set version_str_line=%version_str_line:~0,-1%
REM make a dir with the name of version str, rename the result (adding ver) and move it to this dir.
md ..\%version_str_line%
set result_file_with_ver=%result_setup_file:~0,-4%
set result_file_with_ver=%result_file_with_ver%_%version_str_line%.exe
move /Y %result_setup_file% ..\%version_str_line%\%result_file_with_ver% 
echo 安装文件添加版本号，并移动到如下位置：%version_str_line%\%result_file_with_ver%
