@setlocal
clang++ -maes -msse4 -D _CRT_SECURE_NO_WARNINGS ..\entry.cpp ..\glwrangler.cpp ..\utilities.cpp ..\renderer.cpp -luser32 -lgdi32 -lopengl32 -O2 -o demo-clang.exe
endlocal
