//
//  ESUtil.h
//  MyOpenGLES
//
//  Created by 姚隽楠 on 2021/9/6.
//  Copyright © 2021 姚隽楠. All rights reserved.
//

#ifndef ESUtil_h
#define ESUtil_h

#include <stdlib.h>

#ifdef __APPLE__
#include <OpenGLES/ES3/gl.h>
#else
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif
#ifdef __cplusplus

extern "C"{
#endif

// Macros
#ifdef WIN32
#define ESUTIL_API __cdecl
#define ESCALLBACK __cdecl
#else
#define ESUTIL_API
#define ESCALLBACK
#endif

// esCreateWindow flag - RGB color buffer
#define ES_WINDOW_RGB        0
// esCreateWindow flag - ALPHA color buffer
#define ES_WINDOW_ALPHA      1
// esCreateWindow flag - depth buffer
#define ES_WINDOW_DEPTH      2
// esCreateWindow flag - stencil buffer
#define ES_WINDOW_STENCIL    4
// esCreateWindow flag - multi-sample buffer
#define ES_WINDOW_MUTISAMPLE 8

// Types
#ifdef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

typedef struct
{
    GLfloat m[4][4];
} ESMatrix;

typedef struct ESContext ESContext;

struct ESContext
{
    // Platform specific data
    void *platformData;
    
    // User Data
    void *userData;
    
    // window width
    GLint width;
    
    // window height
    GLint height;
    
#ifndef __APPLE__
    // Display Handle
    EGLNativeDisplayType eglNativeDisplay;
    // Window handle
    EGLNativeWindowType eglNativeWindow;
    // EGL display
    EGLDisplay eglDisplay;
    // EGL context
    EGLContext elgContext;
    // EGL surface
    EGLSurface eglSurface;
#endif
    
    // Callbacks
    void ( ESCALLBACK *drawFunc ) ( ESContext * );
    void ( ESCALLBACK *shutdownFunc ) ( ESContext * );
    void ( ESCALLBACK *keyFunc ) ( ESContext *, unsigned char, int, int );
    void ( ESCALLBACK *updateFunc ) ( ESContext *, float deltaTime );
};

// create a window with specified params
GLboolean ESUTIL_API esCreateWindow( ESContext *esContext, const char *title, GLint width, GLint height, GLuint flags );
// register a draw callback function to be used to render each frame
void ESUTIL_API esRegisterDrawFunc( ESContext *esContext, void ( ESCALLBACK *drawFunc )( ESContext * ) );
// register a callback function to be called on shutdown
void ESUTIL_API esRegisterShutdownFunc( ESContext *esContext, void ( ESCALLBACK * shutdownFunc )( ESContext * ) );
// register an update callback function to be used to update on each time step
void ESUTIL_API esRegisterUpdateFunc( ESContext *esContext, void( ESCALLBACK * updateFunc ) ( ESContext *, float ));
// register a keyboard input processing callback function
void ESUTIL_API esRegisterKeyFunc( ESContext *esContext, void( ESCALLBACK *keyFunc )( ESContext *, unsigned char, int,int ));
// log a message to the debug output for the platform
void ESUTIL_API esLogMessage( const char *formatStr, ... );
// load a shader, check for compile errors, print error msgs to output log
GLuint ESUTIL_API esLoadShader( GLenum type, const char *shaderSrc );
// load a vertex and fragment shader, create a program obj, link program
GLuint ESUTIL_API esLoadProgram( const char *vertShaderSrc, const char *fragShaderSrc );
// generates geometry for a sphere. Allocate mem for the vertex data and stores the
// results in the arrays. Generate index list for a TRIANGLE_STRIP
int ESUTIL_API esGenSphere( int numSlices, float radius, GLfloat **vertices, GLfloat **normals, GLfloat **texCoords, GLuint ** indices );
// generates geometry for a cube. Allocate mem for the vertex data and stores the
// results in the arrays. Generate index list for a TRIANGLES
int ESUTIL_API esGenCube( float scale, GLfloat ** vertices, GLfloat **normal, GLfloat **texCoords, GLuint **indices );
// generates a square grid consisting of triangles. Allocate mem for the vertex data and stores
// the results in the arrays. Generate index list as TRIANGLES
int ESUTIL_API esGenSquareGrid( int size, GLfloat **vertices, GLfloat **indices );
// loads a 8-bit,24-bit or 32-bit TGA img from a file
char *ESUTIL_API esLoadTGA( void *ioContext, const char *fileName, int *width, int *height );
// multiply matrix specified by result with a scaling matrix and return new matrix in result
void ESUTIL_API esScale( ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz );
// multiply matrix specified by result with a translation matrix and return new matrix in result
void ESUTIL_API esTranslate( ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz );
// multiply matrix specified by result with a rotation matrix and return new matrix in result
void ESUTIL_API esRotate( ESMatrix *result, GLfloat angle,  GLfloat x, GLfloat y, GLfloat z );
// multiply matrix specified by result with a perspective matrix and return new matrix in result
void ESUTIL_API esFrustum( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ );
// multiply matrix specified by result with a perspective matrix and return new matrix in result
void ESUTIL_API esPerspective( ESMatrix *result, float fovy, float aspect, float nearZ, float farZ  );
// multiply matrix specified by result with a perspective matrix and return new matrix in result
void ESUTIL_API esOrtho( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ );
// Perform the following op - result matrix = srcA matrix * srcB matrix
void ESUTIL_API esMatrixMultiply( ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB );
// Return an identity matrix
void ESUTIL_API esMatrixLoadIdentity( ESMatrix *result );
// Generate a transformation matrix from eye pos, look at and up vectors
void ESUTIL_API esMatrixLookAt( ESMatrix *result,
                               float posx, float posX, float posZ,
                               float lookAtX, float lookAtY, float lookAtZ,
                               float upX, float upY, float upZ);

#ifdef _cplusplus
}
#endif

#endif /* ESUtil_h */
