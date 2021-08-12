#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSurfaceFormat>
#include <QDebug>
#include <QScreen>  //for resizing the program at start

#include "renderwindow.h"
#include "camera.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    //this sets up what's in the mainwindow.ui
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete mRenderWindow;
    delete ui;
}

void MainWindow::init()
{
    //This will contain the setup of the OpenGL surface we will render into
    QSurfaceFormat format;

    //OpenGL v 4.1 - (Ole Flatens Mac does not support higher than this - sorry!)
    //you can try other versions, but then have to update RenderWindow and Shader
    //to inherit from other than QOpenGLFunctions_4_1_Core
    format.setVersion(4, 1);
    //Using the main profile for OpenGL - no legacy code permitted
    format.setProfile(QSurfaceFormat::CoreProfile);
    //A QSurface can be other types than OpenGL
    format.setRenderableType(QSurfaceFormat::OpenGL);

    //This should activate OpenGL debug Context used in RenderWindow::startOpenGLDebugger().
    //This line (and the startOpenGLDebugger() and checkForGLerrors() in RenderWindow class)
    //can be deleted, but it is nice to have some OpenGL debug info!
    format.setOption(QSurfaceFormat::DebugContext);

    // The renderer will need a depth buffer - (not requiered to set in glfw-tutorials)
    format.setDepthBufferSize(24);

    //Set the number of samples used for multisampling
    //Same as glfwWindowHint(GLFW_SAMPLES, 4);
    //https://learnopengl.com/Advanced-OpenGL/Anti-Aliasing
    format.setSamples(4);

    //Just prints out what OpenGL format we try to get
    // - this can be deleted
    qDebug() << "Requesting surface format: " << format;

    //We have a format for the OpenGL window, so let's make it:
    mRenderWindow = new RenderWindow(format, this);

    //Check if renderwindow did initialize, else prints error and quit
    if (!mRenderWindow->context()) {
        qDebug() << "Failed to create context. Can not continue. Quits application!";
        delete mRenderWindow;
        return;
    }

    //The OpenGL RenderWindow got made, so continuing the setup:
    //We put the RenderWindow inside a QWidget so we can put in into a
    //layout that is made in the .ui-file
    mRenderWindowContainer = QWidget::createWindowContainer(mRenderWindow);
    //OpenGLLayout is made in the .ui-file!
    ui->OpenGLLayout->addWidget(mRenderWindowContainer);

    //sets the keyboard input focus to the RenderWindow when program starts
    // - can be deleted, but then you have to click inside the renderwindow to get the focus
    // mRenderWindowContainer->setFocus(); //moved to RenderWindow::init()

    //Set the size of the program in % of the actual screen size
    QSize tempSize = QGuiApplication::primaryScreen()->size();
    if (tempSize.width() / tempSize.height() > 1.8)     // if ultra wide
    {
        tempSize.rheight() *= 0.85;
        tempSize.rwidth() *= 0.55;
    }
    else                                                // normal aspect ratio
        tempSize *= 0.7;
    resize(tempSize);

}

void MainWindow::on_actionGrid_toggled(bool arg)
{
    mRenderWindow->mRenderEditorGrid = arg;
}

void MainWindow::on_actionAxis_toggled(bool arg)
{
    mRenderWindow->mRenderEditorAxis = arg;
}

void MainWindow::on_actionCorner_Gizmo_toggled(bool arg)
{
    mRenderWindow->mRenderCornerGizmo = arg;
}

void MainWindow::on_actionWireframe_toggled(bool arg)
{
    mRenderWindow->toggleWireframe(arg);
}

void MainWindow::on_actionDraw_Normals_toggled(bool arg)
{
    mRenderWindow->mDrawNormals = arg;
}

void MainWindow::on_actionScene_1_triggered()
{
    mRenderWindow->mSelectedScene = 0;
    mRenderWindow->mCurrentCamera->setPosition(gsl::vec3(2.f, 10.f, 12.f));
}

void MainWindow::on_actionScene_2_triggered()
{
    mRenderWindow->mSelectedScene = 1;
    mRenderWindow->mCurrentCamera->setPosition(gsl::vec3{5000, 100, 10000});
}

void MainWindow::on_actionScene_3_triggered()
{
    mRenderWindow->mSelectedScene = 2;
    mRenderWindow->mCurrentCamera->setPosition(gsl::vec3(2.f, 10.f, 12.f));
}

void MainWindow::on_actionStart_Stop_Simulation_triggered()
{
    mRenderWindow->playPause();
}

void MainWindow::on_actionRestart_Simulation_triggered()
{
    mRenderWindow->reset();
}
