#include "mat4x4.h"
//#include "quaternion.h"
#include "math_constants.h"
#include "mat3x3.h"

#include <cmath>
#include <cstring> // For memcpy on linux
#include <utility>

namespace gsl
{
mat4::mat4(bool isIdentity)
{
    if(isIdentity)
    {
        identity();
    }
    else
    {
        for(int i = 0; i < 16; i++)
            matrix[i] = 0.f;
    }
}

mat4::mat4(std::initializer_list<GLfloat> values)
{
    //Initializing the matrix class the same way as a 2d array
    int i = 0;
    for(auto value : values)
        matrix[i++] = value;
}

mat4 mat4::identity()
{
    setToIdentity();

    return *this;
}

void mat4::setToIdentity()
{
    *this =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

bool mat4::inverse()
{
    GLfloat inv[16];
    GLfloat det;
    GLfloat invOut[16];

    int i;

    inv[0] = matrix[5]  * matrix[10] * matrix[15] -
            matrix[5]  * matrix[11] * matrix[14] -
            matrix[9]  * matrix[6]  * matrix[15] +
            matrix[9]  * matrix[7]  * matrix[14] +
            matrix[13] * matrix[6]  * matrix[11] -
            matrix[13] * matrix[7]  * matrix[10];

    inv[4] = -matrix[4]  * matrix[10] * matrix[15] +
            matrix[4]  * matrix[11] * matrix[14] +
            matrix[8]  * matrix[6]  * matrix[15] -
            matrix[8]  * matrix[7]  * matrix[14] -
            matrix[12] * matrix[6]  * matrix[11] +
            matrix[12] * matrix[7]  * matrix[10];

    inv[8] = matrix[4]  * matrix[9] * matrix[15] -
            matrix[4]  * matrix[11] * matrix[13] -
            matrix[8]  * matrix[5] * matrix[15] +
            matrix[8]  * matrix[7] * matrix[13] +
            matrix[12] * matrix[5] * matrix[11] -
            matrix[12] * matrix[7] * matrix[9];

    inv[12] = -matrix[4]  * matrix[9] * matrix[14] +
            matrix[4]  * matrix[10] * matrix[13] +
            matrix[8]  * matrix[5] * matrix[14] -
            matrix[8]  * matrix[6] * matrix[13] -
            matrix[12] * matrix[5] * matrix[10] +
            matrix[12] * matrix[6] * matrix[9];

    inv[1] = -matrix[1]  * matrix[10] * matrix[15] +
            matrix[1]  * matrix[11] * matrix[14] +
            matrix[9]  * matrix[2] * matrix[15] -
            matrix[9]  * matrix[3] * matrix[14] -
            matrix[13] * matrix[2] * matrix[11] +
            matrix[13] * matrix[3] * matrix[10];

    inv[5] = matrix[0]  * matrix[10] * matrix[15] -
            matrix[0]  * matrix[11] * matrix[14] -
            matrix[8]  * matrix[2] * matrix[15] +
            matrix[8]  * matrix[3] * matrix[14] +
            matrix[12] * matrix[2] * matrix[11] -
            matrix[12] * matrix[3] * matrix[10];

    inv[9] = -matrix[0]  * matrix[9] * matrix[15] +
            matrix[0]  * matrix[11] * matrix[13] +
            matrix[8]  * matrix[1] * matrix[15] -
            matrix[8]  * matrix[3] * matrix[13] -
            matrix[12] * matrix[1] * matrix[11] +
            matrix[12] * matrix[3] * matrix[9];

    inv[13] = matrix[0]  * matrix[9] * matrix[14] -
            matrix[0]  * matrix[10] * matrix[13] -
            matrix[8]  * matrix[1] * matrix[14] +
            matrix[8]  * matrix[2] * matrix[13] +
            matrix[12] * matrix[1] * matrix[10] -
            matrix[12] * matrix[2] * matrix[9];

    inv[2] = matrix[1]  * matrix[6] * matrix[15] -
            matrix[1]  * matrix[7] * matrix[14] -
            matrix[5]  * matrix[2] * matrix[15] +
            matrix[5]  * matrix[3] * matrix[14] +
            matrix[13] * matrix[2] * matrix[7] -
            matrix[13] * matrix[3] * matrix[6];

    inv[6] = -matrix[0]  * matrix[6] * matrix[15] +
            matrix[0]  * matrix[7] * matrix[14] +
            matrix[4]  * matrix[2] * matrix[15] -
            matrix[4]  * matrix[3] * matrix[14] -
            matrix[12] * matrix[2] * matrix[7] +
            matrix[12] * matrix[3] * matrix[6];

    inv[10] = matrix[0]  * matrix[5] * matrix[15] -
            matrix[0]  * matrix[7] * matrix[13] -
            matrix[4]  * matrix[1] * matrix[15] +
            matrix[4]  * matrix[3] * matrix[13] +
            matrix[12] * matrix[1] * matrix[7] -
            matrix[12] * matrix[3] * matrix[5];

    inv[14] = -matrix[0]  * matrix[5] * matrix[14] +
            matrix[0]  * matrix[6] * matrix[13] +
            matrix[4]  * matrix[1] * matrix[14] -
            matrix[4]  * matrix[2] * matrix[13] -
            matrix[12] * matrix[1] * matrix[6] +
            matrix[12] * matrix[2] * matrix[5];

    inv[3] = -matrix[1] * matrix[6] * matrix[11] +
            matrix[1] * matrix[7] * matrix[10] +
            matrix[5] * matrix[2] * matrix[11] -
            matrix[5] * matrix[3] * matrix[10] -
            matrix[9] * matrix[2] * matrix[7] +
            matrix[9] * matrix[3] * matrix[6];

    inv[7] = matrix[0] * matrix[6] * matrix[11] -
            matrix[0] * matrix[7] * matrix[10] -
            matrix[4] * matrix[2] * matrix[11] +
            matrix[4] * matrix[3] * matrix[10] +
            matrix[8] * matrix[2] * matrix[7] -
            matrix[8] * matrix[3] * matrix[6];

    inv[11] = -matrix[0] * matrix[5] * matrix[11] +
            matrix[0] * matrix[7] * matrix[9] +
            matrix[4] * matrix[1] * matrix[11] -
            matrix[4] * matrix[3] * matrix[9] -
            matrix[8] * matrix[1] * matrix[7] +
            matrix[8] * matrix[3] * matrix[5];

    inv[15] = matrix[0] * matrix[5] * matrix[10] -
            matrix[0] * matrix[6] * matrix[9] -
            matrix[4] * matrix[1] * matrix[10] +
            matrix[4] * matrix[2] * matrix[9] +
            matrix[8] * matrix[1] * matrix[6] -
            matrix[8] * matrix[2] * matrix[5];

    det = matrix[0] * inv[0] + matrix[1] * inv[4] + matrix[2] * inv[8] + matrix[3] * inv[12];

    if (det == 0.f)
        return false;

    det = 1.f / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    memcpy(matrix, invOut, 16*sizeof(GLfloat));

    return true;
}

void mat4::translateX(GLfloat x)
{
    translate(x, 0.f, 0.f);
}

void mat4::translateY(GLfloat y)
{
    translate(0.f, y, 0.f);
}


void mat4::translateZ(GLfloat z)
{
    translate(0.f, 0.f, z);
}

void mat4::setPosition(GLfloat x, GLfloat y, GLfloat z)
{
    matrix[3] = x;
    matrix[7] = y;
    matrix[11] = z;
}

vec3 mat4::getPosition()
{
    return gsl::vec3(matrix[3], matrix[7], matrix[11]);
}

void mat4::rotateX(GLfloat degrees)
{
    GLfloat rad = deg2radf(degrees);

    mat4 temp =
    {
        1.f,   0.f,       0.f,    0.f,
        0.f, std::cos(rad),  std::sin(rad), 0.f,
        0.f, -std::sin(rad), std::cos(rad), 0.f,
        0.f,   0.f,       0.f,    1.f
    };

    *this = (*this)*temp;
}

void mat4::rotateY(GLfloat degrees)
{
    GLfloat rad = deg2radf(degrees);

    mat4 temp =
    {
        std::cos(rad), 0.f, -std::sin(rad), 0.f,
        0.f,       1.f,      0.f,       0.f,
        std::sin(rad), 0.f,  std::cos(rad), 0.f,
        0.f,       0.f,      0.f,       1.f
    };

    *this = (*this)*temp;
}

void mat4::rotateZ(GLfloat degrees)
{
    GLfloat rad = deg2radf(degrees);

    mat4 temp =
    {
        std::cos(rad),  std::sin(rad), 0.f, 0.f,
        -std::sin(rad),  std::cos(rad), 0.f, 0.f,
        0.f,            0.f,       1.f, 0.f,
        0.f,            0.f,       0.f, 1.f
    };

    *this = (*this)*temp;
}

// Rotate around a given vector
//void Matrix4x4::rotate(GLfloat angle, Vector3D vector)
//{
//    vector.normalize();

    //    https://learnopengl.com/Getting-started/Transformations
    //    cosθ+Rx2(1−cosθ)      RxRy(1−cosθ)−Rzsinθ     RxRz(1−cosθ)+Rysinθ     0
    //    RyRx(1−cosθ)+Rzsinθ   cosθ+Ry2(1−cosθ)        RyRz(1−cosθ)−Rxsinθ     0
    //    RzRx(1−cosθ)−Rysinθ   RzRy(1−cosθ)+Rxsinθ     cosθ+Rz2(1−cosθ)        0
    //    0                     0                       0                       1
//}

//void Matrix4x4::rotate(GLfloat angle, GLfloat xIn, GLfloat yIn, GLfloat zIn)
//{
//    rotate(angle, Vector3D(xIn, yIn, zIn));
//}

void mat4::scale(vec3 s)
{
    scale(s.x, s.y, s.z);
}

void mat4::scale(GLfloat uniformScale)
{
    scale(uniformScale, uniformScale, uniformScale);
}

void mat4::scale(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ)
{
    mat4 temp =
    {
        scaleX,  0.f,    0.f,   0.f,
        0.f,   scaleY,  0.f,   0.f,
        0.f,    0.f,   scaleZ, 0.f,
        0.f,    0.f,    0.f,   1.f
    };

    *this = (*this)*temp;
}

vec3 mat4::getScale()
{
    return vec3(matrix[0], matrix[5], matrix[10]);
}

GLfloat *mat4::constData()
{
    return &matrix[0];
}

void mat4::transpose()
{
    std::swap(matrix[1], matrix[4]);
    std::swap(matrix[2], matrix[8]);
    std::swap(matrix[3], matrix[12]);

    std::swap(matrix[6], matrix[9]);
    std::swap(matrix[7], matrix[13]);
    std::swap(matrix[11], matrix[14]);
}

void mat4::ortho(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat nearPlane, GLfloat farPlane)
{
    *this =
    {
        2.f/(r-l), 0.f, 0.f, 0.f,
                0.f, 2.f/(t-b), 0.f, 0.f,
                0.f, 0.f, -2.f/(farPlane-nearPlane), 0.f,
                -(r+l)/(r-l), -(t+b)/(t-b), -(farPlane+nearPlane)/(farPlane-nearPlane), 1.f
    };
}

//From Interactive Computer Graphics ch. 5
void mat4::frustum(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
    *this =
    {
        2*nearPlane/(right-left),    0.f,                    (right+left)/(right-left),                      0.f,
                0.f,              2*nearPlane/(top-bottom),  (top+bottom)/(top-bottom),                      0.f,
                0.f,                 0.f,                    -(farPlane+nearPlane)/(farPlane-nearPlane),    -2*farPlane*nearPlane/(farPlane-nearPlane),
                0.f,                 0.f,                               -1.0f,                               0.0f
    };
}

void mat4::perspective(GLfloat fieldOfView, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane)
{
    /* General form of the Projection Matrix
    //
    // uh = Cot( fov/2 ) == 1/Tan(fov/2)
    // uw / uh = 1/aspect
    //
    //   uw         0       0       0
    //    0        uh       0       0
    //    0         0      f/(f-n)  1
    //    0         0    -fn/(f-n)  0 */

    //Checking numbers for no division on zero:
    if (fieldOfView <= 0.f)
        fieldOfView = 30.f;
    if (aspectRatio <= 0.f)
        aspectRatio = 1.3f;
    if (farPlane - nearPlane <= 0.f)
    {
        nearPlane = 1.f;
        farPlane = 100.f;
    }

    GLfloat uh = static_cast<float>(1/std::tan(gsl::deg2rad(static_cast<double>(fieldOfView)/2)));
    GLfloat uw = (1/aspectRatio) * uh;

    *this =
    {
        uw,     0.f,    0.f,                                        0.f,
        0.f,    uh,     0.f,                                        0.f,
        0.f,    0.f,    -(farPlane)/(farPlane-nearPlane),    -2 * farPlane*nearPlane/(farPlane-nearPlane),
        0.f,    0.f,    -1.f,                                        0.f
    };



    /*
           //fieldOfView = verticalAngle
           //Find right, and calculate the rest from there
            GLfloat scale = std::tan(verticalAngle * PI / 360.f) * nearPlane;
            GLfloat r = aspectRatio * scale;
            GLfloat t = scale;

            //Create perspective-frustrum
            *this =
            {
                nearPlane/r, 0.f, 0.f, 0.f,
                0.f, nearPlane/t, 0.f, 0.f,
                0.f, 0.f, -(farPlane+nearPlane)/(farPlane-nearPlane), -2*farPlane*nearPlane/(farPlane-nearPlane),
                0.f, 0.f, -1.f, 0.f
            };
    */
}

void mat4::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up_axis)
{
    vec3 f = center-eye;    //forward
    f.normalize();
    vec3 s = vec3::cross(f, up_axis);   //sideways
    s.normalize();
    vec3 u = vec3::cross(s, f);     //up

    *this =
    {
        s.x,  s.y,  s.z, -vec3::dot(s, eye),
                u.x,  u.y,  u.z, -vec3::dot(u, eye),
                -f.x, -f.y, -f.z, vec3::dot(f, eye),
                0.f, 0.f, 0.f, 1.f
    };
}

void mat4::setRotationToVector(const vec3 &direction, vec3 up)
{
    vec3 xaxis = vec3::cross(up, direction);
    xaxis.normalize();

    vec3 yaxis = vec3::cross(direction, xaxis);
    yaxis.normalize();

    matrix[0] = xaxis.x;
    matrix[1] = yaxis.x;
    matrix[2] = direction.x;

    matrix[4] = xaxis.y;
    matrix[5] = yaxis.y;
    matrix[6] = direction.y;

    matrix[8] = xaxis.z;
    matrix[9] = yaxis.z;
    matrix[10] = direction.z;
}

void mat4::translate(GLfloat x, GLfloat y, GLfloat z)
{
    mat4 mat =
    {
        1.f, 0.f, 0.f, x,
        0.f, 1.f, 0.f, y,
        0.f, 0.f, 1.f, z,
        0.f, 0.f, 0.f, 1.f
    };

    *this = (*this)*mat;
}

void mat4::translate(vec3 positionIn)
{
    mat4 mat =
    {
        1.f, 0.f, 0.f, positionIn.x,
        0.f, 1.f, 0.f, positionIn.y,
        0.f, 0.f, 1.f, positionIn.z,
        0.f, 0.f, 0.f, 1.f
    };

    *this = (*this)*mat;
}

mat3 mat4::toMatrix3() const
{
    return
    {
        matrix[0], matrix[1], matrix[2],
        matrix[4], matrix[5], matrix[6],
        matrix[8], matrix[9], matrix[10]
    };
}

GLfloat& mat4::operator()(const int &y, const int &x)
{
    return matrix[y * 4 + x];
}

GLfloat mat4::operator()(const int &y, const int &x) const
{
    return matrix[y * 4 + x];
}

mat4 mat4::operator*(const mat4 &other)
{
    return
    {
        matrix[0]  * other.matrix[0] + matrix[1]  * other.matrix[4] + matrix[2]  * other.matrix[8]  + matrix[3]  * other.matrix[12],
        matrix[0]  * other.matrix[1] + matrix[1]  * other.matrix[5] + matrix[2]  * other.matrix[9]  + matrix[3]  * other.matrix[13],
        matrix[0]  * other.matrix[2] + matrix[1]  * other.matrix[6] + matrix[2]  * other.matrix[10] + matrix[3]  * other.matrix[14],
        matrix[0]  * other.matrix[3] + matrix[1]  * other.matrix[7] + matrix[2]  * other.matrix[11] + matrix[3]  * other.matrix[15],

        matrix[4]  * other.matrix[0] + matrix[5]  * other.matrix[4] + matrix[6]  * other.matrix[8]  + matrix[7]  * other.matrix[12],
        matrix[4]  * other.matrix[1] + matrix[5]  * other.matrix[5] + matrix[6]  * other.matrix[9]  + matrix[7]  * other.matrix[13],
        matrix[4]  * other.matrix[2] + matrix[5]  * other.matrix[6] + matrix[6]  * other.matrix[10] + matrix[7]  * other.matrix[14],
        matrix[4]  * other.matrix[3] + matrix[5]  * other.matrix[7] + matrix[6]  * other.matrix[11] + matrix[7]  * other.matrix[15],

        matrix[8]  * other.matrix[0] + matrix[9]  * other.matrix[4] + matrix[10] * other.matrix[8]  + matrix[11] * other.matrix[12],
        matrix[8]  * other.matrix[1] + matrix[9]  * other.matrix[5] + matrix[10] * other.matrix[9]  + matrix[11] * other.matrix[13],
        matrix[8]  * other.matrix[2] + matrix[9]  * other.matrix[6] + matrix[10] * other.matrix[10] + matrix[11] * other.matrix[14],
        matrix[8]  * other.matrix[3] + matrix[9]  * other.matrix[7] + matrix[10] * other.matrix[11] + matrix[11] * other.matrix[15],

        matrix[12] * other.matrix[0] + matrix[13] * other.matrix[4] + matrix[14] * other.matrix[8]  + matrix[15] * other.matrix[12],
        matrix[12] * other.matrix[1] + matrix[13] * other.matrix[5] + matrix[14] * other.matrix[9]  + matrix[15] * other.matrix[13],
        matrix[12] * other.matrix[2] + matrix[13] * other.matrix[6] + matrix[14] * other.matrix[10] + matrix[15] * other.matrix[14],
        matrix[12] * other.matrix[3] + matrix[13] * other.matrix[7] + matrix[14] * other.matrix[11] + matrix[15] * other.matrix[15]
    };
}

GLfloat mat4::getFloat(int space)
{
    return matrix[space];
}

} //namespace
