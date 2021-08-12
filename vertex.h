#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include "vec3.h"
#include "vec2.h"
#include "gltypes.h"
#include <iostream>
#include <fstream>

class Vertex {
public:
    Vertex();
    Vertex(float x, float y, float z, float r, float g, float b, float s = 0.f, float t = 0.f);
    Vertex(gsl::vec3 a, gsl::vec3 b, gsl::vec2 c);
    ~Vertex() = default;

    //! Overloaded ostream operator which writes all vertex data on an open textfile stream
    friend std::ostream& operator<< (std::ostream&, const Vertex&);

    //! Overloaded ostream operator which reads all vertex data from an open textfile stream
    friend std::istream& operator>> (std::istream&, Vertex&);

    void set_xyz(GLfloat *xyz);
    void set_xyz(GLfloat x, GLfloat y, GLfloat z);
    void set_xyz(gsl::vec3 xyz_in);
    void set_rgb(GLfloat *rgb);
    void set_rgb(GLfloat r, GLfloat g, GLfloat b);
    void set_normal(GLfloat *normal);
    void set_normal(GLfloat x, GLfloat y, GLfloat z);
    void set_normal(gsl::vec3 normal_in);
    void set_st(GLfloat *st);
    void set_st(GLfloat s, GLfloat t);
    void set_uv(GLfloat u, GLfloat v);


    gsl::vec3 mXYZ{0.f};
    gsl::vec3 mNormal{0.f};
    gsl::vec2 mST{0.f};

    void setX(float x)
    {
        mXYZ.x = x;
    }
    void setY(float y)
    {
        mXYZ.y = y;
    }
    void setZ(float z)
    {
        mXYZ.z = z;
    }

    float getX()
    {
        return mXYZ.x;
    }
    float getY()
    {
        return mXYZ.y;
    }
    float getZ()
    {
        return mXYZ.z;
    }
};

#endif // VERTEX_H
