#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "gltypes.h"

class RenderWindow;

class Scene
{
public:
    Scene(RenderWindow *renderWindow);

    void init();
    void update();
    void render();
    void renderNormals(GLint matrixUniform);
    void updateColliders();

    std::vector<class VisualObject*> mVisualObjects;
    RenderWindow *mRenderWindow{nullptr};
    class Light *mLight{nullptr};
    class Terrain *mTerrain{nullptr};
};

#endif // SCENE_H
