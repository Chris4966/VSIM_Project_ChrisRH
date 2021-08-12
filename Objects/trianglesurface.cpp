#include "trianglesurface.h"
#include "vec3.h"
#include "vec2.h"
#include "vertex.h"
#include "material.h"
#include "resourcemanager.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

TriangleSurface::TriangleSurface(std::string fileName) : mFileName{fileName}
{
    //if ResourceManager does not find this file
    if(!ResourceManager::getInstance().searchMap(fileName, mVAO, mIndiceCount))
    {
        readFile();
        setTriangles();
        init();
    }
    //file is read before - correct parameters already given in ResourceManager::getInstance().searchMap call
    else
        //must call this to use OpenGL functions. Done in init() for new files
        initializeOpenGLFunctions();

    mMatrix.setToIdentity();
}

void TriangleSurface::init()
{
    //must call this to use OpenGL functions
    initializeOpenGLFunctions();

    //Vertex Array Object - VAO
    glGenVertexArrays( 1, &mVAO );
    glBindVertexArray( mVAO );

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers( 1, &mVBO );
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );

    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW );

    // 1rst attribute buffer : vertices
    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);

    // 3rd attribute buffer : uvs
    glVertexAttribPointer(2, 2,  GL_FLOAT, GL_FALSE, sizeof( Vertex ), (GLvoid*)( 6 * sizeof( GLfloat ) ));
    glEnableVertexAttribArray(2);

    //Second buffer - holds the indices (Element Array Buffer - EAB):
    glGenBuffers(1, &mEAB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEAB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(GLuint), mIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    mIndiceCount = mIndices.size();
    ResourceManager::getInstance().insertInMap(mFileName, mVAO, mIndiceCount);
}

void TriangleSurface::readFile()
{
    std::ifstream in;
    in.open(mFileName, std::ifstream::in);
    if (!in)
        qDebug() << "Could not open file for reading: " << QString::fromStdString(mFileName);

    std::string oneLine, oneWord;

    std::vector<gsl::vec3> tempVertices;
    std::vector<gsl::vec3> tempNormals;
    std::vector<gsl::vec2> tempUVs;

    unsigned int tempIndex{0};

    while (std::getline(in, oneLine))
    {
        std::stringstream ss;
        ss << oneLine;
        oneWord = "";
        ss >> oneWord;

        if (oneWord.empty()) continue;
        if (oneWord == "v")
        {
            gsl::vec3 tempVert;
            ss >> oneWord;
            tempVert.x = std::stof(oneWord);
            ss >> oneWord;
            tempVert.y = std::stof(oneWord);
            ss >> oneWord;
            tempVert.z = std::stof(oneWord);

            tempVertices.push_back(tempVert);
            continue;
        }
        if (oneWord == "uv")
        {
            gsl::vec2 tempUV;
            ss >> oneWord;
            tempUV.x = std::stof(oneWord);
            ss >> oneWord;
            tempUV.y = std::stof(oneWord);

            tempUVs.push_back(tempUV);
            continue;
        }
        if (oneWord == "n")
        {
            gsl::vec3 tempNormal;
            ss >> oneWord;
            tempNormal.x = std::stof(oneWord);
            ss >> oneWord;
            tempNormal.y = std::stof(oneWord);
            ss >> oneWord;
            tempNormal.z = std::stof(oneWord);

            tempNormals.push_back(tempNormal);
            continue;
        }
        if (oneWord == "f")
        {
            int index, normal, uv;
            for(int i = 0; i < 3; i++)
            {
                ss >> oneWord;
                std::stringstream tempWord(oneWord);
                std::string segment;
                std::vector<std::string> segmentArray;
                while(std::getline(tempWord, segment, '/'))
                {
                    segmentArray.push_back(segment);
                }
                index = std::stoi(segmentArray[0]);
                if (!segmentArray[1].empty())
                    uv = std::stoi(segmentArray[1]);
                else
                {
                    uv = 0;
                }
                normal = std::stoi(segmentArray[2]);
                --index;
                --uv;
                --normal;

                if (uv > -1)
                {
                    Vertex tempVert(tempVertices[index], tempNormals[normal], tempUVs[uv]);
                    mVertices.push_back(tempVert);
                }
                else
                {
                    Vertex tempVert(tempVertices[index], tempNormals[normal], gsl::vec2(0.0f, 0.0f));
                    mVertices.push_back(tempVert);
                }
                mIndices.push_back(tempIndex++);
            }
            continue;
        }
    }

    in.close();
    qDebug() << "Data file " << QString::fromStdString(mFileName) << "successfully read";
}

void TriangleSurface::draw()
{
    glBindVertexArray( mVAO );
    mMaterial->mShader->use(mMaterial);
    glUniformMatrix4fv( mMaterial->mShader->mMatrixUniform, 1, GL_TRUE, mMatrix.constData());   //move to shader???
    glDrawElements(GL_TRIANGLES, mIndiceCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
