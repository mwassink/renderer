@setlocal
clang++ -maes -msse4 -D _CRT_SECURE_NO_WARNINGS ..\entry.cc ..\glwrangler.cc ..\utilities.cc ..\renderer.cc -luser32 -lgdi32 -lopengl32 -O2 -o demo-clang.exe
endlocal
