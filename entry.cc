// This will serve as the place for the opengl layer... hopefully
#include <windows.h>
#include <gl/gl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "glwrangler.h"
#include "utilities.h" 
#include "types.h"
#include "vecmath.h"



static bool runnable = 1;


#define RUNTESTS 1
// this should give us a buffer that we can draw into

static void initOpenGL(HWND Window ) {
    HDC WindowDC = GetDC(Window);
    setPixelAttrs(WindowDC); // it can do this because we have already made a context    
    HGLRC OpenGLRC = 0;
    if (wglCreateContextAttribsARB) {
        OpenGLRC = wglCreateContextAttribsARB(WindowDC, 0, glAttribList); //can we get a modern context
    }
    if (!OpenGLRC) {
        FAIL();
    }
    
    if (wglMakeCurrent(WindowDC, OpenGLRC)) {
        wrangle();
    }
    else {
        FAIL();
    }
    ReleaseDC(Window, WindowDC);   
}

#if RUNTESTS
//#include "tests/gltest2.cc"
#include "tests/gltest3.cc"

#endif

static void ResizeDIBSection(int width, int height, HWND wind) {
    HDC windowDC = GetDC(wind);
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f , 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    SwapBuffers(windowDC);
    ReleaseDC(wind, windowDC);
}

LRESULT CALLBACK MainCallback(HWND window,
                              UINT msg,
                              WPARAM wparam,
                              LPARAM lparam)
{
    
    
    LRESULT result = 0;
    switch (msg) {
        
    case WM_SIZE:
    {
        RECT newRect;
        GetClientRect(window, &newRect);
        int width = newRect.right - newRect.left;
        int height = newRect.bottom - newRect.top;
        ResizeDIBSection(width, height, window);
        OutputDebugStringA("wm size\n");
    } break;
        
    case WM_CLOSE:
    {
        runnable = 0;
        OutputDebugStringA("wm close\n");
    } break;
    case WM_DESTROY:
    {
        runnable = 0;
        OutputDebugStringA("wm destroy\n");
    } break;
    case WM_PAINT:
    {

        PAINTSTRUCT paint;
        HDC deviceContext = BeginPaint(window, &paint);
        EndPaint(window, &paint);

    } break;
        
    default:
    {
        result = DefWindowProc(window, msg, wparam, lparam);
    }
    }
    return (result);
}


int CALLBACK WinMain(HINSTANCE hInstance,
                     HINSTANCE prevInstance,
                     LPSTR commandLine,
                     int showCode)

{
    loadInitialContext();
    WNDCLASS  WindowClass = {};
    WindowClass.lpfnWndProc = MainCallback;
    WindowClass.hInstance = hInstance;
    WindowClass.lpszClassName = "Renderer";
    bool ran = false;
    Mesh mesh;
    if (RegisterClass(&WindowClass)) {
        HWND windowHandle = CreateWindowExA (0, "Renderer", "Renderer Test", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, hInstance, 0);
        if (windowHandle) {

            initOpenGL(windowHandle);
            
            for (; runnable; ) {

                MSG msg;
                BOOL msg_res = PeekMessage(&msg, windowHandle, 0, 1000, PM_REMOVE);
                if (msg_res > 0) {

                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                if (!ran) {
#if RUNTESTS
                    mesh = gltest3();
#endif
                    HDC windowDC = GetDC(windowHandle);
                    SwapBuffers(windowDC);
                }
                doDraw(mesh);
                HDC windowDC = GetDC(windowHandle);
                SwapBuffers(windowDC);
                
                ran = true;
            }
        }
        return (0);
    }
    return (0);
}
