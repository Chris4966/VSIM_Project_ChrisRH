#include "renderwindow.h"

//General Qt stuff
#include <QTimer>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLDebugLogger>
#include <QKeyEvent>
#include <QStatusBar>
#include <QDebug>
#include <QGuiApplication>

#include <iostream>
#include <cmath>
#include <cassert>

//Essentials
#include "mainwindow.h"
#include "visualobject.h"
#include "camera.h"
#include "light.h"
#include "scene.h"
#include "texture.h"
#include "constants.h"
#include "material.h"
#include "math_constants.h"

//Objects
#include "xyz.h"
#include "objmesh.h"
#include "gizmo.h"
#include "bsplinecurve.h"
#include "editorgrid.h"
#include "terrain.h"
#include "quad.h"
#include "rollingball.h"

//Shaders
#include "shader.h"
#include "phongshader.h"
#include "textureshader.h"
#include "colorshader.h"
#include "normalsshader.h"
#include "skyboxshader.h"

RenderWindow::RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow)
    : mContext(nullptr), mInitialized(false), mMainWindow(mainWindow)
{
    //This is sent to QWindow:
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);
    //Make the OpenGL context
    mContext = new QOpenGLContext(this);
    //Give the context the wanted OpenGL format (v4.1 Core)
    mContext->setFormat(requestedFormat());
    if (!mContext->create()) {
        delete mContext;
        mContext = nullptr;
        qDebug() << "Context could not be made - quitting this application";
    }

    //Make the gameloop timer:
    mRenderTimer = new QTimer(this);
}

// Sets up the general OpenGL stuff and the buffers needed to render a triangle
void RenderWindow::init()
{
    //Connect the gameloop timer to the render function:
    //This makes our render loop
    connect(mRenderTimer, SIGNAL(timeout()), this, SLOT(render()));
    //********************** General OpenGL stuff **********************

    //The OpenGL context has to be set.
    //The context belongs to the instanse of this class!
    if (!mContext->makeCurrent(this)) {
        qDebug() << "makeCurrent() failed";
        return;
    }

    //just to make sure we don't init several times
    //used in exposeEvent()
    if (!mInitialized)
        mInitialized = true;

    //must call this to use OpenGL functions
    initializeOpenGLFunctions();

    //Print render version info (what GPU is used):
    //(Have to use cout to see text- qDebug just writes numbers...)
    //Nice to see if you use the Intel GPU or the dedicated GPU on your laptop
    // - can be deleted
    std::cout << "The active GPU and API: \n";
    std::cout << "  Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "  Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "  Version: " << glGetString(GL_VERSION) << std::endl;
    int textureUnits;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &textureUnits);
    std::cout << "This GPU as " << textureUnits << " texture units / slots in total, ";
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &mTextureUnits);
    std::cout << "and supports " << mTextureUnits << " texture units pr shader" << std::endl;

    //Start the Qt OpenGL debugger
    //Really helpfull when doing OpenGL
    //Supported on most Windows machines
    //reverts to plain glGetError() on Mac and other unsupported PCs
    // - can be deleted
    startOpenGLDebugger();

    //general OpenGL stuff:
    glEnable(GL_DEPTH_TEST);            //enables depth sorting - must then use GL_DEPTH_BUFFER_BIT in glClear
    glEnable(GL_CULL_FACE);       //draws only front side of models - usually what you want - test it out!
    glClearColor(0.3f, 0.3f, 0.3f,1.0f);    //gray color used in glClear GL_COLOR_BUFFER_BIT

    //set up alpha blending for textures
    glEnable(GL_BLEND);// you enable blending function
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Compile shaders and set up materials:
    shaderAndMaterialSetup();

    //********************* Making the editor to be drawn **********************

    mIdentityMatrix = new gsl::mat4();
    mIdentityMatrix->identity();
    VisualObject *temp = new Gizmo();                           // 0
    temp->init();
    temp->mMaterial = mMaterial[gsl::COLORSHADER];
    mEditorAssets[gsl::GIZMO] = temp;

    temp = new EditorGrid(20, 1);                               // 1
    temp->init();
    mEditorAssets[gsl::GRID] = temp;

    temp = new XYZ();                                           // 2
    temp->init();
    mEditorAssets[gsl::AXIS] = temp;

    //********************** Set up scenes **********************
    mScene = new Scene(this);
    mScene->init();

    VisualObject *Ball = new RollingBall(3, mSurfaces[0]);
    Ball->mMatrix.translate(1.f, 5.f, 10.f);
    Ball->init();
    mBalls.push_back(Ball);

    Ball = new RollingBall(3, mSurfaces[1]);
    Ball->mMatrix.translate(5010.f, 72.f, 10015.f);
    Ball->init();
    mBalls.push_back(Ball);

    /*
    Ball = new RollingBall(3, mSurfaces[1]);
    Ball->mMatrix.translate(5015.f, 72.f, 10010.f);
    Ball->init();
    mBalls.push_back(Ball);

    Ball = new RollingBall(3, mSurfaces[1]);
    Ball->mMatrix.translate(5010.f, 72.f, 10010.f);
    Ball->init();
    mBalls.push_back(Ball);
    */

    for (unsigned int i{0}; i < mBalls.size(); i++)
        mBalls[i]->mInitPos = mBalls[i]->mMatrix.getPosition();


    //B-Splines
    std::vector<gsl::vec3> controls1;
    std::vector<float> knots1;

    controls1.push_back(gsl::vec3{0.f, 0.f, 0.f});
    controls1.push_back(mTrophies[0]->mMatrix.getPosition());
    controls1.push_back(mTrophies[1]->mMatrix.getPosition());
    controls1.push_back(mTrophies[2]->mMatrix.getPosition());
    controls1.push_back(gsl::vec3{10.f, 0.f, 20.f});

    knots1.push_back(0);
    knots1.push_back(0);
    knots1.push_back(0);
    knots1.push_back(0);
    knots1.push_back(0.5);
    knots1.push_back(1);
    knots1.push_back(1);
    knots1.push_back(1);
    knots1.push_back(1);

    BSplineCurve *spline = new BSplineCurve(controls1, knots1, 3);
    spline->init();
    mCurves.push_back(spline);

    std::vector<gsl::vec3> controls2;
    std::vector<float> knots2;

    controls2.push_back(gsl::vec3{0.f, 0.f, 0.f});
    controls2.push_back(mTrophies[1]->mMatrix.getPosition());
    controls2.push_back(mTrophies[2]->mMatrix.getPosition());
    controls2.push_back(gsl::vec3{10.f, 0.f, 20.f});

    knots2.push_back(0);
    knots2.push_back(0);
    knots2.push_back(0);
    knots2.push_back(0.5);
    knots2.push_back(1);
    knots2.push_back(1);
    knots2.push_back(1);

    spline = new BSplineCurve(controls2, knots2, 2);
    spline->init();
    mCurves.push_back(spline);

    std::vector<gsl::vec3> controls3;
    std::vector<float> knots3;

    controls3.push_back(gsl::vec3{0.f, 0.f, 0.f});
    controls3.push_back(mTrophies[0]->mMatrix.getPosition());
    controls3.push_back(mTrophies[2]->mMatrix.getPosition());
    controls3.push_back(gsl::vec3{10.f, 0.f, 20.f});

    knots3.push_back(0);
    knots3.push_back(0);
    knots3.push_back(0);
    knots3.push_back(0.5);
    knots3.push_back(1);
    knots3.push_back(1);
    knots3.push_back(1);

    spline = new BSplineCurve(controls3, knots3, 2);
    spline->init();
    mCurves.push_back(spline);

    std::vector<gsl::vec3> controls4;
    std::vector<float> knots4;

    controls4.push_back(gsl::vec3{0.f, 0.f, 0.f});
    controls4.push_back(mTrophies[0]->mMatrix.getPosition());
    controls4.push_back(mTrophies[1]->mMatrix.getPosition());
    controls4.push_back(gsl::vec3{10.f, 0.f, 20.f});

    knots4.push_back(0);
    knots4.push_back(0);
    knots4.push_back(0);
    knots4.push_back(0.5);
    knots4.push_back(1);
    knots4.push_back(1);
    knots4.push_back(1);

    spline = new BSplineCurve(controls4, knots4, 2);
    spline->init();
    mCurves.push_back(spline);

    std::vector<gsl::vec3> controls5;
    std::vector<float> knots5;

    controls5.push_back(gsl::vec3{0.f, 0.f, 0.f});
    controls5.push_back(mTrophies[2]->mMatrix.getPosition());
    controls5.push_back(gsl::vec3{10.f, 0.f, 20.f});
    controls5.push_back(gsl::vec3{0.f, 0.f, 0.f});

    knots5.push_back(0);
    knots5.push_back(0);
    knots5.push_back(0);
    knots5.push_back(0.5);
    knots5.push_back(1);
    knots5.push_back(1);
    knots5.push_back(1);

    spline = new BSplineCurve(controls5, knots5, 2);
    spline->init();
    mCurves.push_back(spline);

    std::vector<gsl::vec3> controls6;
    std::vector<float> knots6;

    controls6.push_back(gsl::vec3{0.f, 0.f, 0.f});
    controls6.push_back(mTrophies[1]->mMatrix.getPosition());
    controls6.push_back(gsl::vec3{10.f, 0.f, 20.f});
    controls6.push_back(gsl::vec3{0.f, 0.f, 0.f});

    knots6.push_back(0);
    knots6.push_back(0);
    knots6.push_back(0);
    knots6.push_back(0.5);
    knots6.push_back(1);
    knots6.push_back(1);
    knots6.push_back(1);

    spline = new BSplineCurve(controls6, knots6, 2);
    spline->init();
    mCurves.push_back(spline);

    std::vector<gsl::vec3> controls7;
    std::vector<float> knots7;

    controls7.push_back(gsl::vec3{0.f, 0.f, 0.f});
    controls7.push_back(mTrophies[0]->mMatrix.getPosition());
    controls7.push_back(gsl::vec3{10.f, 0.f, 20.f});
    controls7.push_back(gsl::vec3{0.f, 0.f, 0.f});

    knots7.push_back(0);
    knots7.push_back(0);
    knots7.push_back(0);
    knots7.push_back(0.5);
    knots7.push_back(1);
    knots7.push_back(1);
    knots7.push_back(1);

    spline = new BSplineCurve(controls7, knots7, 2);
    spline->init();
    mCurves.push_back(spline);

    //alternate paths
    std::vector<gsl::vec3> controls8;
    std::vector<float> knots8;

    controls8.push_back(gsl::vec3{10.f, 0.f, 20.f});
    controls8.push_back(mTrophies[0]->mMatrix.getPosition());
    controls8.push_back(mTrophies[2]->mMatrix.getPosition());
    controls8.push_back(mTrophies[1]->mMatrix.getPosition());
    controls8.push_back(gsl::vec3{0.f, 0.f, 0.f});

    knots8.push_back(0);
    knots8.push_back(0);
    knots8.push_back(0);
    knots8.push_back(0);
    knots8.push_back(0.5);
    knots8.push_back(1);
    knots8.push_back(1);
    knots8.push_back(1);
    knots8.push_back(1);

    spline = new BSplineCurve(controls8, knots8, 3);
    spline->init();
    mCurves.push_back(spline);

    std::vector<gsl::vec3> controls9;
    std::vector<float> knots9;

    controls9.push_back(gsl::vec3{0.f, 0.f, 0.f});
    controls9.push_back(mTrophies[1]->mMatrix.getPosition());
    controls9.push_back(mTrophies[0]->mMatrix.getPosition());
    controls9.push_back(mTrophies[2]->mMatrix.getPosition());
    controls9.push_back(gsl::vec3{10.f, 0.f, 20.f});

    knots9.push_back(0);
    knots9.push_back(0);
    knots9.push_back(0);
    knots9.push_back(0);
    knots9.push_back(0.5);
    knots9.push_back(1);
    knots9.push_back(1);
    knots9.push_back(1);
    knots9.push_back(1);

    spline = new BSplineCurve(controls9, knots9, 3);
    spline->init();
    mCurves.push_back(spline);

    std::vector<gsl::vec3> controls10;
    std::vector<float> knots10;

    controls10.push_back(gsl::vec3{10.f, 0.f, 20.f});
    controls10.push_back(mTrophies[1]->mMatrix.getPosition());
    controls10.push_back(mTrophies[2]->mMatrix.getPosition());
    controls10.push_back(mTrophies[0]->mMatrix.getPosition());
    controls10.push_back(gsl::vec3{0.f, 0.f, 0.f});

    knots10.push_back(0);
    knots10.push_back(0);
    knots10.push_back(0);
    knots10.push_back(0);
    knots10.push_back(0.5);
    knots10.push_back(1);
    knots10.push_back(1);
    knots10.push_back(1);
    knots10.push_back(1);

    spline = new BSplineCurve(controls10, knots10, 3);
    spline->init();
    mCurves.push_back(spline);

    std::vector<gsl::vec3> controls11;
    std::vector<float> knots11;

    controls11.push_back(gsl::vec3{10.f, 0.f, 20.f});
    controls11.push_back(mTrophies[2]->mMatrix.getPosition());
    controls11.push_back(mTrophies[1]->mMatrix.getPosition());
    controls11.push_back(gsl::vec3{0.f, 0.f, 0.f});

    knots11.push_back(0);
    knots11.push_back(0);
    knots11.push_back(0);
    knots11.push_back(0.5);
    knots11.push_back(1);
    knots11.push_back(1);
    knots11.push_back(1);

    spline = new BSplineCurve(controls11, knots11, 2);
    spline->init();
    mCurves.push_back(spline);

    std::vector<gsl::vec3> controls12;
    std::vector<float> knots12;

    controls12.push_back(gsl::vec3{10.f, 0.f, 20.f});
    controls12.push_back(mTrophies[2]->mMatrix.getPosition());
    controls12.push_back(mTrophies[0]->mMatrix.getPosition());
    controls12.push_back(gsl::vec3{0.f, 0.f, 0.f});

    knots12.push_back(0);
    knots12.push_back(0);
    knots12.push_back(0);
    knots12.push_back(0.5);
    knots12.push_back(1);
    knots12.push_back(1);
    knots12.push_back(1);

    spline = new BSplineCurve(controls12, knots12, 2);
    spline->init();
    mCurves.push_back(spline);

    std::vector<gsl::vec3> controls13;
    std::vector<float> knots13;

    controls13.push_back(gsl::vec3{10.f, 0.f, 20.f});
    controls13.push_back(mTrophies[1]->mMatrix.getPosition());
    controls13.push_back(mTrophies[0]->mMatrix.getPosition());
    controls13.push_back(gsl::vec3{0.f, 0.f, 0.f});

    knots13.push_back(0);
    knots13.push_back(0);
    knots13.push_back(0);
    knots13.push_back(0.5);
    knots13.push_back(1);
    knots13.push_back(1);
    knots13.push_back(1);

    spline = new BSplineCurve(controls13, knots13, 2);
    spline->init();
    mCurves.push_back(spline);

    std::vector<gsl::vec3> controls14;
    std::vector<float> knots14;

    controls14.push_back(gsl::vec3{0.f, 0.f, 0.f});
    controls14.push_back(gsl::vec3{0.f, 0.f, 20.f});
    controls14.push_back(gsl::vec3{20.f, 0.f, 20.f});
    controls14.push_back(gsl::vec3{20.f, 0.f, 0.f});
    controls14.push_back(gsl::vec3{0.f, 0.f, 0.f});

    knots14.push_back(0);
    knots14.push_back(0);
    knots14.push_back(0);
    knots14.push_back(0);
    knots14.push_back(0.5);
    knots14.push_back(1);
    knots14.push_back(1);
    knots14.push_back(1);
    knots14.push_back(1);

    spline = new BSplineCurve(controls14, knots14, 3);
    spline->init();
    mCurves.push_back(spline);

    //********************** Set up camera **********************
    mCurrentCamera = new Camera();
    mCurrentCamera->setPosition(gsl::vec3(2.f, 3.5f, 12.f));
    mCurrentCamera->pitch(10.f);
    mCurrentCamera->yaw(-5.f);

    //Move these to a SetCurrentCamera-ish function?
    //ColorShader needs camera
    static_cast<ColorShader*>(mShaderProgram[gsl::COLORSHADER])->mCurrentCamera = mCurrentCamera;
    //TextureShader needs camera
    static_cast<TextureShader*>(mShaderProgram[gsl::TEXTURESHADER])->mCurrentCamera = mCurrentCamera;
    //Phongshader needs light and camera:
    static_cast<PhongShader*>(mShaderProgram[gsl::PHONGSHADER])->mCurrentLight = mScene->mLight;
    static_cast<PhongShader*>(mShaderProgram[gsl::PHONGSHADER])->mCurrentCamera = mCurrentCamera;
    //Skybox needs camera
    static_cast<SkyboxShader*>(mShaderProgram[gsl::SKYBOXSHADER])->mCurrentCamera = mCurrentCamera;

    //On Windows this is neccessary to start the rendering before we click into the program...
    mMainWindow->mRenderWindowContainer->setFocus();
}

// Called each frame - this is actually the game loop!
void RenderWindow::render()
{
    //Keyboard / mouse input
    handleInput();

    mCurrentCamera->update();
    mScene->update();

    mTimeStart.restart(); //restart FPS clock
    mContext->makeCurrent(this); //must be called every frame (every time mContext->swapBuffers is called)
    initializeOpenGLFunctions();    //must call this every frame it seems...

    //to clear the screen for each redraw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Render Editor assets:
    renderEditorAssets();

    mScene->render();

    if (mSelectedScene == 0)
    {
        glUniformMatrix4fv( mShaderProgram[gsl::COLORSHADER]->mMatrixUniform, 1, GL_TRUE,  mBalls[0]->mMatrix.constData());
        mBalls[0]->draw();
    }

    if (mSelectedScene == 1)
    {
        glUniformMatrix4fv( mShaderProgram[gsl::COLORSHADER]->mMatrixUniform, 1, GL_TRUE,  mBalls[1]->mMatrix.constData());
        mBalls[1]->draw();
        /*
        for (unsigned int i{1}; i < mBalls.size(); i++)
        {
            glUniformMatrix4fv( mShaderProgram[gsl::COLORSHADER]->mMatrixUniform, 1, GL_TRUE,  mBalls[i]->mMatrix.constData());
            mBalls[i]->draw();
        }
        */
    }

    if (mSelectedScene == 2)
    {
        glUniformMatrix4fv( mShaderProgram[gsl::COLORSHADER]->mMatrixUniform, 1, GL_TRUE,  mCurves[mSelectedCurve]->mMatrix.constData());
        mCurves[mSelectedCurve]->draw();
    }

    if(mDrawNormals)
    {
        glUseProgram(mShaderProgram[gsl::NORMALSSHADER]->getProgram());
        glUniformMatrix4fv( mShaderProgram[gsl::NORMALSSHADER]->vMatrixUniform, 1, GL_TRUE, mCurrentCamera->mViewMatrix.constData());
        glUniformMatrix4fv( mShaderProgram[gsl::NORMALSSHADER]->pMatrixUniform, 1, GL_TRUE, mCurrentCamera->mProjectionMatrix.constData());
        mScene->renderNormals(mShaderProgram[gsl::NORMALSSHADER]->mMatrixUniform);
    }

    glUseProgram(0);        //Nvidia card gives "shader in program ... recompiled based on GL state" warning without this

    //Calculate framerate before
    // checkForGLerrors() because that takes a long time
    // and before swapBuffers(), else it will show the vsync time
    calculateFramerate();

    //using our expanded OpenGL debugger to check if everything is OK.
    checkForGLerrors();

    //Qt require us to call this swapBuffers() -function.
    // swapInterval is 1 by default which means that swapBuffers() will (hopefully) block
    // and wait for vsync.

    mContext->swapBuffers(this);
}

void RenderWindow::renderEditorAssets()
{
    glUseProgram(mShaderProgram[gsl::COLORSHADER]->getProgram());     // all editor assets uses color shder

    glUniformMatrix4fv( mShaderProgram[gsl::COLORSHADER]->vMatrixUniform, 1, GL_TRUE, mCurrentCamera->mViewMatrix.constData());
    glUniformMatrix4fv( mShaderProgram[gsl::COLORSHADER]->pMatrixUniform, 1, GL_TRUE, mCurrentCamera->mProjectionMatrix.constData());

    if(mRenderEditorGrid)
    {
        glUniformMatrix4fv( mShaderProgram[gsl::COLORSHADER]->mMatrixUniform, 1, GL_TRUE, mEditorAssets[1]->mMatrix.constData());
        mEditorAssets[gsl::GRID]->draw();  //grid
    }
    if(mRenderEditorAxis)
    {
        glUniformMatrix4fv( mShaderProgram[gsl::COLORSHADER]->mMatrixUniform, 1, GL_TRUE, mEditorAssets[2]->mMatrix.constData());
        mEditorAssets[gsl::AXIS]->draw();  //XYZ
    }
    //Corner Gizmo
    if (mRenderCornerGizmo)
    {
        glUniformMatrix4fv( mShaderProgram[gsl::COLORSHADER]->vMatrixUniform, 1, GL_TRUE, mIdentityMatrix->constData()); //Not using viewMatrix
        glUniformMatrix4fv( mShaderProgram[gsl::COLORSHADER]->pMatrixUniform, 1, GL_TRUE, mIdentityMatrix->constData()); //Not using projectionMatrix
        mEditorAssets[gsl::GIZMO]->mMatrix.setToIdentity();
        mEditorAssets[gsl::GIZMO]->mMatrix.rotateX(mCurrentCamera->mPitch);
        mEditorAssets[gsl::GIZMO]->mMatrix.rotateY(mCurrentCamera->mYaw);
        mEditorAssets[gsl::GIZMO]->mMatrix.setPosition(-0.88f, -0.88f, 0.0f);
        mEditorAssets[gsl::GIZMO]->draw();  //gizmo
    }
}

void RenderWindow::shaderAndMaterialSetup()
{
    mShaderProgram[gsl::COLORSHADER] = new ColorShader("colorshader");
    mShaderProgram[gsl::COLORSHADER]->mRenderWindow = this;  //only for debugging
    mShaderProgram[gsl::COLORSHADER]->setupShader();

    mShaderProgram[gsl::TEXTURESHADER] = new TextureShader("textureshader");
    mShaderProgram[gsl::TEXTURESHADER]->mRenderWindow = this;  //only for debugging
    mShaderProgram[gsl::TEXTURESHADER]->setupShader();

    mShaderProgram[gsl::PHONGSHADER] = new PhongShader("phongshader");
    mShaderProgram[gsl::PHONGSHADER]->mRenderWindow = this; //only for debugging
    mShaderProgram[gsl::PHONGSHADER]->setupShader();

    mShaderProgram[gsl::NORMALSSHADER] = new NormalsShader("shownormalsshader");
    mShaderProgram[gsl::NORMALSSHADER]->mRenderWindow = this; //only for debugging
    mShaderProgram[gsl::NORMALSSHADER]->setupShader();

    mShaderProgram[gsl::SKYBOXSHADER] = new SkyboxShader("skyboxshader");
    mShaderProgram[gsl::SKYBOXSHADER]->mRenderWindow = this; //only for debugging
    mShaderProgram[gsl::SKYBOXSHADER]->setupShader();

    //**********************  Texture stuff: **********************
    //Returns a pointer to the Texture class. This reads and sets up the texture for OpenGL
    //and returns the Texture ID that OpenGL uses from Texture::id()

    Q_ASSERT(gsl::NumberOfTextures < mTextureUnits && "Fix more flexible, robust system");
    mTexture[0] = new Texture();
    mTexture[1] = new Texture("Textures/hund.bmp");
    mTexture[2] = new Texture("Textures/white.bmp");
    mTexture[3] = new Texture("Textures/height8.bmp");
    mTexture[4] = new Texture("Textures/red.bmp");
    mTexture[5] = new Texture("Textures/green.bmp");
    mTexture[6] = new Texture("Textures/blue.bmp");

    //Set the textures loaded to a texture unit
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture[0]->id());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mTexture[1]->id());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mTexture[2]->id());
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, mTexture[3]->id());
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, mTexture[4]->id());
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, mTexture[5]->id());
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, mTexture[6]->id());

    //********************* Making some materials **********************

    Material *tempMaterial = new Material();
    tempMaterial->mShader = mShaderProgram[gsl::COLORSHADER];
    tempMaterial->mColor = gsl::vec3{0.3f, 0.3f, 0.3f};
    tempMaterial->mName = "PlainGray";
    mMaterial[0] = tempMaterial;

    tempMaterial = new Material();
    tempMaterial->mShader = mShaderProgram[gsl::TEXTURESHADER];
    tempMaterial->mTexture = mTexture[1];
    tempMaterial->mColor = gsl::vec3{0.3f, 0.3f, 0.3f};
    tempMaterial->mName = "TextureA";
    mMaterial[1] = tempMaterial;

    tempMaterial = new Material();
    tempMaterial->mShader = mShaderProgram[gsl::PHONGSHADER];
    tempMaterial->mTexture = mTexture[2];
    tempMaterial->mColor = gsl::vec3{0.3f, 0.3f, 0.3f};
    tempMaterial->mName = "Shiny";
    tempMaterial->mSpecularStrenght = 0.9f;
    tempMaterial->mSpecularExponent = 164;
    mMaterial[2] = tempMaterial;

    tempMaterial = new Material();
    tempMaterial->mShader = mShaderProgram[gsl::PHONGSHADER];
    tempMaterial->mTexture = mTexture[4];
    tempMaterial->mColor = gsl::vec3{0.3f, 0.3f, 0.3f};
    tempMaterial->mName = "ShinyR";
    tempMaterial->mSpecularStrenght = 0.9f;
    tempMaterial->mSpecularExponent = 164;
    mMaterial[3] = tempMaterial;

    tempMaterial = new Material();
    tempMaterial->mShader = mShaderProgram[gsl::PHONGSHADER];
    tempMaterial->mTexture = mTexture[5];
    tempMaterial->mColor = gsl::vec3{0.3f, 0.3f, 0.3f};
    tempMaterial->mName = "ShinyG";
    tempMaterial->mSpecularStrenght = 0.9f;
    tempMaterial->mSpecularExponent = 164;
    mMaterial[4] = tempMaterial;

    tempMaterial = new Material();
    tempMaterial->mShader = mShaderProgram[gsl::PHONGSHADER];
    tempMaterial->mTexture = mTexture[6];
    tempMaterial->mColor = gsl::vec3{0.3f, 0.3f, 0.3f};
    tempMaterial->mName = "ShinyB";
    tempMaterial->mSpecularStrenght = 0.9f;
    tempMaterial->mSpecularExponent = 164;
    mMaterial[5] = tempMaterial;
}

void RenderWindow::setCameraSpeed(float value)
{
    mCameraSpeed += value;

    //Keep within some min and max values
    if(mCameraSpeed < 0.01f)
        mCameraSpeed = 0.01f;
    if (mCameraSpeed > 0.5f)
        mCameraSpeed = 0.5f;
}

//This function is called from Qt when window is exposed (shown)
// and when it is resized
//exposeEvent is a overridden function from QWindow that we inherit from
void RenderWindow::exposeEvent(QExposeEvent *)
{
    //if not already initialized - run init() function
    if (!mInitialized)
        init();

    //This is just to support modern screens with "double" pixels (Macs and some 4k Windows laptops)
    const qreal retinaScale = devicePixelRatio();

    //Set viewport width and height
    glViewport(0, 0, static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));

    //If the window actually is exposed to the screen we start the main loop
    //isExposed() is a function in QWindow
    if (isExposed())
    {
        //This timer runs the actual MainLoop
        //16 means 16ms = 60 Frames pr second (should be 16.6666666 to be exact...)
        mRenderTimer->start(16);
        mTimeStart.start();
    }

    //calculate aspect ration and set projection matrix
    mAspectratio = static_cast<float>(width()) / height();
    //    qDebug() << mAspectratio;
    mCurrentCamera->mProjectionMatrix.perspective(45.f, mAspectratio, 0.1f, 1000.f);
    //    qDebug() << mCamera.mProjectionMatrix;
}

//The way this is set up is that we start the clock before doing the draw call,
// and check the time right after it is finished (done in the render function)
//This will approximate what framerate we COULD have.
//The actual frame rate on your monitor is limited by the vsync and is probably 60Hz
void RenderWindow::calculateFramerate()
{
    long nsecElapsed = mTimeStart.nsecsElapsed();
    static int frameCount{0};                       //counting actual frames for a quick "timer" for the statusbar

    if (mMainWindow)            //if no mainWindow, something is really wrong...
    {
        ++frameCount;
        if (frameCount > 30)    //once pr 30 frames = update the message twice pr second (on a 60Hz monitor)
        {
            //showing some statistics in status bar
            mMainWindow->statusBar()->showMessage(" Time pr FrameDraw: " +
                                                  QString::number(nsecElapsed/1000000.f, 'g', 4) + " ms  |  " +
                                                  "FPS (approximated): " + QString::number(1E9 / nsecElapsed, 'g', 7));
            frameCount = 0;     //reset to show a new message in 60 frames
        }
    }
}

//Simple way to turn on/off wireframe mode
//Not totally accurate, but draws the objects with
//lines instead of filled polygons
void RenderWindow::toggleWireframe(bool buttonState)
{
    if (buttonState)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);    //turn on wireframe mode
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    //turn off wireframe mode
        glEnable(GL_CULL_FACE);
    }
}

void RenderWindow::reset()
{
    if (mSelectedScene == 0)
    {
        mBalls[0]->bIsMoving = false;
        mBalls[0]->mMatrix.setPosition(mBalls[0]->mInitPos.x, mBalls[0]->mInitPos.y, mBalls[0]->mInitPos.z);
        mBalls[0]->mTime = 0;
    }
    else if (mSelectedScene == 1)
    {
        mBalls[1]->bIsMoving = false;
        mBalls[1]->mMatrix.setPosition(mBalls[1]->mInitPos.x, mBalls[1]->mInitPos.y, mBalls[1]->mInitPos.z);
        mBalls[1]->mTime = 0;
        /*
        for (unsigned int i{1}; i < mBalls.size(); i++)
        {
            mBalls[i]->bIsMoving = false;
            mBalls[i]->mMatrix.setPosition(mBalls[i]->mInitPos.x, mBalls[i]->mInitPos.y, mBalls[i]->mInitPos.z);
            mBalls[i]->mTime = 0;
        }
        */
    }
    else if (mSelectedScene == 2)
    {
        mPlayer->mMatrix.setPosition(mPlayer->mInitPos.x, mPlayer->mInitPos.y, mPlayer->mInitPos.z);
        mEnemy->bIsMoving = false;
        mEnemy->mMatrix.setPosition(mEnemy->mInitPos.x, mEnemy->mInitPos.y, mEnemy->mInitPos.z);

        for (unsigned int i{0}; i < mTrophies.size(); i++)
            mTrophies[i]->bIsPicked = false;
    }
}

void RenderWindow::playPause()
{
    if (mSelectedScene == 0)
    {
        if (!mBalls[0]->bIsMoving) mBalls[0]->bIsMoving = true;
        else mBalls[0]->bIsMoving = false;
    }
    else if (mSelectedScene == 1)
    {
        if (!mBalls[1]->bIsMoving) mBalls[1]->bIsMoving = true;
        else mBalls[1]->bIsMoving = false;
        /*
        for (unsigned int i{1}; i < mBalls.size(); i++)
        {
            if (!mBalls[i]->bIsMoving) mBalls[i]->bIsMoving = true;
            else mBalls[i]->bIsMoving = false;
        }
        */
    }
    else if (mSelectedScene == 2)
    {
        if (!mEnemy->bIsMoving) mEnemy->bIsMoving = true;
        else mEnemy->bIsMoving = false;
    }
}

void RenderWindow::NPCmovement()
{
    static unsigned int i{0};
    static bool movingForward{true};

    if (movingForward)
    {
        if (i < mCurves[mSelectedCurve]->mCurvePts.size())
        {
            i = mCurves[mSelectedCurve]->mCurvePts.size();
        }
        movingForward = false;
    }
    else
    {
        if (i>0)
        {
            --i;
        }
        else
        {
            if (i<=0)
            {
                i=0;
            }
            movingForward = true;
        }
    }

    //following current curve
    if (i < mCurves[mSelectedCurve]->mCurvePts.size())
    {
        mEnemy->bReachedEndPoint = false;
        mEnemy->mMatrix.setPosition(mCurves[mSelectedCurve]->mCurvePts[i].x, 0.f, mCurves[mSelectedCurve]->mCurvePts[i].z);
    }
    else mEnemy->bReachedEndPoint = true;
}

void RenderWindow::followTerrain(VisualObject &a)
{
    gsl::vec3 bar;

    for (auto t : mSurfaces[mSelectedScene]->mTriangles)
    {
        bar = a.baryCoord(gsl::vec2{t.V1.mXYZ.x, t.V1.mXYZ.z},
                          gsl::vec2{t.V2.mXYZ.x, t.V2.mXYZ.z},
                          gsl::vec2{t.V3.mXYZ.x, t.V3.mXYZ.z},
                          gsl::vec2{a.mMatrix.getPosition().x, a.mMatrix.getPosition().z});

        if (bar.x >= 0.f && bar.y >= 0.f && bar.z >= 0.f)
        {
            a.mMatrix.setPosition(a.mMatrix.getPosition().x, (t.V1.mXYZ.y * bar.x) + (t.V2.mXYZ.y * bar.y) + (t.V3.mXYZ.y * bar.z), a.mMatrix.getPosition().z);
            break;
        }
    }
}

//Uses QOpenGLDebugLogger if this is present
//Reverts to glGetError() if not
void RenderWindow::checkForGLerrors()
{
    if(mOpenGLDebugLogger)
    {
        const QList<QOpenGLDebugMessage> messages = mOpenGLDebugLogger->loggedMessages();
        for (const QOpenGLDebugMessage &message : messages)
            qDebug() << message;
    }
    else
    {
        GLenum err = GL_NO_ERROR;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            qDebug() << "glGetError returns " << err;
        }
    }
}

//Tries to start the extended OpenGL debugger that comes with Qt
void RenderWindow::startOpenGLDebugger()
{
    QOpenGLContext * temp = this->context();
    if (temp)
    {
        QSurfaceFormat format = temp->format();
        if (! format.testOption(QSurfaceFormat::DebugContext))
            qDebug() << "This system can not use QOpenGLDebugLogger, so we revert to glGetError()";

        if(temp->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
        {
            qDebug() << "System can log OpenGL errors!";
            mOpenGLDebugLogger = new QOpenGLDebugLogger(this);
            if (mOpenGLDebugLogger->initialize()) // initializes in the current context
            {
                //Turn off Notifications
                mOpenGLDebugLogger->disableMessages(QOpenGLDebugMessage::AnySource, QOpenGLDebugMessage::AnyType, QOpenGLDebugMessage::NotificationSeverity);
                qDebug() << "Started OpenGL debug logger!";
            }
        }
    }
}

void RenderWindow::handleInput()
{
    //Camera
    mCurrentCamera->setSpeed(0.f);  //cancel last frame movement
    if(mInput.RMB)
    {
        //Dirty speedup when Shift is pressed:
        //Seems to kick in on autorepeate of the keyboard
        float tempSpeed = mCameraSpeed;
        if (mSelectedScene == 1) tempSpeed *= 10;

        Qt::KeyboardModifiers temp = QGuiApplication::keyboardModifiers();
        if(temp == Qt::ShiftModifier)
            tempSpeed *= 2.5f;

        if(mInput.W)
            mCurrentCamera->setSpeed(-tempSpeed);
        if(mInput.S)
            mCurrentCamera->setSpeed(tempSpeed);
        if(mInput.D)
            mCurrentCamera->moveRight(tempSpeed);
        if(mInput.A)
            mCurrentCamera->moveRight(-tempSpeed);
        if(mInput.Q)
            mCurrentCamera->updateHeigth(-tempSpeed);
        if(mInput.E)
            mCurrentCamera->updateHeigth(tempSpeed);
    }
    else    //very manual way to make player move
    {
        float playerSpeed = 0.05f;

        //Dirty run when Shift is pressed:
        Qt::KeyboardModifiers temp = QGuiApplication::keyboardModifiers();
        if(temp == Qt::ShiftModifier)
            playerSpeed = 0.1f;

        if(mInput.W)
            mPlayer->mMatrix.translate(playerSpeed, 0.f, 0.f);
        if(mInput.A)
            mPlayer->mMatrix.translate(0.f, 0.f, -playerSpeed);
        if(mInput.S)
            mPlayer->mMatrix.translate(-playerSpeed, 0.f, 0.f);
        if(mInput.D)
            mPlayer->mMatrix.translate(0.f, 0.f, playerSpeed);
    }
}

void RenderWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat())
        return;
    //        qDebug() << "Auto KeyPressed";
    else
    {
        //        qDebug() << "KeyPressed";
        if (event->key() == Qt::Key_Escape) //Shuts down whole program
        {
            mMainWindow->close();
        }

        //    You get the keyboard input like this
        if(event->key() == Qt::Key_W)
        {
            mInput.W = true;
        }
        if(event->key() == Qt::Key_S)
        {
            mInput.S = true;
        }
        if(event->key() == Qt::Key_D)
        {
            mInput.D = true;
        }
        if(event->key() == Qt::Key_A)
        {
            mInput.A = true;
        }
        if(event->key() == Qt::Key_Q)
        {
            mInput.Q = true;
        }
        if(event->key() == Qt::Key_E)
        {
            mInput.E = true;
        }
        if(event->key() == Qt::Key_R)
        {
            reset();
        }
        if(event->key() == Qt::Key_X)
        {
            playPause();
        }
        if(event->key() == Qt::Key_Up)
        {
            mInput.UP = true;
        }
        if(event->key() == Qt::Key_Down)
        {
            mInput.DOWN = true;
        }
        if(event->key() == Qt::Key_Left)
        {
            mInput.LEFT = true;
        }
        if(event->key() == Qt::Key_Right)
        {
            mInput.RIGHT = true;
        }
        if(event->key() == Qt::Key_U)
        {        }
        if(event->key() == Qt::Key_O)
        {        }
        if(event->key() == Qt::Key_K)
        {        }
        if(event->key() == Qt::Key_1)
        {
            mSelectedScene = 0;
            mCurrentCamera->setPosition(gsl::vec3(2.f, 10.f, 12.f));
        }
        if(event->key() == Qt::Key_2)
        {
            mSelectedScene = 1;
            mCurrentCamera->setPosition(gsl::vec3{5000, 100, 10000});
        }
        if(event->key() == Qt::Key_3)
        {
            mSelectedScene = 2;
            mCurrentCamera->setPosition(gsl::vec3(2.f, 10.f, 12.f));
        }
    }
}

void RenderWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat())
        return;
    //        qDebug() << "Auto KeyReleased";
    else
    {
        //        qDebug() << "KeyReleased";

        //    You get the keyboard input like this

        if(event->key() == Qt::Key_W)
        {
            mInput.W = false;
        }
        if(event->key() == Qt::Key_S)
        {
            mInput.S = false;
        }
        if(event->key() == Qt::Key_D)
        {
            mInput.D = false;
        }
        if(event->key() == Qt::Key_A)
        {
            mInput.A = false;
        }
        if(event->key() == Qt::Key_Q)
        {
            mInput.Q = false;
        }
        if(event->key() == Qt::Key_E)
        {
            mInput.E = false;
        }
        if(event->key() == Qt::Key_R)
        {        }
        if(event->key() == Qt::Key_X)
        {        }
        if(event->key() == Qt::Key_Up)
        {
            mInput.UP = false;
        }
        if(event->key() == Qt::Key_Down)
        {
            mInput.DOWN = false;
        }
        if(event->key() == Qt::Key_Left)
        {
            mInput.LEFT = false;
        }
        if(event->key() == Qt::Key_Right)
        {
            mInput.RIGHT = false;
        }
        if(event->key() == Qt::Key_U)
        {        }
        if(event->key() == Qt::Key_O)
        {        }
    }
}

void RenderWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        mInput.RMB = true;
    if (event->button() == Qt::LeftButton)
        mInput.LMB = true;
    if (event->button() == Qt::MiddleButton)
        mInput.MMB = true;
}

void RenderWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        mInput.RMB = false;
    if (event->button() == Qt::LeftButton)
        mInput.LMB = false;
    if (event->button() == Qt::MiddleButton)
        mInput.MMB = false;
}

void RenderWindow::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;

    //if RMB, change the speed of the camera
    if (mInput.RMB)
    {
        if (numDegrees.y() < 1)
            setCameraSpeed(0.001f);
        if (numDegrees.y() > 1)
            setCameraSpeed(-0.001f);
    }
    event->accept();
}

void RenderWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (mInput.RMB)
    {
        //Using mMouseXYlast as deltaXY so we don't need extra variables
        mMouseXlast = event->pos().x() - mMouseXlast;
        mMouseYlast = event->pos().y() - mMouseYlast;

        if (mMouseXlast != 0)
            mCurrentCamera->yaw(mCameraRotateSpeed * mMouseXlast);
        if (mMouseYlast != 0)
            mCurrentCamera->pitch(mCameraRotateSpeed * mMouseYlast);
    }
    mMouseXlast = event->pos().x();
    mMouseYlast = event->pos().y();
}
