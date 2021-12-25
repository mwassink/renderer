This is a 3D renderer. Now, it does basic rendering of models along with some other things. It contains a demo to test models, which are not included in the source.
I am writing it to learn and use in a game.

It uses OpenGL 4.5, the C runtime library, and some platform specific functions for Windows.

Cool Features:
- Normal Mapping
- Spotlight Shadows
- Bounding Spheres (frustum culling)
  - Visible through ray tracing compute shader
- Skyboxes

## Build
Use the most recent release to try and run the program. To build, navigate to the build/ folder and run build-clang.bat or click on it from the file explorer. You would need Clang installed to do this. This will create an executable called demo-clang.exe that can run the program. Run it from the same directory. It can also be built with MSVC if the user has the correct tools, but I usually use that to debug the program.