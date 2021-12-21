@setlocal
@..\..\clocc.exe ..
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat
call cl -FC -Zi ..\entry.cpp ..\glwrangler.cpp ..\utilities.cpp ..\renderer.cpp user32.lib gdi32.lib opengl32.lib
endlocal
