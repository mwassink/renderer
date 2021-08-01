setlocal
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat
rem preprocess: call cl /P /C glwrangler.cc
call cl -FC -Zi entry.cc glwrangler.cc user32.lib gdi32.lib opengl32.lib
endlocal
