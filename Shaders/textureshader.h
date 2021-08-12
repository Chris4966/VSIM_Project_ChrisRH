#ifndef TEXTURESHADER_H
#define TEXTURESHADER_H

#include "shader.h"

class TextureShader : public Shader
{
public:
    TextureShader(const std::string &shaderName);

    void setupShader() override;
    void use(class Material *materialIn) override;

    GLint mTextureUniform{-1};
};

#endif // TEXTURESHADER_H
