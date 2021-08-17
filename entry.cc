// This will serve as the place for the opengl layer... hopefully
#include <windows.h>
#include <gl/gl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "glwrangler.h"
#include "entry.h" // TODO move this somewhere else
// Notes from Handmade hero: 
// Need to link with opengl32.lib
// this gives the ability to call the operating system functions that we have
// the platform specific part needs to be set up here, like wglMakeCurrent
// Device context is the state of the graphics at a specific time, stores mode stuff
// opengl has an "rc", which sits on top of the device context to the user, THEN can do the opengl calls but with a caveat
// the whole state of the gl is implicit to your THREAD
// any time a thread opens an gl call it needs to have an rc associated with it, which is done with wglMakeCurrent
// the make current is not enough. because of leagcyish reasons we need to ask Windows choose the pixel formats
//wglGetProcAddress will allow you to load any openGL function
// then a typedef and a #define will allow for different functions to be used and loaded
typedef float f32;
static int64_t ctr = 0;
typedef uint8_t uint8;
static bool runnable = 1;
static HBITMAP bitMapHandle; // initialized to 0
static HDC compatibleContext;
static   BITMAPINFO bitMapInfo;
static void* bitMapMemory;
static int bitMapHeight;
static int bitMapWidth;
// this should give us a buffer that we can draw into

static void initOpenGL(HWND Window ) {
    HDC WindowDC = GetDC(Window);
    setPixelAttrs(WindowDC); // it can do this because we have ALREADY MADE A CONTEXT    
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


void gltest(void) {
    
    float vertices[] = { -.5f, -.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };  
    
    unsigned int VBO, vertexShader, fragmentShader, VAO, shaderProgram = 0;
    int success = 0;
    char log[512]; // is this long enough?
    const char* glVertexSrc = 0; const char* glPixelShaderSrc = 0;
    glGenVertexArrays(1, &VAO);  
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // should return a char* ptr to the 
    glVertexSrc = readFile("test_vertex.glsl");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &glVertexSrc, NULL);
    glCompileShader(vertexShader);
    checkFailure(vertexShader);
    
    glPixelShaderSrc = readFile("test_pixel.glsl");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &glPixelShaderSrc, NULL);
    glCompileShader(fragmentShader);
    checkFailure(fragmentShader);
    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkFailure(shaderProgram);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  
    
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    
    
    
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    
    
    
    
}



char* readFile(const char* name) {
    FILE* fp = fopen(name, "r");
    int size;
    char* buf = 0;
    if (!fp) 
        return 0;
    
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    buf = (char*)malloc(size);
    fseek(fp, 0, SEEK_SET);
    fread(buf, 1, size, fp);
    fclose(fp);
    
    
    return buf;
    
}


void checkFailure(int shader) {
    
    int success = 0;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    if(!success ){
        glGetShaderInfoLog(shader, 512, NULL, log);
        FILE* fp = fopen("error.log", "w");
        fwrite( log, 1, 512, fp);
        fclose(fp);
    }
}

static void ResizeDIBSection(int width, int height, HWND wind) {
    
    
    HDC windowDC = GetDC(wind);
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f , 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    SwapBuffers(windowDC);
    ReleaseDC(wind, windowDC);
    // set the opengl viewport to this
    
}

void callOpenGL(HWND wind, f32 r, f32 g, f32 b) {
    HDC windowDC = GetDC(wind);
    glViewport(0, 0, 1920, 1080);
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    SwapBuffers(windowDC);
    ReleaseDC(wind, windowDC);
}

// we are going to call a function here
// this creates a dib section that we can write to directly somewhere in here
// takes one window, puts it onto the otherk window, and stretches it along the way
static void UpdateWindow(HDC deviceContext, RECT* windowRect, int x, int y, int width, int height)
{
    int windowWidth = windowRect->right - windowRect->left;
    int windowHeight = windowRect->bottom - windowRect->top;
    
    StretchDIBits(
        deviceContext,
        0, 0, bitMapWidth, bitMapHeight,
        0, 0, windowWidth, windowHeight,
        bitMapMemory,
        &bitMapInfo,
        DIB_RGB_COLORS,
        SRCCOPY
        );
    
    
}

// switch statement over the possible messages that get passed in to this
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
        
    case WM_ACTIVATEAPP:
    {
        // (TODO) something here
        OutputDebugStringA("activate app\n");
            
    } break;
        
    case WM_DESTROY:
    {
        runnable = 0;
        OutputDebugStringA("wm destroy\n");
    } break;
    // this is not the only place where we do the paintinbg, but it is one of them
    case WM_PAINT:
    {
        PAINTSTRUCT paint;
        HDC deviceContext = BeginPaint(window, &paint);
        int x = paint.rcPaint.left;
        int y = paint.rcPaint.top;
        int width = paint.rcPaint.right - paint.rcPaint.left;
        int height = paint.rcPaint.bottom - paint.rcPaint.top;
            
            
            
            
        RECT clientRect;
        GetClientRect(window, &clientRect);
        //windows wants us to upate the window when we do this
        UpdateWindow(deviceContext, &clientRect,  x, y, width, height);
            
            
            
            
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
    WindowClass.lpszClassName = "My class name";
    
    if (RegisterClass(&WindowClass)) {
        
        HWND windowHandle = CreateWindowExA (
            0,
            "My class name",
            "hello",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            hInstance,
            0);
        
        
        if (windowHandle) {
            
            initOpenGL(windowHandle);
            for (; runnable; ){
                
                MSG msg;
                BOOL msg_res = PeekMessage(&msg, windowHandle, 0, 1000, PM_REMOVE);
                if (msg_res > 0) {
                    
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                    
                    
                }
                else {
                    //break;
                    
                }
                

                
                gltest();
                HDC windowDC = GetDC(windowHandle);
                SwapBuffers(windowDC);
                
                Sleep(1000);                
                
                
            }
            
        }
        
        
        return (0);
    }

    return (0);
    
}







