#include "phongshader.h"

#include "material.h"
#include "camera.h"
#include "light.h"
#include "texture.h"

PhongShader::PhongShader(const std::string &shaderName) : Shader(shaderName)
{
}

void PhongShader::use(Material *materialIn)
{
    glUseProgram(mProgram);
    glUniformMatrix4fv( vMatrixUniform, 1, GL_TRUE, mCurrentCamera->mViewMatrix.constData());
    glUniformMatrix4fv( pMatrixUniform, 1, GL_TRUE, mCurrentCamera->mProjectionMatrix.constData());

    glUniform3f(mLightPositionUniform, mCurrentLight->mMatrix.getPosition().x,
                mCurrentLight->mMatrix.getPosition().y, mCurrentLight->mMatrix.getPosition().z);
    glUniform3f(mCameraPositionUniform, mCurrentCamera->position().x, mCurrentCamera->position().y, mCurrentCamera->position().z);
    glUniform3f(mLightColorUniform, mCurrentLight->mLightColor.x, mCurrentLight->mLightColor.y, mCurrentLight->mLightColor.z);
    glUniform1i(mTextureUniform, materialIn->mTexture->id()-1); //TODO: this is a hack!! should check and use correct texture slot!!!

    glUniform1f(mSpecularStrengthUniform, materialIn->mSpecularStrenght);
    glUniform1i(mSpecularExponentUniform, materialIn->mSpecularExponent);
}

void PhongShader::setupShader()
{
    mMatrixUniform = glGetUniformLocation( mProgram, "mMatrix" );
    vMatrixUniform = glGetUniformLocation( mProgram, "vMatrix" );
    pMatrixUniform = glGetUniformLocation( mProgram, "pMatrix" );

    mLightColorUniform =            glGetUniformLocation( mProgram, "lightColor" );
//    mObjectColorUniform =           glGetUniformLocation( mProgram, "objectColor" );
    mAmbientLightStrengthUniform =  glGetUniformLocation( mProgram, "ambientStrengt" );
    mLightPositionUniform =         glGetUniformLocation( mProgram, "lightPosition" );
    mLightStrenghtUniform =         glGetUniformLocation( mProgram, "lightStrengt" );
    mSpecularStrengthUniform =      glGetUniformLocation( mProgram, "specularStrength" );
    mSpecularExponentUniform =      glGetUniformLocation( mProgram, "specularExponent" );
    mCameraPositionUniform =        glGetUniformLocation( mProgram, "cameraPosition" );
    mTextureUniform =               glGetUniformLocation( mProgram, "textureSampler" );
}
