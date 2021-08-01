setlocal
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat
call dir
call cl /C glwrangler.cc 
endlocal