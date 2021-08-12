#ifndef ROLLINGBALL_H
#define ROLLINGBALL_H

#include "octahedronball.h"

class RollingBall : public OctahedronBall
{
public:
    RollingBall(int n = 0, VisualObject *surface = nullptr);
    ~RollingBall() override;

    void init() override;
    void draw() override;

    void move(float dt) override;
    void setRef();

    VisualObject *mTriangleSurface{nullptr};

    gsl::vec3 mAcceleration{0.f, 0.f, 0.f}, mVelocity{0.f, 0.f, 0.f};

    float r{1.f}, mMass{0.5};

    int mPrevIndex{0};
};

#endif // ROLLINGBALL_H
