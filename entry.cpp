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
#include "renderer.h"


static bool runnable = 1;
static bool boundingVolumesVisible = 0;


#define RUNTESTS 1
#if RUNTESTS
#include "tests.h"
#endif


void fatalError(const char* msg, const char* title) {
    MessageBox(0, msg, title,  MB_OK | MB_ICONERROR );
    ExitProcess(1);
}


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
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    ReleaseDC(Window, WindowDC);
}



static void ResizeDIBSection(int width, int height, HWND wind) {
    HDC windowDC = GetDC(wind);
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f , 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    SwapBuffers(windowDC);
    ReleaseDC(wind, windowDC);
}

void clear(HWND wind, HDC windowDC) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glFlush();
    ReleaseDC(wind, windowDC);
}

void orientCameraFromInput(UINT msg, WPARAM wparam, CoordinateSpace* cameraSpace) {
    
    switch (msg) {
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYUP:
        case WM_KEYDOWN:
        {
            u32 code = (u32)wparam;
            switch (code) {
                case 'W': {
                    cameraSpace->origin = cameraSpace->origin - cameraSpace->t;
                } break;
                case 'S': {
                    cameraSpace->origin = cameraSpace->origin + cameraSpace->t;
                } break;
                case 'A': {
                    cameraSpace->origin = cameraSpace->origin - cameraSpace->r;
                } break;
                case 'D': {
                    cameraSpace->origin = cameraSpace->origin + cameraSpace->r;
                } break;
                // Rotation about the x axis
                case VK_UP: {
                    Matrix3 m = rotateX3(-3.14f/80.0f);
                    cameraSpace->rotate(m);  
                } break;
                case VK_LEFT: {
                    Matrix3 m = rotateY3(-3.14/80.0f);
                    cameraSpace->rotate(m);
                } break;
                case VK_DOWN: {
                    Matrix3 m = rotateX3(3.14f/80.0f);
                    cameraSpace->rotate(m);
                } break;
                case VK_RIGHT: {
                    Matrix3 m = rotateY3(3.14/80.0f);
                    cameraSpace->rotate(m);
                } break;
                case '1': {
                    if (msg == WM_KEYUP || msg == WM_SYSKEYUP ){
                        boundingVolumesVisible = !boundingVolumesVisible;
                    }
                    
                } break;
                
                
            }
        } break;
    }
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
    Array<Model> models;
    Array<SpotLight> spotLights;
    Array<PointLight> pointLights;
    Model p;
    int64_t frames = 0;
    HDC windowDC;
    Model lightOne;
    if (RegisterClass(&WindowClass)) {
        HWND windowHandle = CreateWindowExA (0, "Renderer", "Renderer Test", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT,
                                             CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, hInstance, 0);
        if (windowHandle) {
            
            initOpenGL(windowHandle);
            Renderer renderer;
            
            renderer.context.windowHandle = windowHandle;
            for (; runnable; ) {
                if (!ran) {
                    windowDC = GetDC(windowHandle);
                }
                MSG msg = {};
                BOOL msg_res = PeekMessage(&msg, windowHandle, 0, 0, PM_REMOVE);
                if (msg_res > 0) {
                    
                    if (msg.message ==  WM_SYSKEYDOWN || msg.message == WM_SYSKEYUP ||
                        msg.message ==  WM_KEYDOWN || msg.message == WM_KEYUP) {
#if RUNTESTS
                        orientCameraFromInput(msg.message, msg.wParam, &renderer.context.cameraSpace);
#else
                        orientCameraFromInput(msg.message, msg.wParam, 0);
#endif
                    }
                    else {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }
                }
#if RUNTESTS
                testUtil = &renderer.utilHelper;
                shadowRenderer = &renderer;
                shadowRendererUtil = &renderer.utilHelper;
                Skybox box;
                
                if (!ran) {
                    SpotLight s;
                    s.worldSpaceCoord = Vector3(0, 5, -60);
                    s.color = Vector3(1,1,1);
                    s.irradiance = 50.0f;
                    s.lightSpace.r = Vector3(1,0,0);
                    s.lightSpace.s = Vector3(0, 0, -1);
                    s.lightSpace.t = Vector3(0, 1, 0);
                    s.lightSpace.origin = Vector3(0, 5, -60);
#if 1
                    populateModels(&models);
                    
                    models.push(barrel());
#endif
                    pointLights.push(pl());
                    models.push(car(0, -25, -60));
                    spotLights.push(s);
                    // r, l, t, b, f, b
                    const char* px = "../tests/resources/SunnyDay_px.bmp";
                    const char* nx = "../tests/resources/SunnyDay_nx.bmp";
                    const char* py = "../tests/resources/SunnyDay_py.bmp";
                    const char* ny = "../tests/resources/SunnyDay_ny.bmp";
                    const char* pz = "../tests/resources/SunnyDay_pz.bmp";
                    const char* nz = "../tests/resources/SunnyDay_nz.bmp";
                    
                    
                    const char* files[6] = {px, nx, py, ny, pz, nz};
                    box = renderer.MakeSkybox(files);
                    renderer.context.cameraSpace = lookAtCoordSpace(models[0].modelSpace.origin, renderer.context.cameraSpace.origin);
                    lightOne = renderer.CreateLightModel(&spotLights[0], 0.3f);
                    InitialPointDemoSetup(&renderer);

                }
                
                spotLights[0].lightSpace = lookAtCoordSpace(models[0].modelSpace.origin, spotLights[0].lightSpace.origin);
                testShadow(&models, &spotLights[0]);
                Vector3 yellow = Vector3(1.0f, 1.0f, 0.0f);
                renderer.RenderSkybox(box);
                renderer.DrawBoundingSphere(&lightOne);
                TestPointShadow(&renderer);
                

                if (boundingVolumesVisible) {
                    for (int i = 0; i < models.sz; i++) {
                        renderer.DrawBoundingSphere(&models[i]);
                    }
                }


#endif
                SwapBuffers(windowDC);
                
                ran = true;

                frames++;
            }
        }
        return (0);
    }
    return (0);
}
