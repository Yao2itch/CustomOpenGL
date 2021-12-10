//
//  MyGLApplication.c
//  MyOpenGLES
//
//  Created by 姚隽楠 on 2021/9/13.
//  Copyright © 2021 姚隽楠. All rights reserved.
//

#include "ESUtil.h"
#include <string.h>
#include <math.h>

#define NUM_INSTANCES 1
#define POSITION_LOC 0
#define COLOR_LOC    1
#define MVP_LOC      2

typedef struct
{
    // handle to a program object
    GLuint programObject;
    // VertexBufferObject ids
    GLuint vboIds[3];
    // VertexArrayObject Id
    GLuint vaoId;
    // x-offset uniform location
    GLuint offsetLoc;
    
    // Instancing
    // ---
    GLuint positionVBO;
    GLuint colorVBO;
    GLuint mvpVBO;
    GLuint indicesIBO;
    // Number of indices
    int numIndices;
    // Rotation angle
    GLfloat angle[NUM_INSTANCES];
    // ---
    
    // Vertex
    // ---
    // Uniform locations
    GLint mvpLoc;
    // Vertex data
    GLfloat *vertices;
    GLuint  *indices;
    //Rotation angle
    ESMatrix mvpMatrix;
    // ---
    
} UserData;

#define VERTEX_POS_SIZE   3 // x,y and z
#define VERTEX_COLOR_SIZE 4 // r,g,b, and a

#define VERTEX_POS_INDX   0
#define VERTEX_COLOR_INDX 1

#define VERTEX_STRIDE ( sizeof(GLfloat) * \
                         ( VERTEX_POS_SIZE + \
                            VERTEX_COLOR_SIZE ) )

void GenerateCubeVertexShader(UserData *userData)
{
    userData->numIndices = esGenCube( 1.0, &userData->vertices, NULL, NULL, &userData->indices );
    
    userData->angle[0] = 45.0f;
}

void GenerateCubesByInstancing(UserData *userData)
{
   // generate the vertex data
   GLfloat *positions;
   GLuint  *indices;
   userData->numIndices = esGenCube( 0.5f, &positions, NULL, NULL, &indices);
       
   // Index buffer obj
   glGenBuffers( 1, &userData->indicesIBO );
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, userData->indicesIBO);
   glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( GLint ) * userData->numIndices, indices, GL_STATIC_DRAW );
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
   free( indices );
    
   // Position VBO for cube model
   glGenBuffers( 1, &userData->positionVBO );
   glBindBuffer( GL_ARRAY_BUFFER, userData->positionVBO );
   glBufferData( GL_ARRAY_BUFFER, 24 * sizeof( GLfloat ) * 3, positions, GL_STATIC_DRAW);
   free( positions );
   
   // Random color for each instance
   {
       GLubyte colors[NUM_INSTANCES][4];
       int instance;
       
       srandom( 0 );
       
       for( instance = 0; instance < NUM_INSTANCES; instance++ )
       {
           colors[instance][0] = random() % 255;
           colors[instance][1] = random() % 255;
           colors[instance][2] = random() % 255;
           colors[instance][3] = 0;
       }
       
       glGenBuffers( 1, &userData->colorVBO );
       glBindBuffer( GL_ARRAY_BUFFER, userData->colorVBO );
       glBufferData( GL_ARRAY_BUFFER, NUM_INSTANCES * 4, colors, GL_STATIC_DRAW );
   }
    
   // Allocate storage to store MVP per instance
   {
       int instance;
       
       for( instance = 0; instance < NUM_INSTANCES; instance++ )
       {
           userData->angle[instance] = (float) ( random() % 32768 ) / 32767.0f * 360.0f;
       }
       
       glGenBuffers( 1, &userData->mvpVBO );
       glBindBuffer( GL_ARRAY_BUFFER, userData->mvpVBO);
       glBufferData( GL_ARRAY_BUFFER, NUM_INSTANCES * sizeof( ESMatrix ), NULL, GL_DYNAMIC_DRAW );
   }
   glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

int Init( ESContext *esContext )
{
    UserData *userData = esContext->userData;
    char vShaderStr[] =
        "#version 300 es                          \n"
        "layout(location = 0) in vec4 a_position; \n"
        "layout(location = 1) in vec4 a_color;    \n"
        "layout(location = 2) in mat4 a_mvpMatrix;\n"
        "uniform float u_offset;                  \n"
        "uniform mat4 u_mvpMatrix;                \n"
        "out vec4 v_color;                        \n"
        "void main()                              \n"
        "{                                        \n"
        " v_color = a_color;                      \n"
        " gl_Position = u_mvpMatrix * a_position; \n"
      //" gl_Position.x += u_offset;              \n"
        "}                                        \n";
    
    char fShaderStr[] =
        "#version 300 es                         \n"
        "precision mediump float;                \n"
        "in vec4 v_color;                        \n"
        //"out vec4 o_fragColor;                 \n"
        "layout(location = 0) out vec4 outColor; \n"
        "void main()                             \n"
        "{                                       \n"
        " outColor = v_color;                    \n"
        "}                                       \n";
    
    GLuint  programObject;
    
    // 3 vertices, with ( x,y,z ) , ( r, g, b, a ) per-vertex
    /*GLfloat vertices[ 3 * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE ) ] = {
        0.0f, 0.5f, 0.0f, // v0
        1.0f, 0.0f, 0.0f, 1.0f, // c0
       -0.5f,-0.5f, 0.0f, // v1
        0.0f, 1.0f, 0.0f, 1.0f, // c1
        0.5f,-0.5f, 0.0f, // v2
        0.0f, 0.0f, 1.0f, 1.0f, // c2
    };*/
    // Index buffer data
    //GLushort indices[3] = { 0, 1, 2 };
    
    // Create the program object
    programObject  = esLoadProgram( vShaderStr, fShaderStr );
    // get uniform location
    userData->offsetLoc = glGetUniformLocation( programObject, "u_offset" );
    userData->mvpLoc = glGetUniformLocation( programObject, "u_mvpMatrix");
    
    if( programObject == 0 )
    {
        return GL_FALSE;
    }
    
    // Store the program object
    userData->programObject = programObject;
    
    // vbo
    userData->vboIds[0] = 0;
    userData->vboIds[1] = 0;
    userData->vboIds[2] = 0;
    //
    
    // GenerateCubeInstanced( userData );
    
    GenerateCubeVertexShader( userData );
    
    glClearColor( 1.0f, 1.0f, 1.0f, 0.0f);
    
    return GL_TRUE;
}

void DrawPrimitiveviewWithVBOs( ESContext *esContext,
                                 GLint numVertices, GLfloat **vtxBuf,
                                 GLint *vtxStrides, GLint numIndices,
                                 GLushort *indices)
{
    UserData *userData = esContext->userData;
    
    // vboIds[0] - used to store vertex position
    // vboIds[1] - used to store vertex color
    // vboIds[2] - used to store element indices
    
    if( userData->vboIds[0] == 0 && userData->vboIds[1] == 0 &&
         userData->vboIds[2] == 0 )
    {
        // only allocate on the first draw
        glGenBuffers( 2, userData->vboIds );
        
        glBindBuffer( GL_ARRAY_BUFFER, userData->vboIds[0] );
        glBufferData( GL_ARRAY_BUFFER, vtxStrides[0] * numVertices, vtxBuf[0], GL_STATIC_DRAW );
        
        glBindBuffer( GL_ARRAY_BUFFER, userData->vboIds[1]);
        glBufferData( GL_ARRAY_BUFFER, vtxStrides[1] * numVertices, vtxBuf[1], GL_STATIC_DRAW );
        
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[2]);
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( GLushort ) * numIndices, indices, GL_STATIC_DRAW );
    }
    
    glBindBuffer( GL_ARRAY_BUFFER, userData->vboIds[0]);
    glEnableVertexAttribArray( VERTEX_POS_INDX );
    glVertexAttribPointer( VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, vtxStrides[0], 0);
    
    glBindBuffer( GL_ARRAY_BUFFER, userData->vboIds[1]);
    glEnableVertexAttribArray( VERTEX_COLOR_INDX );
    glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, vtxStrides[1], 0 );
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[2]);
    
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
    
    glDisableVertexAttribArray(VERTEX_POS_INDX);
    glDisableVertexAttribArray(VERTEX_COLOR_INDX);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DrawPrimitiveWithVBOsV2( ESContext *esContext,
                                 GLint numVertices, GLfloat *vtxBuf,
                                 GLint vtxStride, GLint numIndices,
                                 GLushort *indices)
{
    
    UserData *userData = esContext->userData;
    GLuint offset = 0;
    
    // vboIds[0] - used to store vertex position
    // vboIds[1] - used to store vertex color
    // vboIds[2] - used to store element indices
    
    if( userData->vboIds[0] == 0 && userData->vboIds[1] == 0 )
    {
        // only allocate on the first draw
        glGenBuffers( 2, userData->vboIds );
        
        glBindBuffer( GL_ARRAY_BUFFER, userData->vboIds[0] );
        glBufferData( GL_ARRAY_BUFFER, vtxStride * numVertices, vtxBuf, GL_STATIC_DRAW );
        
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( GLushort ) * numIndices, indices, GL_STATIC_DRAW );
    }
    
    glBindBuffer( GL_ARRAY_BUFFER, userData->vboIds[0] );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1] );
    
    glEnableVertexAttribArray( VERTEX_POS_INDX );
    glEnableVertexAttribArray( VERTEX_COLOR_INDX );
    
    glVertexAttribPointer( VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, vtxStride, (const void*) offset );
    
    offset += VERTEX_POS_SIZE * sizeof(GL_FLOAT);
    glVertexAttribPointer( VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, vtxStride, (const void *) offset );
    
    glDrawElements( GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0 );
    
    glDisableVertexAttribArray(VERTEX_POS_INDX);
    glDisableVertexAttribArray(VERTEX_COLOR_INDX);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DrawPrimitiveWithoutVBOs( GLfloat *vertices,
                                GLint vtxStride,
                                 GLint numIndices,
                                  GLushort * indices )
{
    GLfloat *vtxBuf = vertices;
    
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    
    glEnableVertexAttribArray( VERTEX_POS_INDX );
    glEnableVertexAttribArray( VERTEX_COLOR_INDX );
    
    glVertexAttribPointer( VERTEX_POS_INDX, VERTEX_POS_SIZE,
                           GL_FLOAT, GL_FALSE, vtxStride,
                           vtxBuf );
    
    vtxBuf += VERTEX_POS_SIZE;
    
    glVertexAttribPointer( VERTEX_COLOR_INDX,
                           VERTEX_COLOR_SIZE, GL_FLOAT,
                           GL_FALSE, vtxStride, vtxBuf );
    
    glDrawElements( GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT,
                    indices );
    
    glDisableVertexAttribArray( VERTEX_POS_INDX );
    glDisableVertexAttribArray( VERTEX_COLOR_INDX );
}

void DrawPrimitiveWithVBOsMapBuffers( ESContext *esContext,
                                      GLint numVertices, GLfloat *vtxBuf,
                                      GLint vtxStride, GLint numIndices,
                                      GLushort *indices)
{
    UserData *userData = esContext->userData;
    GLuint offset = 0;
    
    // vboIds[0] - used to store vertex attributes data
    // vboIds[1] - used to store element indices
    if( userData->vboIds[0] == 0 && userData->vboIds[1] == 0 )
    {
        GLfloat  *vtxMappedBuf;
        GLushort *idxMappedBuf;
        
        // only allocate on the first draw
        glGenBuffers( 2, userData->vboIds );
        
        glBindBuffer( GL_ARRAY_BUFFER, userData->vboIds[0] );
        glBufferData( GL_ARRAY_BUFFER, vtxStride * numVertices, NULL, GL_STATIC_DRAW );
        
        vtxMappedBuf = ( GLfloat * ) glMapBufferRange( GL_ARRAY_BUFFER, 0, vtxStride * numVertices, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );
        
        if( vtxMappedBuf == NULL )
        {
            esLogMessage( " Error mapping vertex buffer object. " );
            return;
        }
        
        // Copy the data into the mapped buffer
        memcpy( vtxMappedBuf, vtxBuf, vtxStride * numVertices );
        
        // Unmap the buffer
        if( glUnmapBuffer( GL_ARRAY_BUFFER ) == GL_FALSE )
        {
            esLogMessage( " Error unmapping array buffer object. " );
            return;
        }
        
        // Map the index buffer
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( GLushort ) * numVertices, NULL, GL_STATIC_DRAW );
        
        idxMappedBuf = ( GLushort * )
        glMapBufferRange( GL_ELEMENT_ARRAY_BUFFER, 0, sizeof( GLushort ) * numIndices, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );
        
        if( idxMappedBuf == NULL )
        {
            esLogMessage( " Error mapping element array buffer object. " );
            return;
        }
        
        // Copy the data into the mapped buffer
        memcpy( idxMappedBuf, indices, sizeof( GLushort ) * numIndices );
        
        // Unmap the buffer
        if( glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER ) == GL_FALSE )
        {
            esLogMessage( " Error unmapping element array buffer object " );
            return;
        }
    }
    
    glBindBuffer( GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
    
    glEnableVertexAttribArray( VERTEX_POS_INDX );
    glEnableVertexAttribArray( VERTEX_COLOR_INDX );
    
    glVertexAttribPointer( VERTEX_POS_INDX , VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, vtxStride, ( const void * ) offset );
    
    offset += VERTEX_POS_SIZE * sizeof( GLfloat );
    glVertexAttribPointer( VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, vtxStride, ( const void * ) offset );
    
    glDrawElements( GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0 );
    
    glDisableVertexAttribArray( VERTEX_POS_INDX );
    glDisableVertexAttribArray( VERTEX_COLOR_INDX );
    
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void DrawPrimitiveviewWithVAO( ESContext *esContext, GLint numVertices, GLfloat **vtxBuf, GLint vtxStride, GLint numIndices,
                                    GLushort *indices )
{
    UserData *userData = esContext->userData;
    
    if( userData->vboIds[0] == 0
            && userData->vboIds[1] == 0 )
    {
        //vao
        // Generate VBO Ids and load the VBOs with data
        glGenBuffers( 2, userData->vboIds );
        
        glBindBuffer( GL_ARRAY_BUFFER, userData->vboIds[0] );
        
        glBufferData( GL_ARRAY_BUFFER, numVertices * vtxStride, vtxBuf, GL_STATIC_DRAW );
         
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
        
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, numIndices, indices, GL_STATIC_DRAW );
    
        // Generate VAO id
        glGenVertexArrays( 1, &userData->vaoId );
    
        // Bind the VAO and then setup the vertex
        // attributes
        glBindVertexArray( userData->vaoId );
        
        glBindBuffer( GL_ARRAY_BUFFER, userData->vboIds[0]);
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
        
        glEnableVertexAttribArray( VERTEX_POS_INDX );
        glEnableVertexAttribArray( VERTEX_COLOR_INDX );
        
        glVertexAttribPointer( VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, ( const void * ) 0 );
        glVertexAttribPointer( VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, ( const void * ) ( VERTEX_POS_SIZE * sizeof( GLfloat ) ) );
        
        // Reset to the default VAO
        glBindVertexArray( 0 );
    }
    
    // Bind the VAO
    glBindVertexArray( userData->vaoId );
    // Draw with the VAO settings
    glDrawElements( GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, ( const void*) 0 );
    // Return to the default VAO
    glBindVertexArray( 0 );
}

void DrawCubesByInstancing( UserData *userData )
{
    // Load the vertex position
    glBindBuffer( GL_ARRAY_BUFFER, userData->positionVBO );
    glVertexAttribPointer( POSITION_LOC, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat),( const void *) NULL);
    glEnableVertexAttribArray( POSITION_LOC );
    
    // Load the instance color buffer
    glBindBuffer( GL_ARRAY_BUFFER, userData->colorVBO );
    glVertexAttribPointer( COLOR_LOC, 4, GL_UNSIGNED_BYTE, GL_TRUE, 4 * sizeof( GLubyte ), (const void *)NULL);
    glEnableVertexAttribArray( COLOR_LOC );
    glVertexAttribDivisor( COLOR_LOC, 1 );
    
    // Load the instance MVP buffer
    glBindBuffer( GL_ARRAY_BUFFER, userData->mvpVBO);
    
    // Load each matrix row of the MVP, Each row gets an increasing attribute location
    glVertexAttribPointer( MVP_LOC + 0, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix), (const void *) NULL );
    glVertexAttribPointer( MVP_LOC + 1, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix), (const void *) ( sizeof(GLfloat) * 4 ) );
    glVertexAttribPointer( MVP_LOC + 2, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix), (const void *) ( sizeof(GLfloat) * 8 ) );
    glVertexAttribPointer( MVP_LOC + 3, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix), (const void *) ( sizeof(GLfloat) * 12 ) );
    
    glEnableVertexAttribArray( MVP_LOC + 0 );
    glEnableVertexAttribArray( MVP_LOC + 1 );
    glEnableVertexAttribArray( MVP_LOC + 2 );
    glEnableVertexAttribArray( MVP_LOC + 3 );
    
    // One MVP per instance
    glVertexAttribDivisor( MVP_LOC + 0, 1 );
    glVertexAttribDivisor( MVP_LOC + 1, 1 );
    glVertexAttribDivisor( MVP_LOC + 2, 1 );
    glVertexAttribDivisor( MVP_LOC + 3, 1 );
    
    // Bind the index buffer
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, userData->indicesIBO);
    
    glDrawElementsInstanced( GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, (const void *) NULL, NUM_INSTANCES );
}

void DrawCubeByVertexShader( ESContext *esContext )
{
    UserData *userData = esContext->userData;
    
    // Load the vertex position
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), userData->vertices );
    glEnableVertexAttribArray( 0 );
    
    // Set the vertex position
    glVertexAttrib4f( 1, 1.0f, 0.0f, 0.0f, 1.0f );
    
    // Load the MVP matrix
    glUniformMatrix4fv( userData->mvpLoc, 1, GL_FALSE, ( GLfloat * )
                       &userData->mvpMatrix.m[0][0] );
    
    // Draw the cube
    glDrawElements( GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, userData->indices);
}
                            
void UpdateCubesByInstancing( ESContext *esContext, float deltaTime )
{
    UserData *useData = ( UserData * ) esContext->userData;
    ESMatrix *matrixBuf;
    ESMatrix perspective;
    float aspect;
    int instance = 0;
    int numRows;
    int numCols;
    
    // Compute the win aspect ratio
    aspect = ( GLfloat ) esContext->width / ( GLfloat ) esContext->height;
    
    // Generate a perspective matrix with a 60 degree FOV
    esMatrixLoadIdentity( &perspective );
    esPerspective( &perspective, 60.0f, aspect, 1.0f, 20.0f );
    
    glBindBuffer( GL_ARRAY_BUFFER, useData->mvpVBO );
    matrixBuf = ( ESMatrix * ) glMapBufferRange( GL_ARRAY_BUFFER, 0, sizeof( ESMatrix ) * NUM_INSTANCES, GL_MAP_WRITE_BIT );
    
    // Compute a per-instance MVP that translates and rotates each instance differently
    numRows = (int)sqrtf( NUM_INSTANCES );
    numCols = numRows;
    
    for( instance = 0; instance < NUM_INSTANCES; instance++ )
    {
        ESMatrix modelview;
        float translateX = 0.0f;//( ( float ) ( instance % numRows ) / ( float ) numRows ) * 2.0f - 1.0f;
        float translateY = 0.0f;//( ( float ) ( instance / numCols ) / ( float ) numCols ) * 2.0f - 1.0f;
        
        // Generate a mode view matrix to rotate/translate the cube
        esMatrixLoadIdentity( &modelview );
        
        // Per-instance translation
        esTranslate( &modelview, translateX, translateY, -5.0f);
        
        // Compute a rotation angle based on time to rotate the cube
        useData->angle[instance] += ( deltaTime * 40.0f );
        if( useData->angle[instance] >= 360.0f )
        {
            useData->angle[instance] -= 360.0f;
        }
        
        // Rotate the cube
        esRotate( &modelview, useData->angle[instance], 1.0f, 0.0, 1.0 );
        
        //esRotate( &modelview, 0.0, 1.0f, 0.0, 1.0 );
        // Compute the final MVP by multiplying the
        // modelview and perspective matrices together
        esMatrixMultiply( &matrixBuf[instance], &modelview, &perspective);
    }
    
    glUnmapBuffer( GL_ARRAY_BUFFER );
}

void UpdateCubeByVertexShader( ESContext *esContext, float deltaTime )
{
    UserData *useData = ( UserData * ) esContext->userData;
    ESMatrix perspective;
    ESMatrix modelview;
    float aspect;
    
    // Compute a rotation angle based on time to rotate the cube
    useData->angle[0] += ( deltaTime * 40.0f );
    
    if( useData->angle[0] >= 360.0f )
    {
        useData->angle[0] -= 360.0f;
    }
    
    // Compute the window aspect ratio
    aspect = (GLfloat) esContext->width / (GLfloat) esContext->height;
    
    // Generate a perspective matrix with a 60 degree FOV
    esMatrixLoadIdentity( &perspective );
    esPerspective( &perspective, 60.0f, aspect, 1.0, 20.0f );
    
    // Generate a model view matrix to rotate/translate the cube
    esMatrixLoadIdentity( &modelview );
    
    // Translate away from the viewer
    esTranslate( &modelview, 0.0f, 0.0f, -2.0);
    
    // Rotate the cube
    esRotate( &modelview, useData->angle[0], 1.0, 0.0, 1.0 );
    
    // Compute the final MVP by multiplying the
    // modelview and perspective matrices together
    esMatrixMultiply( &useData->mvpMatrix, &modelview, &perspective );
}

void Update( ESContext *esContext, float deltaTime )
{
    // UpdateCubesByInstanced( esContext, deltaTime );
    
    UpdateCubeByVertexShader( esContext, deltaTime );
}

void Draw( ESContext *esContext )
{
    UserData *userData = esContext->userData;
    
    // --- VBO ---
    // 3 vertices, with (x,y,z) , ( r,g,b,a ) per-vertex
    /*GLfloat vertexPos[3 * VERTEX_POS_SIZE] = {
         0.0f,   0.5f,  0.0f,
        -0.5f,  -0.5f,  0.0f,
         0.5f,  -0.5f,  0.0f
    };
    GLfloat color[ 4 * VERTEX_COLOR_SIZE ] = {
        1.0f, 0.0f, 0.0f, 1.0f, // c0
        0.0f, 1.0f, 0.0f, 1.0f, // c1
        0.0f, 0.0f, 1.0f, 1.0f  // c2
    };
    GLint vtxStrides[2] =
    {
        VERTEX_POS_SIZE * sizeof( GLfloat ),
        VERTEX_COLOR_SIZE * sizeof( GLfloat )
    };
    
    // Index buffer data
    GLushort indices[3] = { 0, 1, 2 };
    GLfloat  *vtxBuf[2] = { vertexPos, color };
    
    // set the viewport
    glViewport( 0, 0, esContext->width, esContext->height);
    // clear the color buffer
    glClear( GL_COLOR_BUFFER_BIT );
    // use the program object
    glUseProgram( usUserData *userDataerData->programObject );
    
    DrawPrimitiveviewWidthVBOs( esContext, 3, vtxBuf, vtxStrides, 3, indices );*/
    // --- ---
    
    // 3 vertices, with ( x,y,z ) , ( r,g,b,a ) per-vertex
    GLfloat vertices[ 3 * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE ) ] =
    {
        /*0.0f,  0.5f, 0.0f, // v0
        1.0f, 0.0f, 0.0f, 1.0f, // c0
        -0.5, -0.5f, 0.0f, // v1
        0.0f, 1.0f, 0.0f, 1.0f, // c1
        0.5f, -0.5f, 0.0f, // v2
        0.0f, 0.0f, 1.0f, 1.0f, // c2*/
        -0.5f,  0.5f, 0.0f, // v0
         1.0f,  0.0f, 0.0f, 1.0f, // c0
        -1.0f, -0.5f, 0.0f, // v1
         0.0f,  1.0f, 0.0f, 1.0f, // c1
         0.0f, -0.5f, 0.0f, // v2
         0.0f,  0.0f, 1.0f, 1.0f, // c2
    };
    GLushort indices[3] = { 0, 1, 2 };
    
    // set the viewport
    glViewport( 0, 0, esContext->width, esContext->height);
    // clear the color buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    // use the program object
    glUseProgram( userData->programObject );
    
    //glUniform1f( userData->offsetLoc, 0.0f );
    //DrawPrimitiveWithoutVBOs( vertices, sizeof( GLfloat ) * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE ) , 3, indices );
    
    //DrawPrimitiveWithVBOsMapBuffers( esContext, 3, vertices, sizeof(GL_FLOAT) * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE ), 3, indices);
    //DrawPrimitiveviewWithVAO( esContext, 3, vertices, sizeof( GL_FLOAT ) * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE ), sizeof( GLushort ) * 3, //indices );
    
    //glUniform1f( userData->offsetLoc, 1.0f );
    //DrawPrimitiveWithVBOsV2( esContext, 3, vertices, sizeof( GLfloat ) * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE ), 3, indices );
    
    // DrawCubsWithVBOs( userData );
    
    DrawCubeByVertexShader( esContext );
}

void Shutdown( ESContext *esContext )
{
    UserData *userData = esContext->userData;
    
    if( userData->vertices != NULL )
    {
        free( userData->vertices );
    }
    
    if( userData->indices != NULL )
    {
        free( userData->indices );
    }
    
    glDeleteProgram( userData->programObject );
}

int esMain( ESContext *esContext)
{
    esContext->userData = malloc(sizeof(UserData));
    
    esCreateWindow( esContext, "My Application", 640, 480, ES_WINDOW_RGB | ES_WINDOW_DEPTH );
    
    if( !Init( esContext ) )
    {
        return GL_FALSE;
    }
    
    esRegisterShutdownFunc( esContext, Shutdown );
    esRegisterDrawFunc( esContext, Draw );
    esRegisterUpdateFunc( esContext, Update);
    
    return GL_TRUE;
}
