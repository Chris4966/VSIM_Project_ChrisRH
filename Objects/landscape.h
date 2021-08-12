#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include "visualobject.h"

class Landscape : public VisualObject
{
public:
    Landscape(std::string fileName);
    ~Landscape() override = default;

    void init() override;
    void draw() override;

    void readFile();
    std::string mFileName;

    void Triangulate();
};

#endif // LANDSCAPE_H
