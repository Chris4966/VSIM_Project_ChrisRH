#include "octahedronball.h"
#include "material.h"

OctahedronBall::OctahedronBall(int n) : mRecursions(n), mIndex(0)
{
    mVertices.reserve(3 * 8 * pow(4, mRecursions));
    absoluteUnitBall();
    mMatrix.setToIdentity();
}

OctahedronBall::~OctahedronBall()
{

}

void OctahedronBall::makeTriangle(const gsl::vec3 &u, const gsl::vec3 &v, const gsl::vec3 &w)
{
    mVertices.push_back(Vertex{u.x, u.y, u.z, u.x, u.x, u.z, 0.f, 0.f});
    mVertices.push_back(Vertex{v.x, v.y, v.z, v.x, v.x, v.z, 1.f, 0.f});
    mVertices.push_back(Vertex{w.x, w.y, w.z, w.x, w.x, w.z, 0.5f, 1.f});
}

//recursive subdivision
void OctahedronBall::subDivide(const gsl::vec3 &u, const gsl::vec3 &v, const gsl::vec3 &w, int n)
{
    if (n > 0)
    {
        gsl::vec3 a = u+v; a.normalize();
        gsl::vec3 b = u+w; b.normalize();
        gsl::vec3 c = w+v; c.normalize();
        subDivide(u, a, b, n-1);
        subDivide(w, b, c, n-1);
        subDivide(v, c, a, n-1);
        subDivide(c, b, a, n-1);
    }
    else
    {
        makeTriangle(u, v, w);
    }
}

void OctahedronBall::absoluteUnitBall()
{
    gsl::vec3 v0{0, 0, 1};
    gsl::vec3 v1{1, 0, 0};
    gsl::vec3 v2{0, 1, 0};
    gsl::vec3 v3{-1, 0, 0};
    gsl::vec3 v4{0, -1, 0};
    gsl::vec3 v5{0, 0, -1};

    subDivide(v0, v1, v2, mRecursions);
    subDivide(v0, v2, v3, mRecursions);
    subDivide(v0, v3, v4, mRecursions);
    subDivide(v0, v4, v1, mRecursions);
    subDivide(v5, v2, v1, mRecursions);
    subDivide(v5, v3, v2, mRecursions);
    subDivide(v5, v4, v3, mRecursions);
    subDivide(v5, v1, v4, mRecursions);
}

void OctahedronBall::init()
{
    initializeOpenGLFunctions();

    //Vertex Array Object - VAO
    glGenVertexArrays( 1, &mVAO );
    glBindVertexArray( mVAO );

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers( 1, &mVBO );
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );

    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof( Vertex ), mVertices.data(), GL_STATIC_DRAW );

    // 1rst attribute buffer : vertices
    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);

    // 3rd attribute buffer : uvs
    glVertexAttribPointer(2, 2,  GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)( 6 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void OctahedronBall::draw()
{
    glBindVertexArray( mVAO );
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    glBindVertexArray(0);
}
