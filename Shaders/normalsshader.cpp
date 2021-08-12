#include "normalsshader.h"
#include "camera.h"

NormalsShader::NormalsShader(const std::string &shaderName) : Shader(shaderName)
{   }

void NormalsShader::setupShader()
{
    mMatrixUniform = glGetUniformLocation( mProgram, "mMatrix" );
    vMatrixUniform = glGetUniformLocation( mProgram, "vMatrix" );
    pMatrixUniform = glGetUniformLocation( mProgram, "pMatrix" );
}

void NormalsShader::use(Material *)
{
    glUseProgram(mProgram);
    glUniformMatrix4fv( vMatrixUniform, 1, GL_TRUE, mCurrentCamera->mViewMatrix.constData());
    glUniformMatrix4fv( pMatrixUniform, 1, GL_TRUE, mCurrentCamera->mProjectionMatrix.constData());
}

