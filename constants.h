#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

namespace gsl  //Game School Lib
{
    const std::string ProjectFolderName{"../VSIM_Project_ChrisRH/"};
    const std::string AssetFilePath{ProjectFolderName + "Assets/"};
    const std::string ShaderFilePath{ProjectFolderName + "Shaders/"};

    enum Shaders
    {
        NONE = -1,
        COLORSHADER = 0,
        TEXTURESHADER,
        PHONGSHADER,
        NORMALSSHADER,
        SKYBOXSHADER
    };

    const int NumberOfShaders{8};
    const int NumberOfTextures{8};
    const int NumberOfMaterials{10};

    const int EditorAssetNumber{3};

    enum EditorAssets
    {
        GIZMO,
        GRID,
        AXIS
    };

    enum States
    {
        PATROL,
        LEARN,
        CHASE
    };

}

#endif // CONSTANTS_H
