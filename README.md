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

## Current Demo
Currently, the demo from the release (v1) has several barrels floating in the air with a light source denoted by a green sphere. This should showcase the features that can be obtained (those up above), although the demo isn't good. Pressing 1 on the keyboard should toggle bounding volumes. 
<br>
<img src=samples/bounding.PNG width="338" height="150" /> <img src=samples/no_bounding.PNG width="338" height="150" />

*I should also have point shadows working... I just need to create a demo*
## Other Notes
- Working on more realistic features
- Learned many algorithms from Eric Lengyel's FGED 2 book. Methods for bounding sphere creation and mesh tangents were taken pretty closely from that book.
- Method for intrinsic hash function was learned from a series called Handmade Hero, and so was setting up an OpenGL context.
