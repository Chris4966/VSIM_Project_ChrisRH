#ifndef VISUALOBJECT_H
#define VISUALOBJECT_H

#include <QOpenGLFunctions_4_1_Core>
#include <vector>
#include "vertex.h"
#include "mat4x4.h"
#include "shader.h"
#include "constants.h"

class RenderWindow;
class Material;

struct Triangle
{
    Triangle(Vertex v1, Vertex v2, Vertex v3) : V1{v1}, V2{v2}, V3{v3}
    {

    }

    Vertex V1, V2, V3;
};


class VisualObject : public QOpenGLFunctions_4_1_Core {
public:
    VisualObject() = default;
    virtual ~VisualObject();
    virtual void init();
    virtual void draw()=0;

    virtual void move(float dt)
    {
        mMatrix.translateX(dt);
    };

    void calcNormals(std::vector<Vertex> *verts, bool bSmooth);

    std::vector<gsl::vec3> mVerticesFlat;
    std::vector<gsl::vec3> mVerticesSmooth;
    std::vector<gsl::vec3> mVerticesPos;

    void setTriangles();

    //barycentric coordinates
    gsl::vec3 baryCoord(const gsl::vec2 &p1, const gsl::vec2 &p2, const gsl::vec2 &p3, const gsl::vec2 &pos);

    ///Use to draw the normals of this mesh
    void drawNormals(GLint matrixUniform);

    ///Should this objects normals be drawn from drawNormals()-function - set this from editor (eventually)
    bool mDrawNormals{true};
    bool bIsHidden{false};
    bool bIsPicked{false};
    bool bReachedEndPoint{false};
    bool bIsMoving{false};

    gsl::States mState; //states for the NPC

    gsl::mat4 mMatrix;
    std::vector<Triangle> mTriangles;
    std::string mName;
    gsl::vec3 mInitPos;
    float mTime{0};

    Material *mMaterial{nullptr};

    RenderWindow *mRenderWindow{nullptr}; //Just to be able to call checkForGLerrors()
    int mSceneNr{0};

    //collision stuff
    bool bCollidable{false};
    gsl::vec3 mOBBmin, mOBBmax;
    void setOBB(gsl::vec3 min, gsl::vec3 max);
    bool testOBBcollision(VisualObject *a);

protected:
    std::vector<Vertex> mVertices;
    std::vector<GLuint> mIndices;
    int mIndiceCount{0};

    GLuint mVAO{0};
    GLuint mVBO{0};
    GLuint mEAB{0}; //holds the indices (Element Array Buffer - EAB)
};
#endif // VISUALOBJECT_H

