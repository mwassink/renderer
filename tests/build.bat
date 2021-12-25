setlocal
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
cl -FC -Zi /Oi testvmath.cc test.cc
@if ERRORLEVEL == 0 (goto pass)
@if ERRORLEVEL != 0 (
    goto end
)
:pass
    call .\testvmath.exe
    call "C:\Program Files\Mozilla Firefox\firefox.exe" file:///C:/Users/MarkW/Documents/renderer/tests/default.html
    goto end
:end
endlocal
