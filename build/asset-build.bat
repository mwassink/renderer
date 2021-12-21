@setlocal
clang++ -g -maes -msse4 -D ASSET_BUILD -D _CRT_SECURE_NO_WARNINGS ..\utilities.cpp ..\utilmain.cpp  -o assets.exe
endlocal
