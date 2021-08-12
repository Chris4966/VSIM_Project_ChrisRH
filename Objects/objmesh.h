#ifndef OBJMESH_H
#define OBJMESH_H
#include "visualobject.h"


class ObjMesh : public VisualObject
{
public:
    ObjMesh(std::string filename);
    ~ObjMesh() override = default;

    virtual void draw() override;
    virtual void init() override;

    std::string mObjFilename;
private:
    void readFile();
};

#endif // OBJMESH_H
