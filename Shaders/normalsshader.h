#ifndef NORMALSSHADER_H
#define NORMALSSHADER_H

#include "shader.h"

class NormalsShader : public Shader
{
public:
    NormalsShader(const std::string &shaderName);

    void setupShader() override;
    void use(class Material * = nullptr) override;
};

#endif // NORMALSSHADER_H
