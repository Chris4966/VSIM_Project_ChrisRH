#include "rollingball.h"
#include "math_constants.h"

RollingBall::RollingBall(int n, VisualObject *surface)
{
    r = 1.f;
    mRecursions = n;
    mIndex = 0;
    mTriangleSurface = surface;
    mVertices.reserve(3 * 8 * pow(4, mRecursions));
    absoluteUnitBall();
    mMatrix.setToIdentity();
}

RollingBall::~RollingBall()
{

}

void RollingBall::move(float dt)
{
    if (mTime == 0) mVelocity = {0.f, 0.f, 0.f};
    mTime += dt;
    int index{0};


    for (unsigned int i{0}; i < mTriangleSurface->mTriangles.size(); i++)
    {
        //searches for current triangle with barycentric coordinates
        Triangle t = mTriangleSurface->mTriangles[i];
        gsl::vec3 bar = baryCoord(gsl::vec2{t.V1.getX(), t.V1.getZ()},
                                  gsl::vec2{t.V2.getX(), t.V2.getZ()},
                                  gsl::vec2{t.V3.getX(), t.V3.getZ()},
                                  gsl::vec2{mMatrix.getPosition().x, mMatrix.getPosition().z});


        if (bar.x >= 0 && bar.x <= 1 && bar.y >= 0 && bar.y <= 1 && bar.z >= 0 && bar.z <= 1)
        {
            qDebug() << "Barycentric coordinates: (" << bar.x << ", " << bar.y << ", " << bar.z << ")";

            //calculates normal and acceleration on current triangle
            mPrevIndex = i;

            gsl::vec3 n = gsl::vec3::cross(t.V2.mXYZ - t.V1.mXYZ, t.V3.mXYZ - t.V1.mXYZ);
            n.normalize();

            //a = [x * y, y^2 -1. z * y] * g
            mAcceleration = gsl::vec3{n.x * n.y, (n.y * n.y) -1, n.z * n.y} * gsl::GRAVITY;
            mAcceleration.normalize();

            mMatrix.setPosition(mMatrix.getPosition().x,
                                (t.V1.getY() * bar.x) + (t.V2.getY() * bar.y) + (t.V3.getY() * bar.z) + r,
                                mMatrix.getPosition().z);
        }

        if (index != mPrevIndex && index != -1)
        {
            //updates movement for the next triangles
            Triangle t0 = mTriangleSurface->mTriangles[mPrevIndex];
            Triangle t1 = mTriangleSurface->mTriangles[index];
            mPrevIndex = index;

            gsl::vec3 n0 = gsl::vec3::cross(t0.V2.mXYZ - t0.V1.mXYZ, t0.V3.mXYZ - t0.V1.mXYZ).normalized();
            gsl::vec3 n1 = gsl::vec3::cross(t1.V2.mXYZ - t1.V1.mXYZ, t1.V3.mXYZ - t1.V1.mXYZ).normalized();

            gsl::vec3 cn = gsl::vec3(n0 + n1).normalized();

            mVelocity -= (cn * gsl::vec3::dot(mVelocity, cn) * 2);

            mAcceleration = gsl::vec3{n1.x * n1.y, (n1.y * n1.y) -1, n1.z * n1.y} * gsl::GRAVITY;
            mAcceleration.normalize();
        }

        //calculates velocity and moves the ball
        mVelocity += mAcceleration * dt * mMass;
        mMatrix.translate(mVelocity);
    }
}

void RollingBall::init()
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

void RollingBall::draw()
{
    glBindVertexArray( mVAO );
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    glBindVertexArray(0);
}
