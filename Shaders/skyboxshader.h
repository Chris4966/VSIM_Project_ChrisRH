#ifndef SKYBOXSHADER_H
#define SKYBOXSHADER_H

#include "shader.h"

class SkyboxShader : public Shader
{
public:
    SkyboxShader(const std::string &shaderName);

    void setupShader() override;
    void use(class Material *materialIn) override;

    GLint mTextureUniform{-1};
};

#endif // SKYBOXSHADER_H
