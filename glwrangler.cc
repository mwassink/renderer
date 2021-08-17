// do opengl 4.5 as it was the last minor release of OpenGL
#include <windows.h>
#include <gl/gl.h>
#include "glwrangler.h"




DECLARE( GenVertexArrays );
DECLARE( BindVertexArray);
DECLARE( GenBuffers);
DECLARE( BindBuffer);
DECLARE( BufferData);
DECLARE( CreateShader);
DECLARE( ShaderSource);
DECLARE( CompileShader);
DECLARE( CreateProgram);
DECLARE( AttachShader);
DECLARE( LinkProgram);
DECLARE( VertexAttribPointer);
DECLARE( EnableVertexAttribArray);
DECLARE( UseProgram);
DECLARE( GetShaderiv);
WDECLARE( ChoosePixelFormatARB);
WDECLARE( CreateContextAttribsARB);
DECLARE( GetShaderInfoLog);

const int pixelAttribList[] =
{
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    WGL_COLOR_BITS_ARB, 32,
    WGL_DEPTH_BITS_ARB, 24,
    WGL_STENCIL_BITS_ARB, 8,
    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
    0, // End
};

const int glAttribList[] = {
    
    WGL_CONTEXT_MAJOR_VERSION_ARB, 4, 
    WGL_CONTEXT_MINOR_VERSION_ARB, 5,
    0
        
};


void loadInitialContext(void) {
    
    WNDCLASSA setup = {};
    setup.lpfnWndProc = DefWindowProcA;
    setup.hInstance = GetModuleHandle(0);
    setup.lpszClassName = "Test class name";
    
    if (RegisterClassA(&setup)) {
        
        HWND windowHandle = CreateWindowExA(0, 
                                            setup.lpszClassName,
                                            "Window",
                                            0,
                                            CW_USEDEFAULT,
                                            CW_USEDEFAULT,
                                            CW_USEDEFAULT,
                                            CW_USEDEFAULT,
                                            0,
                                            0,
                                            setup.hInstance,
                                            0);
        
        HDC winDC = GetDC(windowHandle);
        
        initialPixelAttrs(winDC);
        HGLRC openGLRC = wglCreateContext(winDC);
        if (wglMakeCurrent(winDC, openGLRC)) {
            WWRANGLE(ChoosePixelFormatARB);
            WWRANGLE(CreateContextAttribsARB);
            wglMakeCurrent(0, 0);
        }
        wglDeleteContext(openGLRC);
        ReleaseDC(windowHandle, winDC);
        DestroyWindow(windowHandle);
        
    }
    
}

// these are the ones that we get initially
void initialPixelAttrs(HDC winDC) {
    
    int suggestedIndex = 0;
    GLuint ExtendedPick = 0;
    
    
    PIXELFORMATDESCRIPTOR desired = {};
    desired.nSize = sizeof(desired);
    desired.nVersion = 1;
    desired.iPixelType = PFD_TYPE_RGBA;
    desired.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    desired.cColorBits = 32;
    
    desired.cAlphaBits = 8;
    desired.cDepthBits = 24;
    desired.iLayerType = PFD_MAIN_PLANE;
    suggestedIndex = ChoosePixelFormat(winDC, &desired);
    
    PIXELFORMATDESCRIPTOR suggested = {};
    DescribePixelFormat(winDC, suggestedIndex, sizeof(suggested), &suggested);
    SetPixelFormat(winDC, suggestedIndex, &suggested);
    
    
    
}

void setPixelAttrs(HDC windowDC) {
    
    int suggested = 0;
    GLuint nnumFormats = 0;
    
    wglChoosePixelFormatARB(windowDC, pixelAttribList, 0, 1, &suggested, &nnumFormats);
    
    
    if (nnumFormats == 0) {
        return;
        FAIL();
    }
    PIXELFORMATDESCRIPTOR pixelFormat;
    DescribePixelFormat(windowDC, suggested,
                        sizeof(pixelFormat), &pixelFormat);
    
    SetPixelFormat(windowDC, suggested, &pixelFormat);
}

void wrangleSetup(void) {
    WWRANGLE(ChoosePixelFormatARB);
    WWRANGLE(CreateContextAttribsARB);
}

void wrangle(void) {
    
    WRANGLE(GenVertexArrays);
    WRANGLE(BindVertexArray);
    WRANGLE(GenBuffers);
    WRANGLE(BindBuffer);
    WRANGLE(BufferData);
    WRANGLE(CreateShader);
    WRANGLE(ShaderSource);
    WRANGLE(CompileShader);
    WRANGLE(CreateProgram);
    WRANGLE(AttachShader);
    WRANGLE(LinkProgram);
    WRANGLE(VertexAttribPointer);
    WRANGLE(EnableVertexAttribArray);
    WRANGLE(UseProgram);
    WRANGLE(GetShaderiv);    
    WRANGLE( GetShaderInfoLog);
    
    
    
}
