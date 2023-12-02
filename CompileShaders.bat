@echo off

if exist "%cd%\libs\ShaderMake\bin\Debug\ShaderMake.exe" goto ContinueShaders

echo Building ShaderMake executable...

cmake -G "Visual Studio 17 2022" -S %cd%\libs\ShaderMake -B %cd%\libs\ShaderMake\build\Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build %cd%\libs\ShaderMake\build\Debug --config Debug

echo Done!

:ContinueShaders

echo Compiling shaders...

FOR /F "tokens=2*" %%A IN ('reg query "HKLM\SOFTWARE\WOW6432Node\Microsoft\Microsoft SDKs\Windows\v10.0" /v "InstallationFolder"') DO SET "WIN_SDK_PATH=%%B"

FOR /F "tokens=2*" %%A IN ('reg query "HKLM\SOFTWARE\WOW6432Node\Microsoft\Microsoft SDKs\Windows\v10.0" /v "ProductVersion"') DO SET "WIN_SDK_VER=%%B"

SET "WIN_SDK_BINARY_DIR=%WIN_SDK_PATH%bin\%WIN_SDK_VER%.0"

%cd%\libs\ShaderMake\bin\Debug\ShaderMake.exe -p DXIL --binaryBlob -c "%cd%\shaders\list.config" -o "%cd%\build\Debug\Shaders" --compiler "%WIN_SDK_BINARY_DIR%\x86\dxc.exe"

PAUSE