@echo off
setlocal enabledelayedexpansion

REM -- 设置文件目录，如果需要可以修改为特定路径
set "FOLDER_PATH=%~dp0OpenGL18_1"

REM -- 更改文件名
cd /d "%FOLDER_PATH%"
if exist OpenGL17_1.sln ren OpenGL17_1.sln OpenGL18_1.sln
if exist OpenGL17_1.vcxproj ren OpenGL17_1.vcxproj OpenGL18_1.vcxproj
if exist OpenGL17_1.vcxproj.filters ren OpenGL17_1.vcxproj.filters OpenGL18_1.vcxproj.filters
if exist OpenGL17_1.vcxproj.user ren OpenGL17_1.vcxproj.user OpenGL18_1.vcxproj.user

REM -- 替换文件内容
for %%f in (OpenGL18_1.sln, OpenGL18_1.vcxproj) do (
    if exist "%%f" (
        set "TEMP_FILE=%%f.temp"
        del "%TEMP_FILE%" 2>nul
        for /f "delims=" %%l in ('type "%%f" ^& break ^> "%%f"') do (
            set "LINE=%%l"
            set "MODIFIED_LINE=!LINE:OpenGL17_1=OpenGL18_1!"
            echo !MODIFIED_LINE!>>"%TEMP_FILE%"
        )
        move /y "%TEMP_FILE%" "%%f" >nul
    )
)

echo Files have been renamed and modified.
pause
