#include "camera.h"

#include "math_constants.h"
#include <QDebug>

Camera::Camera()
{
    mViewMatrix.setToIdentity();
    mProjectionMatrix.setToIdentity();

    mYawMatrix.setToIdentity();
    mPitchMatrix.setToIdentity();
}

void Camera::pitch(float degrees)
{
    //  rotate around mRight
    mPitch -= degrees;
    updateForwardVector();
}

void Camera::yaw(float degrees)
{
    // rotate around mUp ?
    // rotate around world up?
    mYaw -= degrees;
    updateRightVector();
    updateForwardVector();
}

void Camera::updateRightVector()
{
    //Since camera is not allowed to roll, right will always be in XZ plane
    mRight = gsl::vec3{1.f, 0.f, 0.f};      //gsl::RIGHT
    mRight.rotateY(mYaw);
    mRight.normalize();
}

void Camera::updateForwardVector()
{
    mForward = gsl::vec3{0.f, 0.f, -1.f};       //gsl::FORWARD;
    mForward.rotateX(mPitch);
    mForward.rotateY(mYaw);     //Rotate around world up
    mForward.normalize();

    mUp = mRight^mForward;
}

void Camera::update()
{
    mYawMatrix.setToIdentity();
    mPitchMatrix.setToIdentity();

    mPitchMatrix.rotateX(mPitch);
    mYawMatrix.rotateY(mYaw);

    mPosition -= mForward * mSpeed;

    mViewMatrix = mPitchMatrix* mYawMatrix;
    mViewMatrix.translate(-mPosition);
}

void Camera::setPosition(const gsl::vec3 &position)
{
    mPosition = position;
}

void Camera::setSpeed(float speed)
{
    mSpeed = speed;
}

void Camera::updateHeigth(float deltaHeigth)
{
    mPosition.y += deltaHeigth;
}

void Camera::moveRight(float delta)
{
    mPosition += mRight * delta;
}

gsl::vec3 Camera::position() const
{
    return mPosition;
}

gsl::vec3 Camera::up() const
{
    return mUp;
}
