QT          += core gui widgets

TEMPLATE    = app
CONFIG      += c++17

TARGET      = 3D-programmering

INCLUDEPATH += \
    ./gsl \
    ./Shaders \
	./Objects

SOURCES += main.cpp \
    Objects/bsplinecurve.cpp \
    Objects/landscape.cpp \
    Objects/octahedronball.cpp \
    Objects/rollingball.cpp \
    Objects/trianglesurface.cpp \
    camera.cpp \
    Objects/editorgrid.cpp \
    Objects/gizmo.cpp \
    Objects/light.cpp \
    Objects/objmesh.cpp \
    Objects/quad.cpp \
    renderwindow.cpp \
    mainwindow.cpp \
    resourcemanager.cpp \
    scene.cpp \
    Objects/skybox.cpp \
    Objects/terrain.cpp \
    texture.cpp \
    vertex.cpp \
    visualobject.cpp \
    Objects/xyz.cpp \
    gsl/mat3x3.cpp \
    gsl/mat4x4.cpp \
    gsl/vec2.cpp \
    gsl/vec3.cpp \
    Shaders/colorshader.cpp \
    Shaders/phongshader.cpp \
    Shaders/shader.cpp \
    Shaders/skyboxshader.cpp \
    Shaders/textureshader.cpp \
    Shaders/normalsshader.cpp

HEADERS += \
    Objects/bsplinecurve.h \
    Objects/landscape.h \
    Objects/octahedronball.h \
    Objects/rollingball.h \
    Objects/trianglesurface.h \
    camera.h \
    constants.h \
    Objects/editorgrid.h \
    Objects/gizmo.h \
    gltypes.h \
    input.h \
    Objects/light.h \
    material.h \
    Objects/objmesh.h \
    Objects/quad.h \
    renderwindow.h \
    mainwindow.h \
    resourcemanager.h \
    scene.h \
    Objects/skybox.h \
    Objects/terrain.h \
    texture.h \
    vertex.h \
    visualobject.h \
    Objects/xyz.h \
    gsl/mat3x3.h \
    gsl/mat4x4.h \
    gsl/math_constants.h \
    gsl/vec2.h \
    gsl/vec3.h \
    Shaders/colorshader.h \
    Shaders/phongshader.h \
    Shaders/shader.h \
    Shaders/skyboxshader.h \
    Shaders/textureshader.h \
    Shaders/normalsshader.h

FORMS += \
    mainwindow.ui

DISTFILES += \
    Shaders/colorshader.frag \
    Shaders/colorshader.vert \
    Shaders/phongshader.frag \
    Shaders/phongshader.vert \
    Shaders/shownormalsshader.frag \
    Shaders/shownormalsshader.geom \
    Shaders/shownormalsshader.vert \
    Shaders/skyboxshader.frag \
    Shaders/skyboxshader.vert \
    Shaders/textureshader.frag \
    Shaders/textureshader.vert
