@setlocal
clang++ -g -maes -msse4 -D ASSET_BUILD -D _CRT_SECURE_NO_WARNINGS ..\utilities.cc ..\utilmain.cc  -o assets.exe
endlocal
