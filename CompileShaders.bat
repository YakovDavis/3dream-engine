SET WIN_SDK_PATH=C:\Program Files (x86)\Windows Kits\10\bin\10.0.22621.0

%cd%\libs\ShaderMake\bin\Debug\ShaderMake.exe -p DXIL --binaryBlob -c "%cd%\src\render\shaders\list.config" -o "%cd%\build\Debug\Debug\Shaders" --compiler "%WIN_SDK_PATH%\x86\dxc.exe"

PAUSE