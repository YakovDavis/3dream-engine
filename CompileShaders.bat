@echo off

set current_dir=%cd%

if "%~1" NEQ "" set current_dir="%~1"

if exist "%current_dir%\libs\ShaderMake\bin\Debug\ShaderMake.exe" goto ContinueShaders

echo Building ShaderMake executable...

cmake -G "Visual Studio 17 2022" -S %current_dir%\libs\ShaderMake -B %current_dir%\libs\ShaderMake\build\Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build %current_dir%\libs\ShaderMake\build\Debug --config Debug

echo Done!

:ContinueShaders

echo Compiling shaders...

FOR /F "tokens=2*" %%A IN ('reg query "HKLM\SOFTWARE\WOW6432Node\Microsoft\Microsoft SDKs\Windows\v10.0" /v "InstallationFolder"') DO SET "WIN_SDK_PATH=%%B"

FOR /F "tokens=2*" %%A IN ('reg query "HKLM\SOFTWARE\WOW6432Node\Microsoft\Microsoft SDKs\Windows\v10.0" /v "ProductVersion"') DO SET "WIN_SDK_VER=%%B"

SET "WIN_SDK_BINARY_DIR=%WIN_SDK_PATH%bin\%WIN_SDK_VER%.0"

%current_dir%\libs\ShaderMake\bin\Debug\ShaderMake.exe -p DXIL --binaryBlob -c "%current_dir%\shaders\list.config" -o "%current_dir%\build\Debug\Shaders" --compiler "%WIN_SDK_BINARY_DIR%\x86\dxc.exe"

PAUSE