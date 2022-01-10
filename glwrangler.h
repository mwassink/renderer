/* date = July 7th 2021 10:09 pm */

#ifndef GLWRANGLER_H
#define GLWRANGLER_H
#include <windows.h>
#include <gl/gl.h>
extern const int pixelAttribList[];
extern const int glAttribList[];

void wrangle(void);
void setPixelAttrs(HDC windowDC);

#define FAIL() *((volatile int*)0) = 0
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB  0x20A9
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023
#define WGL_ACCELERATION_ARB              0x2003
#define WGL_FULL_ACCELERATION_ARB         0x2027
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                 0x88E4
#define GL_VERTEX_SHADER               0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_COMPILE_STATUS                 0x8B81
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_TEXTURE_2D_ARRAY               0x8C1A
#define GL_RGBA32F                        0x8814
#define GL_LINK_STATUS                    0x8B82
#define GL_BGR                            0x80E0
#define GL_MIRRORED_REPEAT                0x8370
#define GL_CLAMP_TO_BORDER                0x812D
#define GL_FRAMEBUFFER                    0x8D40
#define GL_DEPTH_STENCIL_ATTACHMENT       0x821A
#define GL_DEPTH_COMPONENT32              0x81A7
#define GL_TEXTURE_COMPARE_MODE           0x884C
#define GL_TEXTURE_COMPARE_FUNC           0x884D
#define GL_COMPARE_REF_TO_TEXTURE         0x884E
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_SEAMLESS      0x884F
#define GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS 0x90EB
#define GL_MAX_COMPUTE_WORK_GROUP_COUNT   0x91BE
#define GL_MAX_COMPUTE_WORK_GROUP_SIZE    0x91BF
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 0x00000020
#define GL_WRITE_ONLY                     0x88B9
#define GL_COMPUTE_SHADER                 0x91B9
#define GL_TEXTURE_WRAP_R                 0x8072

typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;
typedef char GLchar;
typedef unsigned int GLbitfield;

typedef void WINAPI t_GenVertexArrays(GLsizei n, GLuint *arrays);
typedef void WINAPI t_BindVertexArray(GLuint array);
typedef void WINAPI t_GenBuffers(GLsizei  n,GLuint *  	buffers);
typedef void WINAPI t_BindBuffer(GLenum target, GLuint buffer);
typedef void WINAPI t_BufferData( GLenum target,GLsizeiptr size,const GLvoid * data,GLenum usage);
typedef GLuint WINAPI t_CreateShader(GLenum shaderType);
typedef void WINAPI t_ShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
typedef void WINAPI t_CompileShader(GLuint shader);
typedef GLuint WINAPI t_CreateProgram(void);
typedef void WINAPI t_AttachShader( GLuint program, GLuint shader);
typedef void WINAPI t_LinkProgram(GLuint program);
typedef void WINAPI t_VertexAttribPointer(GLuint  index, GLint  size, GLenum  type, GLboolean  normalized, GLsizei  stride, const GLvoid *  pointer);
typedef void WINAPI t_EnableVertexAttribArray(GLuint index);
typedef void WINAPI t_UseProgram(GLuint program);
typedef void WINAPI t_BindVertexArray(GLuint array);
// TODO does this have a iv tacked on at the end of it?
typedef void WINAPI t_GetShaderiv(GLuint shader, GLenum pname, GLint *params);
typedef BOOL WINAPI t_ChoosePixelFormatARB(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats); 

typedef HGLRC WINAPI t_CreateContextAttribsARB(HDC hDC, HGLRC hShareContext,
                                               const int *pixelAttribList);
typedef void WINAPI  t_GetShaderInfoLog(GLuint shader,GLsizei maxLength, GLsizei *length, GLchar *infoLog);

typedef void WINAPI t_CreateTextures( GLenum target, GLsizei n, GLuint *textures);
typedef void WINAPI t_TextureStorage2D(	GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void WINAPI t_TextureSubImage2D(	GLuint texture,
                                        GLint level,
                                        GLint xoffset,
                                        GLint yoffset,
                                        GLsizei width,
                                        GLsizei height,
                                        GLenum format,
                                        GLenum type,
                                        const void *pixels);

typedef void WINAPI t_TextureParameteri( GLenum target,
                                        GLenum pname,
                                        GLint param);
typedef void WINAPI t_GenerateTextureMipmap(GLuint texture);
typedef void WINAPI t_UniformMatrix4fv(	GLint location,
                                       GLsizei count,
                                       GLboolean transpose,
                                       const GLfloat *value);
typedef GLuint WINAPI t_GetUniformLocation(	GLuint program,
                                           const GLchar *name);

typedef void WINAPI t_GetProgramInfoLog(GLuint program,
                                        GLsizei maxLength,
                                        GLsizei *length,
                                        GLchar *infoLog);
typedef void WINAPI  t_GetProgramiv(	GLuint program,
                                    GLenum pname,
                                    GLint *params);

typedef void WINAPI t_UniformMatrix3fv(	GLint location,
                                       GLsizei count,
                                       GLboolean transpose,
                                       const GLfloat *value);
typedef void WINAPI t_Uniform3f(GLint location,
                                GLfloat v0,
                                GLfloat v1,
                                GLfloat v2);
typedef void WINAPI t_Uniform1f(GLint location, GLfloat v0);
typedef void WINAPI t_BindTextureUnit(GLuint unit, GLuint texture);
typedef void WINAPI t_GenFramebuffers(GLsizei n,GLuint* ids );
typedef void WINAPI t_BindFramebuffer(GLenum t, GLuint f);

typedef void WINAPI t_FramebufferTexture(	GLenum target,
                                         GLenum attachment,
                                         GLuint texture,
                                         GLint level);


typedef void WINAPI t_FramebufferTexture2D(	GLenum target,
                                           GLenum attachment,
                                           GLenum textarget,
                                           GLuint texture,
                                           GLint level);
typedef GLenum WINAPI t_CheckFramebufferStatus(	GLenum target);

typedef void WINAPI t_Uniform3fv(	GLint location,
                                 GLsizei count,
                                 const GLfloat *value);
typedef void WINAPI t_Uniform4fv(	GLint location,
                                 GLsizei count,
                                 const GLfloat *value);

typedef void WINAPI t_Uniform2fv(GLint location, GLsizei count, const GLfloat* value);

typedef void WINAPI t_GetIntegeri_v(GLenum target,
                                    GLuint index,
                                    GLint *data);


typedef void WINAPI t_DispatchCompute(GLuint num_groups_x,
                                      GLuint num_groups_y,
                                      GLuint num_groups_z);

typedef void WINAPI t_MemoryBarrier(GLbitfield barriers);


typedef void WINAPI t_BindImageTexture(	GLuint unit,
                                       GLuint texture,
                                       GLint level,
                                       GLboolean layered,
                                       GLint layer,
                                       GLenum access,
                                       GLenum format);
#if 0
typedef void WINAPI t_TexImage2D(	GLenum target,
                                 GLint level,
                                 GLint internalformat,
                                 GLsizei width,
                                 GLsizei height,
                                 GLint border,
                                 GLenum format,
                                 GLenum type,
                                 const void * data);




#endif



#define CONSTIFY(name) #name

#define MAKESTRING(name) const char* str_##name = CONSTIFY(gl##name) 
#define EXPORT(name) extern t_##name  *gl##name
#define DECLARE(name) t_##name  *gl##name 
#define WRANGLE(name) MAKESTRING(name); gl##name = (t_##name *)wglGetProcAddress(str_##name)



#define WMAKESTRING(name) const char* str_##name = CONSTIFY(wgl##name)
#define WEXPORT(name) extern t_##name  *wgl##name
#define WDECLARE(name) t_##name  *wgl##name
#define WWRANGLE(name) WMAKESTRING(name); wgl##name = (t_##name *)wglGetProcAddress(str_##name)


EXPORT( GenVertexArrays );
EXPORT( BindVertexArray);
EXPORT( GenBuffers);
EXPORT( BindBuffer);
EXPORT( BufferData);
EXPORT( CreateShader);
EXPORT( ShaderSource);
EXPORT( CompileShader);
EXPORT( CreateProgram);
EXPORT( AttachShader);
EXPORT( LinkProgram);
EXPORT( VertexAttribPointer);
EXPORT( EnableVertexAttribArray);
EXPORT( UseProgram);
EXPORT( GetShaderiv);
WEXPORT( ChoosePixelFormatARB);
WEXPORT( CreateContextAttribsARB);
EXPORT( GetShaderInfoLog);
EXPORT( CreateTextures);
EXPORT( TextureStorage2D);
EXPORT( TextureSubImage2D);
EXPORT( TextureParameteri);
EXPORT( GenerateTextureMipmap);
EXPORT( UniformMatrix4fv);
EXPORT( GetUniformLocation);
EXPORT( GetProgramInfoLog);
EXPORT( GetProgramiv);
EXPORT( UniformMatrix3fv);
EXPORT( Uniform3f);
EXPORT( Uniform1f);
EXPORT( BindTextureUnit);
EXPORT(GenFramebuffers);
EXPORT(BindFramebuffer);
EXPORT(FramebufferTexture);
EXPORT(FramebufferTexture2D);
EXPORT(CheckFramebufferStatus);
EXPORT(Uniform3fv);
EXPORT(Uniform4fv);
EXPORT(Uniform2fv);
EXPORT(GetIntegeri_v);
EXPORT(DispatchCompute);
EXPORT(MemoryBarrier);
EXPORT(BindImageTexture);


void setPixelAttrs(HDC windowDC);
void wrangleSetup(void);
void wrangle(void);
void initialPixelAttrs(HDC winDC);
void loadInitialContext(void);

#endif //GLWRANGLER_H
