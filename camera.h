#ifndef CAMERA_H
#define CAMERA_H

#include "mat4x4.h"
#include "vec3.h"
/**
  This class still have some bugs. It mostly work, but when you rotate the camera 180 degrees
  the forward / backward is wrong, when steered with W and S.
 */
class Camera
{
public:
    Camera();

    void pitch(float degrees);
    void yaw(float degrees);
    void updateRightVector();
    void updateForwardVector();
    void update();

    gsl::mat4 mViewMatrix;
    gsl::mat4 mProjectionMatrix;

    void setPosition(const gsl::vec3 &position);

    void setSpeed(float speed);
    void updateHeigth(float deltaHeigth);
    void moveRight(float delta);

    gsl::vec3 position() const;
    gsl::vec3 up() const;

//private:
    gsl::vec3 mForward{0.f, 0.f, -1.f};
    gsl::vec3 mRight{1.f, 0.f, 0.f};
    gsl::vec3 mUp{0.f, 1.f, 0.f};

    gsl::vec3 mPosition{0.f, 0.f, 0.f};
    float mPitch{0.f};
    float mYaw{0.f};

    gsl::mat4 mYawMatrix;
    gsl::mat4 mPitchMatrix;

    float mSpeed{0.f}; //camera will move by this speed along the mForward vector
};

#endif // CAMERA_H
