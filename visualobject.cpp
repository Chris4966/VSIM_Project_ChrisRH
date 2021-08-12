#include "visualobject.h"
#include "shader.h"

VisualObject::~VisualObject()
{
    initializeOpenGLFunctions();
    glDeleteVertexArrays( 1, &mVAO );
    glDeleteBuffers( 1, &mVBO );
}

void VisualObject::init()
{
}

void VisualObject::calcNormals(std::vector<Vertex> *verts, bool bSmooth)
{
    int it{0}; //index

    //loops through vertices and calculates flat normals
    for (unsigned int i{0}; i < verts->size()/3; i++)
    {
        //sets 3 vertices and increases index
        Vertex *p1 = &verts->at(it++);
        Vertex *p2 = &verts->at(it++);
        Vertex *p3 = &verts->at(it++);

        //creates vectors v12 and v13, then calculates normal with the cross product
        gsl::vec3 v12 = (p2->mXYZ - p1->mXYZ);
        gsl::vec3 v13 = (p3->mXYZ - p1->mXYZ);
        gsl::vec3 n = gsl::vec3::cross(v13, v12);
        n.normalize();

        //inserts normal into vertices;
        p1->mNormal = p2->mNormal = p3->mNormal = n;
    }

    //if using smooth normals
    if (bSmooth)
    {
        for (auto p : *verts)
        {
            mVerticesFlat.push_back(p.mNormal);
            mVerticesPos.push_back(p.mXYZ);
        }

        for (unsigned int j{0}; j < mVerticesFlat.size(); j++)
            mVerticesSmooth.push_back(gsl::vec3{0.f, 0.f, 0.f});

        //booleans to check if a normal is visited
        std::vector<bool> bVisited;
        for (unsigned int k{0}; k < mVerticesFlat.size(); k++)
            bVisited.push_back(false);

        std::vector<int> indices;

        gsl::vec3 nSum;

        for (unsigned int l{0}; l < mVerticesFlat.size(); ++l)
        {
            indices.clear();
            nSum = gsl::vec3{0.f, 0.f, 0.f};

            if (!bVisited.at(l))
            {
                //inserts normal from current vector element
                bVisited.at(l) = true;
                nSum += mVerticesFlat.at(l);
                indices.push_back(l);

                //checks the rest of the vector and sums up normals
                for (unsigned int m{l+1}; m < mVerticesFlat.size(); ++m)
                {
                    if (!bVisited.at(m) && mVerticesPos.at(m) == mVerticesPos.at(l))
                    {
                        nSum += mVerticesFlat.at(m);
                        indices.push_back(m);
                        bVisited.at(m) = true;
                    }
                }

                for (auto q : indices)
                {
                    mVerticesSmooth.at(q) = nSum.normalized();
                }
            }
        }

        //inserts smooth normals to vertex vector
        for (unsigned int o{0}; o < verts->size(); ++o)
            verts->at(o).mNormal = mVerticesSmooth.at(o);
    }
}

void VisualObject::setTriangles()
{
    unsigned int i{0};
    Vertex a, b, c;

    while (i < mVertices.size()-1)
    {
        a = mVertices.at(i++);
        b = mVertices.at(i++);
        c = mVertices.at(i++);

        mTriangles.push_back(Triangle(a, b, c));
    }
}

gsl::vec3 VisualObject::baryCoord(const gsl::vec2 &p1, const gsl::vec2 &p2, const gsl::vec2 &p3, const gsl::vec2 &pos)
{
    gsl::vec2 p12 = p2 - p1;
    gsl::vec2 p13 = p3 - p1;
    gsl::vec3 n = {0.f, 0.f, (p13.x * p12.y) - (p13.y * p12.x)};
    float A = n.length();

    //u
    gsl::vec2 p = p2 - pos;
    gsl::vec2 q = p3 - pos;
    gsl::vec3 n1 = {0.f, 0.f, (q.x * p.y) - (q.y * p.x)};

    //v
    p = p3 - pos;
    q = p1 - pos;
    gsl::vec3 n2 = {0.f, 0.f, (q.x * p.y) - (q.y * p.x)};

    //w
    p = p1 - pos;
    q = p2 - pos;
    gsl::vec3 n3 = {0.f, 0.f, (q.x * p.y) - (q.y * p.x)};

    return gsl::vec3{n1.z/A, n2.z/A, n3.z/A};
}

void VisualObject::drawNormals(GLint matrixUniform)
{
    glBindVertexArray( mVAO );
    glUniformMatrix4fv(matrixUniform, 1, GL_TRUE, mMatrix.constData());   //move to shader???
    if(mIndiceCount > 0)
        glDrawElements(GL_TRIANGLES, mIndiceCount, GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(GL_TRIANGLES, 0, mVertices.size());

    glBindVertexArray(0);
}

void VisualObject::setOBB(gsl::vec3 min, gsl::vec3 max)
{
    bCollidable = true;
    mOBBmin = min;
    mOBBmax = max;
}

bool VisualObject::testOBBcollision(VisualObject *a)
{
    return mOBBmax >= a->mOBBmin && a->mOBBmax >= mOBBmin;
}
