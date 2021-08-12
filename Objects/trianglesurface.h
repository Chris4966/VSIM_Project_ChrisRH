#ifndef TRIANGLESURFACE_H
#define TRIANGLESURFACE_H

#include "visualobject.h"
#include <QOpenGLFunctions_4_1_Core>

class TriangleSurface : public VisualObject
{
public:
    TriangleSurface(std::string fileName);
    ~TriangleSurface() override = default;

    void init() override;
    void draw() override;

    void readFile();
    std::string mFileName;
};

#endif // TRIANGLESURFACE_H
