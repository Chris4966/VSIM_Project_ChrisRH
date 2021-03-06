#include "quad.h"
#include "vertex.h"

Quad::Quad()
{

    //                          // Positions            // normals        //UV

    mVertices.emplace_back(Vertex(-1.f, 0.f, 1.f,       1, 0, 0,          0.f, 0.f));
    mVertices.emplace_back(Vertex(1.f, 0.f, 1.f,        0, 1, 0,          1.f, 0.f));
    mVertices.emplace_back(Vertex(1.f, 0.f, -1.f,       0, 0, 1,          1.f, 1.f));

    mVertices.emplace_back(Vertex(-1.f, 0.f, 1.f,       1, 0, 0,          0.f, 0.f));
    mVertices.emplace_back(Vertex(1.f, 0.f, -1.f,       0, 0, 1,          1.f, 1.f));
    mVertices.emplace_back(Vertex(-1.f, 0.f, -1.f,      0, 0, 1,          0.f, 1.f));

    mMatrix.setToIdentity();
}

void Quad::init()
{
    //must call this to use OpenGL functions
    initializeOpenGLFunctions();

    glGenVertexArrays( 1, &mVAO );
    glBindVertexArray( mVAO );

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers( 1, &mVBO );
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );

    //Vertex Buffer Object to hold vertices - VBO
    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof( Vertex ), mVertices.data(), GL_STATIC_DRAW );

    // 1rst attribute buffer : vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0  );          // array buffer offset
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);

    // 3rd attribute buffer : uvs
    glVertexAttribPointer(2, 2,  GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)( 6 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Quad::draw()
{
    glBindVertexArray( mVAO );
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    glBindVertexArray(0);
}
