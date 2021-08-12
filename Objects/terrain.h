#ifndef GROUND_H
#define GROUND_H

#include "visualobject.h"

class Texture;

class Terrain : public VisualObject
{
public:
    Terrain(Texture *texture, float horisontalSpacing = 1.f, float verticalSpacing = 0.5f, float heightPlacement = 0.f);

    void makeTerrain();
    ~Terrain() override;

    void init() override;
    void draw() override;

    void calculateNormals();

    class Texture *mTexture{nullptr};
    unsigned short mWidth{0};
    unsigned short mDepth{0};
    float mHorisontalSpacing{1.f};
    float mHeightSpacing{1.f};
    float mHeightPlacement{0.f};

    float vertexXStart{0.f};
    float vertexZStart{0.f};

    float calculateHeight(const gsl::vec3 &positionIn);
};

#endif // GROUND_H
