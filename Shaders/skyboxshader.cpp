#include "skyboxshader.h"

#include "material.h"
#include "camera.h"
#include "texture.h"

SkyboxShader::SkyboxShader(const std::string &shaderName) : Shader(shaderName)
{ }

void SkyboxShader::setupShader()
{
    mMatrixUniform = glGetUniformLocation( mProgram, "mMatrix" );
    vMatrixUniform = glGetUniformLocation( mProgram, "vMatrix" );
    pMatrixUniform = glGetUniformLocation( mProgram, "pMatrix" );
    mTextureUniform = glGetUniformLocation( mProgram, "skybox");
}

void SkyboxShader::use(Material *materialIn)
{
    glUseProgram(mProgram);
    glUniformMatrix4fv( vMatrixUniform, 1, GL_TRUE, mCurrentCamera->mViewMatrix.constData());
    glUniformMatrix4fv( pMatrixUniform, 1, GL_TRUE, mCurrentCamera->mProjectionMatrix.constData());

    glUniform1i(mTextureUniform, materialIn->mTexture->id()-1);  //should use texture slot!!!
}
