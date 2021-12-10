//
//  ESMapBuffers.c
//  MyOpenGLES
//
//  Created by 姚隽楠 on 2021/9/16.
//  Copyright © 2021 姚隽楠. All rights reserved.
//
#include "ESUtil.h"
#include <string.h>

typedef struct
{
    // handle to a program object
    GLuint programObject;
    // VertexBufferObject ids
    GLuint vboIds[3];
    
} UserData;

#define VERTEX_POS_SIZE   3 // x,y and z
#define VERTEX_COLOR_SIZE 4 // r,g,b, and a

#define VERTEX_POS_INDX   0
#define VERTEX_COLOR_INDX 1

int Init( ESContext *esContext )
{
    UserData *userData = esContext->userData;
    
    char vShaderStr[] =
        "#version 300 es                          \n"
        "layout(location = 0) in vec4 a_position; \n"
        "layout(location = 1) in vec4 a_color;    \n"
        "out vec4 v_color;                        \n"
        "void main()                              \n"
        "{                                        \n"
        " v_color = a_color;                      \n"
        " gl_Position = a_position;               \n"
        "}                                        \n";
    
    char fShaderStr[] =
        "#version 300 es                         \n"
        "precision mediump float;                \n"
        "in vec4 v_color;                        \n"
        "out vec4 o_fragColor;                   \n"
        "void main()                             \n"
        "{                                       \n"
        " o_fragColor = v_color;                 \n"
        "}                                       \n";
    
    GLuint  programObject;
    
    // Create the program object
    programObject  = esLoadProgram( vShaderStr, fShaderStr );
    
    if( programObject == 0 )
    {
        return GL_FALSE;
    }
    
    // Store the program object
    userData->programObject = programObject;
    
    userData->vboIds[0] = 0;
    userData->vboIds[1] = 0;
    userData->vboIds[2] = 0;
    
    glClearColor( 1.0f, 1.0f, 1.0f, 0.0f);
    return GL_TRUE;
}



