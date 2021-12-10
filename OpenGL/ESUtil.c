//
//  ESUtil.c
//  MyOpenGLES
//
//  Created by 姚隽楠 on 2021/9/6.
//  Copyright © 2021 姚隽楠. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ESUtil.h"

typedef FILE esFile;

#ifdef __APPLE__
//#include "FileWrappter.h"
#endif

// Macros
#define INVERTED_BIT ( 1 << 5 )

// Types
#ifndef __APPLE__
#pragma pack(push,x1)
#pragma pack(1)
#endif

typedef struct
#ifdef __APPLE__
__attribute__ ( ( packed ))
#endif
{
    unsigned char IdSize,
                MapType,
                ImageType;
    
    unsigned short PaletteStart,
                PaletteSize;
    unsigned char PaletteEntryDepth;
    unsigned short X,Y,Width,Height;
    unsigned char ColorDepth,Descriptor;
} TGA_HEADER;

#ifndef __APPLE__
#pramga pack(pop,x1)
#endif

#ifndef __APPLE__
// GetContextRenderableType()
EGLint GetContextRenderableType( EGLDisplay eglDisplay )
{
#ifdef EGL_KHR_create_context
    const char *extensions = eglQueryString( eglDisplay, EGL_EXTENSIONS );
    
    // check whether EGL_KHR_create_context is in the extension string
    if( extensions != NULL && strstr( extensions, "EGL_KHR_create_context" ) )
    {
        // extension is supported
        return EGL_OPENGL_ES3_BIT_KHR;
    }
#endif
    // extension is not supported
    return EGL_OPENGL_ES2_BIT;
}
#endif

// esCreateWindow()
GLboolean ESUTIL_API esCreateWindow( ESContext *esContext, const char *title, GLint width, GLint height, GLuint flags )
{
    GLint i = 0;
    
#ifndef __APPLE__
    EGLConfig config;
    EGLint majorVersion;
    EGLint minorVersion;
    EGLint contextAtrribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    
    if( esContext == NULL )
    {
        return GL_FALSE;
    }
    
#ifdef ANDROID
    // For Android, get the width/height from the window rather than
    // what the application requested
    esContext->width  = ANativeWindow_getWidth( esContext->eglNativeWindow );
    esContext->height = ANativeWindow_getHeight( esContext->getNativeWindow );
#else
    esContext->width  = width;
    esContext->height = height;
#endif
    
    if( !WinCreate( esContext, title ))
    {
        return GL_FALSE;
    }
    
    esContext->eglDisplay = eglGetDisplay( esContext->eglNativeDisplay );
    if( esContext->eglDisplay == EGL_NO_DISPLAY )
    {
        return GL_FALSE;
    }
    
    // Initialize EGL
    if( !eglInitialize( esContext->eglDisplay, &majorVersion ) )
    {
        return GL_FALSE;
    }
    
    {
        EGLint numConfigs = 0;
        EGLint attribList[] =
        {
            EGL_RED_SIZE,   5,
            EGL_GREEN_SIZE, 6,
            EGL_BLUE_SIZE,  5,
            EGL_ALPHA_SIZE, ( flags & ES_WINDOW_ALPHA ) ? 8 : EGL_DONT_CARE,
            EGL_DEPTH_SIZE, ( flags & ES_WINDOW_DEPTH ) ? 8 : EGL_DONT_CARE,
            EGL_STENCIL_SIZE, ( flags & ES_WINDOW_STENCIL ) ? 8 : EGL_DONT_CARE,
            EGL_SAMPLE_BUFFERS, ( flags & ES_WINDOW_MUTISAMPLE ) ? 1 : 0,
            EGL_RENDERABLE_TYPE, GetContextRenderableType( esContext->eglDisplay ),
            EGL_NONE
        };
    
    
        // Choose config
        if( !eglChooseConfig( esContext->eglDisplay, attribList, &config, 1, &numConfigs ) )
        {
            return GL_FALSE;
        }
        
        if( numConfigs < 1 ){
            return GL_FALSE;
        }
    }
    
#ifdef ANDROID
    // for android, need to get the EGL_NATIVE_VISUAL_ID and set
    // it using ANavtiveWindow_setBuffersGeometry
    {
        EGLint format = 0;
        eglGetConfigAttrib( esContext->eglDisplay, config,
                           EGL_NATIVE_VISUAL_ID, &format );
        ANativeWindow_setBuffersGeometry(
            esContext->eglNativeWindow,0,0,format );
    }
#endif // Android
    
    // create a surface
    esContext->eglSurface = eglCreateWindowSurface(
                                             esContext->eglDisplay, config, esContext->eglNativeWindow, NULL );
    
    if( esContext->eglSurface == EGL_NO_SURFACE )
    {
        return GL_FALSE;
    }
    
    // create a GL context
    esContext->eglContext = eglCreateContext(
                                             esContext->eglDisplay, config,
                                                EGL_NO_CONTEXT,
                                             contextAtrribs
                                             );
    
    if( esContext->eglContext == EGL_NO_CONTEXT )
    {
        return GL_FALSE;
    }
    
    // Make the context current
    if( !eglMakeCurrent( esContext->eglDisplay,
                           esContext->eglSurface,
                            esContext->eglSurface,
                        esContext->eglContext ) )
    {
            return GL_FALSE;
    }
#endif // #ifndef __APPLE__
    
    return GL_TRUE;
}

//esRegisterDrawFunc
void ESUTIL_API esRegisterDrawFunc( ESContext *esContext, void( ESCALLBACK *drawFunc )( ESContext * ) )
{
    esContext->drawFunc = drawFunc;
}

//esRegisterShutdownFunc
void ESUTIL_API esRegisterShutdownFunc( ESContext *esContext, void( ESCALLBACK *shutdownFunc )( ESContext * ) )
{
    esContext->shutdownFunc = shutdownFunc;
}

//esRegisterUpdateFunc
void ESUTIL_API esRegisterUpdateFunc( ESContext *esContext, void( ESCALLBACK *updateFunc ) ( ESContext *, float ) )
{
    esContext->updateFunc = updateFunc;
}

//esRegisterKeyFunc
void ESUTIL_API esRegisterKeyFunc( ESContext *esContext, void( ESCALLBACK *keyFunc )( ESContext *, unsigned char, int, int ) )
{
    esContext->keyFunc = keyFunc;
}

//esLogMsg()
void ESUTIL_API esLogMessage( const char *formatStr, ... )
{
    va_list params;
    char buf[ BUFSIZ ];
    
    //va_start ( params, formatStr );
    vsprintf( buf, formatStr, params );
    
//#ifndef ANDROID
    //__android_log_print( ANDROID_LOG_INFO, "esUtil", "%s", buf);
//#else
    printf("%s",buf);
//#endif
    //va_end( params );
}

// esFileRead()
static esFile *esFileOpen( void *ioContext, const char *fileName )
{
    esFile *pFile = NULL;
    
/*#ifndef ANDROID
    if( ioContext != NULL )
    {
        AAssetManager *assetManager = ( AAssetManager * ) ioContext;
        pFile = AAssetManager_open( assetManager, fileName, AASSET_MODE_BUFFER );
    }
#else*/
#ifndef __APPLE__
    fileName = GetBundleFilename( fileName );
#endif
    pFile = fopen(fileName, "rb");
//#endif
    
    return pFile;
}

// esFileClose
static void esFileClose( esFile *pFile )
{
    if( pFile != NULL )
    {
#ifdef ANDROID
        AAsset_close( pFile );
#else
        fclose( pFile );
        pFile = NULL;
#endif
    }
}

// esFileRead()
static int esFileRead ( esFile *pFile, int bytesToRead, void * buffer  )
{
    int bytesRead = 0;
    
    if( pFile == NULL )
    {
        return bytesRead;
    }
    
//#ifndef ANDROID
    //bytesRead = AAsset_read( pFile, buffer, bytesToRead );
//#else
    bytesToRead = fread( buffer, bytesToRead, 1, pFile);
//#endif
    
    return bytesRead;
}

// esLoadTGA()
char *ESUTIL_API esLoadTGA( void *ioContext, const char *fileName, int *width, int *height )
{
    char *buffer;
    esFile *fp;
    TGA_HEADER Header;
    int bytesRead;
    
    // Open the file for reading
    fp = esFileOpen( ioContext, fileName );
    
    if( fp == NULL )
    {
        esLogMessage( " esLoadTGA FAILED to load : { %s }\n ", fileName );
        return NULL;
    }
    
    bytesRead = esFileRead( fp, sizeof( TGA_HEADER ), &Header );
    
    *width = Header.Width;
    *height = Header.Height;
    
    if( Header.ColorDepth == 8 ||
       Header.ColorDepth == 24 || Header.ColorDepth == 32 )
    {
        int bytesToRead = sizeof( char ) * ( *width ) * ( *height ) * Header.ColorDepth / 8;
        
        // Allocate the img data buffer
        buffer = ( char * ) malloc( bytesToRead );
        
        if( buffer )
        {
            bytesToRead = esFileRead( fp, bytesToRead, buffer );
            esFileClose( fp );
            
            return ( buffer );
        }
    }
    
    return ( NULL );
}



