#include "terrain.h"
#include "vertex.h"
#include "vec3.h"
#include "texture.h"
#include "material.h"
#include <QDebug>
#include <cmath>

Terrain::Terrain(Texture *texture, float horisontalSpacing, float verticalSpacing, float heightPlacement)
    : mTexture{texture}, mHorisontalSpacing{horisontalSpacing}, mHeightSpacing{verticalSpacing}, mHeightPlacement{heightPlacement}
{
    mWidth = mTexture->mColumns;    //should this not be rows?
    mDepth = mTexture->mRows;       //should this not be columns?

    makeTerrain();
    init();
}

Terrain::~Terrain()
{
}

/**
  Should work work with non square textures, but calculateNormals might not.
  Note to self: have drawings and calculations in a Notability sheet
*/
void Terrain::makeTerrain()
{
    gsl::vec3 normal{0.f, 1.f, 0.f};

    float totalWidthMeters = (mWidth-1) * mHorisontalSpacing;
    vertexXStart = -totalWidthMeters / 2;
    vertexZStart = totalWidthMeters / 2;

    for(int d{0}; d < mDepth; ++d)
    {
        for(int w{0}; w < mWidth; ++w)
        {
            float heightFromBitmap = mTexture->getHeightFromIndex(w + d*mDepth) * mHeightSpacing / 50.f + mHeightPlacement; //TODO: make the mVerticalSpacing calculation better!
            mVertices.emplace_back(gsl::vec3{vertexXStart + (w*mHorisontalSpacing), heightFromBitmap,
                                             vertexZStart - (d*mHorisontalSpacing)}, normal, gsl::vec2{w / (mWidth-1.f), d / (mDepth-1.f)});
        }
    }

//    qDebug() << "Vertices made";
    // The grid is drawn in quads with diagonal from lower left to upper right
    //          _ _
    //         |/|/|
    //          - -
    //         |/|/|
    //          - -
    for(int d{0}; d < mDepth-1; ++d)        //mDepth - 1 because we draw the last quad from mDepth - 1 and in negative z direction
    {
        for(int w{0}; w < mWidth-1; ++w)    //mWidth - 1 because we draw the last quad from mWidth - 1 and in positive x direction
        {
            mIndices.emplace_back(w + d * mWidth);                  // 0 + 0 * mWidth               = 0
            mIndices.emplace_back(w + d * mWidth + mWidth + 1);     // 0 + 0 * mWidth + mWidth + 1  = mWidth + 1
            mIndices.emplace_back(w + d * mWidth + mWidth);         // 0 + 0 * mWidth + mWidth      = mWidth
            mIndices.emplace_back(w + d * mWidth);                  // 0 + 0 * mWidth               = 0
            mIndices.emplace_back(w + d * mWidth + 1);              // 0 + 0 * mWidth + 1           = 1
            mIndices.emplace_back(w + d * mWidth + mWidth + 1);     // 0 + 0 * mWidth + mWidth + 1  = mWidth + 1
        }
    }
//    qDebug() << "Indices made";

//    calculateNormals();

    mMatrix.setToIdentity();
    mMatrix.translateY(-0.02f);      // just put in a little below the world grid

    calculateNormals();

    qDebug() << "Ground made | Total Width in Meters" << totalWidthMeters << "| Size:" << mWidth << ", " << mDepth << "| VertexXStart:" << vertexXStart << "| VertexZStart" << vertexZStart;
}

void Terrain::init()
{
    initializeOpenGLFunctions();

    //Vertex Array Object - VAO
    glGenVertexArrays( 1, &mVAO );
    glBindVertexArray( mVAO );

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers( 1, &mVBO );
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );

    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof( Vertex ), mVertices.data(), GL_STATIC_DRAW );

    // 1rst attribute buffer : vertices
    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);

    // 3rd attribute buffer : uvs
    glVertexAttribPointer(2, 2,  GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)( 6 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(2);

    //Second buffer - holds the indices (Element Array Buffer - EAB):
    glGenBuffers(1, &mEAB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEAB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(GLuint), mIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    mIndiceCount = mIndices.size();
}

void Terrain::draw()
{
    glBindVertexArray( mVAO );
    mMaterial->mShader->use(mMaterial);
    glUniformMatrix4fv( mMaterial->mShader->mMatrixUniform, 1, GL_TRUE, mMatrix.constData());   //move to shader???
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
}

void Terrain::calculateNormals()
{
    //not tested for non square textures:
    if (mWidth != mDepth)
    {
        qDebug() << "Normals for ground not calclulated! Use square texture!";
        return;
    }
    unsigned short resolutionSquared= mWidth * mDepth;

    //terrain has the diagonal of the quads pointing up and right, like |/| (not |\|)

    //************************* Special case - bottom row has no vertices below **********************************
    for(unsigned short i{0}; i < mWidth; i++)
    {
        //temporary normals for each of the 6 surrounding triangles
        gsl::vec3 surroundingNormals[6]{};

        //the center vertex that we calculate the normal for
        gsl::vec3 center = mVertices.at(i).mXYZ;

        gsl::vec3 first{};  //first vector will be crossed with
        gsl::vec3 second{}; //second vector - check right hand rule!

        // a: first = north, second = west
        //check if we are to the left, if so - skip:
        //If first vertex - skip - already (0, 1, 0)
        if(i != 0)
        {
            first = mVertices.at(i+ mWidth).mXYZ - center;
            second = mVertices.at(i-1).mXYZ - center;
            surroundingNormals[0] = first ^ second;
        }
        // b: first and second flips so we don't have to calculate a vector we already have!!
        // second = north east, first = north
        //check if we are at left, if so - skip
        if( ((i + 1) % mWidth) == false)
        {
            second = mVertices.at(i + mWidth + 1).mXYZ - center;
            //first = mVertices.at(i+ mWidth).mXYZ - center;
            surroundingNormals[1] = second ^ first;

            //c: first = east, second = north east
            first = mVertices.at(i+1).mXYZ - center;
            //second = mVertices.at(i + mWidth + 1).mXYZ - center;
            surroundingNormals[2] = first ^ second;
        }

        //add all vectors and normalize
        gsl::vec3 result{};
        for(int i{0}; i < 6; i++)
        {
            surroundingNormals[i].normalize();
            result += surroundingNormals[i];
        }
        result.normalize();

        //put the normal into the vertex
        mVertices.at(i).set_normal(result.x, result.y, result.z);
    }

    //calculate each of the triangles normal - omitting the outer vertices
    //starting av mWidth + 1 == the second vertex on the second row
    //ending each row at second to last vertex, to not hit the outer edge
    //ending at resolutionSquared - mWidth - 2 == second to last vertex on the second to last row
    for(unsigned short i = mWidth + 1 ; i < resolutionSquared - mWidth - 2; i++)
    {
        //if at the end of a row, jump to next
        if( (i + 2) % mWidth == 0)
        {   i += 2; //have to add 2 to get to the next correct index
            continue;
        }
        //goes in a counter clockwise direction
        //terrain has the diagonal of the quads pointing up and right, like |/| (not |\|)

        //temporary normals for each of the 6 surrounding triangles
        gsl::vec3 surroundingNormals[6]{};

        //the center vertex that we calculate the normal for
        gsl::vec3 center = mVertices.at(i).mXYZ;

        //first and second vector flips each time because second
        //is the "first" at the next calculation.
        //We don't want to calculate again!
        gsl::vec3 first{};
        gsl::vec3 second{};

        // a 0: first = north, second = west
        //check if we are to the left, if so - skip:
        first = mVertices.at(i+ mWidth).mXYZ - center;
        second = mVertices.at(i-1).mXYZ - center;
        surroundingNormals[0] = first ^ second;

        // b 1: first = south west, second = west
        first = mVertices.at(i - mWidth - 1).mXYZ - center;
        surroundingNormals[1] = second ^ first;

        //c 2: first = south west, second = south
        second = mVertices.at(i - mWidth).mXYZ - center;
        surroundingNormals[2] = first ^ second;

        //d 3: second = south, first = east
        //check if we are to the left, if so - skip
        first = mVertices.at(i + 1).mXYZ - center;
        surroundingNormals[3] = second ^ first;

        //e 4: first = east, second = north east
        second = mVertices.at(i + mWidth +1).mXYZ - center;
        surroundingNormals[4] = first ^ second;

        //f 5: second = north east, first = north
        first = mVertices.at(i + mWidth).mXYZ - center;
        surroundingNormals[5] = second ^ first;

        //add all vectors and normalize
        gsl::vec3 result{};
        for(int i{0}; i < 6; i++)
        {
            surroundingNormals[i].normalize();
            result += surroundingNormals[i];
        }
        result.normalize();

        //put the normal into the vertex
        mVertices.at(i).set_rgb(result.x, result.y, result.z);
    }
}

float Terrain::calculateHeight(const gsl::vec3 &positionIn)
{
    // finding what position in the terrain grid the position is
    float xPos = std::floor((std::abs(vertexXStart) + positionIn.x)/mHorisontalSpacing);
    float zPos = std::floor((std::abs(vertexZStart) - positionIn.z)/mHorisontalSpacing); //Z is positive the opposite direction of the terrain!
//    qDebug() << "xPos:" << xPos << "zPos:" << zPos;

    float heightA{-10000.f};    // barycentricHeight returns -10000 if height calculation fails
    float heightB{-10000.f};

    // Have to check that the player is within the terrain
    if(xPos >= 0.f && xPos < (mWidth-1) && zPos >= 0.f && zPos < (mWidth-1))
    {

        //Finding array indexes for the two triangles positionIn can be in:
        // Assumes the terrain is square!
        assert(mWidth == mDepth);

        // Lower triangle A
        int triangleA1 = xPos + zPos * mWidth;
        int triangleA2 = triangleA1 + 1;
        int triangleA3 = triangleA2 + mWidth;

        // Upper triangle B
        int triangleB1 = triangleA1;
        int triangleB2 = triangleA3;
        int triangleB3 = triangleA1 + mWidth;

        //Calculate height for triangle A
        gsl::vec3 corner1 = mVertices.at(triangleA1).mXYZ;
        gsl::vec3 corner2 = mVertices.at(triangleA2).mXYZ;
        gsl::vec3 corner3 = mVertices.at(triangleA3).mXYZ;
        heightA = gsl::vec3::barycentricHeight(positionIn, corner1, corner2, corner3);
        //    qDebug() << "Height A:" << heightA;

        gsl::vec3 corner4 = mVertices.at(triangleB1).mXYZ;
        gsl::vec3 corner5 = mVertices.at(triangleB2).mXYZ;
        gsl::vec3 corner6 = mVertices.at(triangleB3).mXYZ;
        heightB = gsl::vec3::barycentricHeight(positionIn, corner4, corner5, corner6);
        //    qDebug() << "Height B:" << heightB;
    }

    // Return the highest height calculated. If barycentricHeight fails it returns -10000
    return heightA>heightB ? heightA : heightB;
}
