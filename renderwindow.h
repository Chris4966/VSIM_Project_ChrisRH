#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QWindow>
#include <QOpenGLFunctions_4_1_Core>
#include <QTimer>
#include <QElapsedTimer>

#include "input.h"
#include "constants.h"
#include "mat4x4.h"

class QOpenGLContext;
class MainWindow;
class VisualObject;

// This inherits from QWindow to get access to the Qt functionality and
// OpenGL surface.
// We also inherit from QOpenGLFunctions, to get access to the OpenGL functions
// This is the same as using glad and glw from general OpenGL tutorials
class RenderWindow : public QWindow, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT
public:
    RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow);
    ~RenderWindow() override = default;

    QOpenGLContext *context() { return mContext; }

    void exposeEvent(QExposeEvent *) override;

    void toggleWireframe(bool buttonState);
    void reset();
    void playPause();

    bool mRenderEditorGrid{true};
    bool mRenderEditorAxis{true};
    bool mRenderCornerGizmo{true};

    void checkForGLerrors();

    class Camera *mCurrentCamera{nullptr};
    class Material *mMaterial[gsl::NumberOfMaterials]{nullptr};
    class Texture *mTexture[gsl::NumberOfTextures]{nullptr};
    VisualObject* mPlayer{nullptr};  //the interactive object - input works on this
    VisualObject* mEnemy{nullptr};
    std::vector<VisualObject*> mBalls;
    std::vector<VisualObject*> mSurfaces;
    std::vector<VisualObject*> mTrophies;
    std::vector<class BSplineCurve*> mCurves;
    class Scene *mScene{nullptr};
    bool mDrawNormals{false};
    unsigned int mSelectedScene{0};
    unsigned int mSelectedCurve{0};

    void NPCmovement();
    void followTerrain(VisualObject &a);

private slots:
    void render();

private:
    void init();

    void calculateFramerate();
    void startOpenGLDebugger();
    void setCameraSpeed(float value);
    void handleInput();

    gsl::mat4* mIdentityMatrix{nullptr};

    int mTextureUnits;      //Supported Texture Units (slots) pr shader.

    class Shader *mShaderProgram[gsl::NumberOfShaders]{nullptr};

//    class Light * mLight{nullptr};

    float mAspectratio{1.f};

    VisualObject * mEditorAssets[gsl::EditorAssetNumber]{nullptr};
    Input mInput;
    float mCameraSpeed{0.05f};
    float mCameraRotateSpeed{0.1f};
    int mMouseXlast{0};
    int mMouseYlast{0};

    QOpenGLContext *mContext{nullptr};
    bool mInitialized;

    QTimer *mRenderTimer{nullptr};           //timer that drives the gameloop
    QElapsedTimer mTimeStart;       //time variable that reads the calculated FPS

    MainWindow *mMainWindow{nullptr};        //points back to MainWindow to be able to put info in StatusBar

    class QOpenGLDebugLogger *mOpenGLDebugLogger{nullptr};

    void shaderAndMaterialSetup();
    void renderEditorAssets();

protected:
    //The QWindow that we inherit from has these functions to capture mouse and keyboard.
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
};

#endif // RENDERWINDOW_H
