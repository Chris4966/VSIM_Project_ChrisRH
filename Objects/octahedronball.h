#ifndef OCTAHEDRONBALL_H
#define OCTAHEDRONBALL_H

#include "visualobject.h"
#include <QOpenGLFunctions_4_1_Core>

class OctahedronBall : public VisualObject
{
public:
    OctahedronBall(int n = 0);
    ~OctahedronBall() override;

    void init() override;
    void draw() override;

    void makeTriangle(const gsl::vec3 &u, const gsl::vec3 &v, const gsl::vec3 &w);
    void subDivide(const gsl::vec3 &u, const gsl::vec3 &v, const gsl::vec3 &w, int n);
    void absoluteUnitBall();

    int mRecursions;
    int mIndex;

};

#endif // OCTAHEDRONBALL_H
