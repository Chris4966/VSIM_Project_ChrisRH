#ifndef COLORSHADER_H
#define COLORSHADER_H

#include "shader.h"

class ColorShader : public Shader
{
public:
    ColorShader(const std::string &shaderName);

    void setupShader() override;
    void use(class Material * = nullptr) override;
};

#endif // COLORSHADER_H
