//
// Includes
//

#include "ESUtil.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

//
/// \brief Generates geometry for a cube. Allocates memory for the vertex data and stores
///        the results in the arrays. Generate index list for a TRIANGLES
/// \param scale The size of the cube, use 1.0 for a unit cube.
/// \param vertices If not NULL, will contain array of float3 positions
/// \param normals If not NULL, will contain array of float3 normals
/// \param texCoords If not NULL, will contain array of float2 texCoords
/// \param indices If not NULL, will contain the array of indices for the triangle strip
/// \return The number of indices required for rendering the buffers ( the number of indices stored in the indices array
///         if it is not NULL ) as a GL_TRIANGLE_STRIP
int ESUTIL_API esGenCube( float scale, GLfloat **vertices, GLfloat **normals,
                          GLfloat **texCoords, GLuint **indices )
{
    int i;
    int numVertices = 24;
    int numIndices  = 36;
    
    GLfloat cubeVerts[] =
    {
        // bottom plane
        -0.5f, -0.5f, -0.5f,//0 ( back-left-bottom 1 )
        -0.5f, -0.5f,  0.5f,//1 ( front-left-bottom 2 )
         0.5f, -0.5f,  0.5f,//2 ( front-right-bottom 3 )
         0.5f, -0.5f, -0.5f,//3 ( back-right-bottom 4 )
        // top plane
        -0.5f,  0.5f, -0.5f,//4 ( back-left-top 5 )
        -0.5f,  0.5f,  0.5f,//5 ( front-left-top 6 )
         0.5f,  0.5f,  0.5f,//6 ( front-right-top 7 )
         0.5f,  0.5f, -0.5f,//7 ( back-right-top 8 )
        // back plane
        -0.5f, -0.5f, -0.5f,//8 ( back-left-bottom 1 )
        -0.5f,  0.5f, -0.5f,//9 ( back-left-top 5 )
         0.5f,  0.5f, -0.5f,//10( back-right-top 8 )
         0.5f, -0.5f, -0.5f,//11( back-right-bottom 4 )
        // front plane
        -0.5f, -0.5f, 0.5f,//12( front-left-bottom 2 )
        -0.5f,  0.5f, 0.5f,//13( front-left-top 6 )
         0.5f,  0.5f, 0.5f,//14( front-right-top 7 )
         0.5f, -0.5f, 0.5f,//15( front-right-bottom 3 )
        // left plane
        -0.5f, -0.5f,-0.5f,//16( back-left-bottom 1 )
        -0.5f, -0.5f, 0.5f,//17( front-left-bottom 2 )
        -0.5f,  0.5f, 0.5f,//18( front-left-top 6 )
        -0.5f, 0.5f, -0.5f,//19( back-left-top 5 )
        // right plane
        0.5f, -0.5f,-0.5f,//20( back-right-bottom 4 )
        0.5f, -0.5f, 0.5f,//21( front-right-bottom 3 )
        0.5f,  0.5f, 0.5f,//22( front-right-top 7 )
        0.5f,  0.5f,-0.5f,//23( back-right-top 8 )
    };
    
    GLfloat cubeNormals[] =
    {
        // bottom
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        // top
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        // back
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        // front
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        // left
        -1.0, 0.0f, 0.0f,
        -1.0, 0.0f, 0.0f,
        -1.0, 0.0f, 0.0f,
        -1.0, 0.0f, 0.0f,
        // right
        1.0, 0.0f, 0.0f,
        1.0, 0.0f, 0.0f,
        1.0, 0.0f, 0.0f,
        1.0, 0.0f, 0.0f,
    };
    
    GLfloat cubeTex[] =
    {
        // bottom ( 1->2->3->4 )
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        // top ( 5->6->7->8 )
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        // back ( 1->5->8->4 )
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        // front ( 2->6->7->3 )
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        // left ( 1->2->6->5 )
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        // right ( 4->3->7->8 )
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };
    
    // allocate mem for buffers
    if( vertices != NULL )
    {
        *vertices = malloc( sizeof(GLfloat) * 3 * numVertices );
        memcpy( *vertices, cubeVerts, sizeof( cubeVerts ));
        
        for( i = 0; i < numVertices * 3; ++i )
        {
            ( *vertices )[i] *= scale;
        }
    }
    
    if( normals != NULL )
    {
        *normals = malloc( sizeof(GLfloat) * 3 * numIndices );
        memcpy( *normals, cubeNormals, sizeof( cubeNormals ));
    }
    
    if( texCoords != NULL )
    {
        *texCoords = malloc( sizeof( GLfloat ) * 2 * numVertices );
        memcpy( *texCoords, cubeTex, sizeof(texCoords));
    }
    
    // Generate the indices
    if( indices != NULL )
    {
        GLuint cubeIndices[] =
        {
          // bottom
          0, 2, 1,
          0, 3, 2,
          // top
          4, 5, 6,
          4, 6, 7,
          // back
          8,  9, 10,
          8, 10, 11,
          // front
          12, 15, 14,
          12, 14, 13,
          // left
          16, 17, 18,
          16, 18, 19,
          // right
          20, 23, 22,
          20, 22, 21,
        };
        
        *indices = malloc(sizeof(GLuint) * numIndices);
        memcpy( *indices, cubeIndices, sizeof(cubeIndices));
    }
    
    return numIndices;
}
