#include "scene.h"
#include "visualobject.h"
#include "light.h"
#include "objmesh.h"
#include "terrain.h"
#include "light.h"
#include "constants.h"
#include "renderwindow.h"
#include "skybox.h"
#include "trianglesurface.h"
#include "landscape.h"
#include "camera.h"


Scene::Scene(RenderWindow *renderWindow) : mRenderWindow{renderWindow}
{

}

void Scene::init()
{
    //********************** Making the object to be drawn **********************
    VisualObject *temp;

    //Game Objects
    temp = new ObjMesh(gsl::AssetFilePath + "Meshes/pawn.obj");
    temp->mMatrix.translate(0, 0, 5);
    temp->mMatrix.scale(0.3, 0.3, 0.3);
    temp->mMaterial = mRenderWindow->mMaterial[5];
    temp->mInitPos = temp->mMatrix.getPosition();
    temp->setOBB(temp->mMatrix.getPosition() - temp->mMatrix.getScale(), temp->mMatrix.getPosition() + temp->mMatrix.getScale());
    mVisualObjects.push_back(temp);
    mRenderWindow->mPlayer = temp; // setting the interactive object

    temp = new ObjMesh(gsl::AssetFilePath + "Meshes/pawn.obj");
    temp->mMatrix.translate(10, 0, 20);
    temp->mMatrix.scale(0.3, 0.3, 0.3);
    temp->mMaterial = mRenderWindow->mMaterial[3];
    temp->mInitPos = temp->mMatrix.getPosition();
    temp->setOBB(temp->mMatrix.getPosition() - temp->mMatrix.getScale(), temp->mMatrix.getPosition() + temp->mMatrix.getScale());
    temp->mState = gsl::PATROL;
    mVisualObjects.push_back(temp);
    mRenderWindow->mEnemy = temp; // setting the enemy npc

    temp = new ObjMesh(gsl::AssetFilePath + "Meshes/box.obj");
    temp->mMatrix.translate(19, 0, 5);
    temp->mMatrix.scale(0.2, 0.5, 0.2);
    temp->mMaterial = mRenderWindow->mMaterial[2];
    temp->setOBB(temp->mMatrix.getPosition() - temp->mMatrix.getScale(), temp->mMatrix.getPosition() + temp->mMatrix.getScale());
    mVisualObjects.push_back(temp);
    mRenderWindow->mTrophies.push_back(temp);

    temp = new ObjMesh(gsl::AssetFilePath + "Meshes/box.obj");
    temp->mMatrix.translate(1, 0, 10);
    temp->mMatrix.scale(0.2, 0.5, 0.2);
    temp->mMaterial = mRenderWindow->mMaterial[2];
    temp->setOBB(temp->mMatrix.getPosition() - temp->mMatrix.getScale(), temp->mMatrix.getPosition() + temp->mMatrix.getScale());
    mVisualObjects.push_back(temp);
    mRenderWindow->mTrophies.push_back(temp);

    temp = new ObjMesh(gsl::AssetFilePath + "Meshes/box.obj");
    temp->mMatrix.translate(15, 0, 15);
    temp->mMatrix.scale(0.2, 0.5, 0.2);
    temp->mMaterial = mRenderWindow->mMaterial[2];
    temp->setOBB(temp->mMatrix.getPosition() - temp->mMatrix.getScale(), temp->mMatrix.getPosition() + temp->mMatrix.getScale());
    mVisualObjects.push_back(temp);
    mRenderWindow->mTrophies.push_back(temp);

    //Surfaces:
    temp = new TriangleSurface(gsl::AssetFilePath + "MapData/SurfaceData_1.dat");
    temp->mMaterial = mRenderWindow->mMaterial[2];
    mVisualObjects.push_back(temp);
    mRenderWindow->mSurfaces.push_back(temp);

    temp = new Landscape(gsl::AssetFilePath + "MapData/LASdata.dat");
    temp->mMaterial = mRenderWindow->mMaterial[4];
    mVisualObjects.push_back(temp);
    mRenderWindow->mSurfaces.push_back(temp);

    temp = new TriangleSurface(gsl::AssetFilePath + "MapData/SurfaceData_2.dat");
    temp->mMaterial = mRenderWindow->mMaterial[4];
    mVisualObjects.push_back(temp);
    mRenderWindow->mSurfaces.push_back(temp);


    //Light:
    mLight = new Light();
    mLight->mMatrix.translate(20.f, 10.f, 10.f);
    mLight->mMaterial = mRenderWindow->mMaterial[0];
    mVisualObjects.push_back(mLight);
}

void Scene::update()
{
    //sets the light to follow the camera
    gsl::vec3 lpos = mRenderWindow->mCurrentCamera->position() + gsl::vec3{0.f, 2.f, 0.f};
    mLight->mMatrix.setPosition(lpos.x, lpos.y, lpos.z);

    for (unsigned int i{0}; i < mRenderWindow->mBalls.size(); i++)
        mRenderWindow->mBalls[i]->mSceneNr = mRenderWindow->mSelectedScene;

    //updates scene based on the selected scene
    //Scene 1:
    if (mRenderWindow->mSelectedScene == 0)
    {
        //hides unused objects
        for (unsigned int i{0}; i<5; i++)
        {
            mVisualObjects[i]->bIsHidden = true;
        }

        //displays the correct surface object
        mRenderWindow->mSurfaces[0]->bIsHidden = false;
        mRenderWindow->mSurfaces[1]->bIsHidden = true;
        mRenderWindow->mSurfaces[2]->bIsHidden = true;

        //moves ball
        if (mRenderWindow->mBalls[0]->bIsMoving) mRenderWindow->mBalls[0]->move(0.001f);
    }
    //Scene 2:
    else if (mRenderWindow->mSelectedScene == 1)
    {
        for (unsigned int i{0}; i<5; i++)
        {
            mVisualObjects[i]->bIsHidden = true;
        }
        mRenderWindow->mSurfaces[0]->bIsHidden = true;
        mRenderWindow->mSurfaces[1]->bIsHidden = false;
        mRenderWindow->mSurfaces[2]->bIsHidden = true;

        if (mRenderWindow->mBalls[1]->bIsMoving) mRenderWindow->mBalls[1]->move(0.001f);
        /*
        for (unsigned int i{1}; i < mRenderWindow->mBalls.size(); i++)
        {
            if (mRenderWindow->mBalls[i]->bIsMoving) mRenderWindow->mBalls[i]->move(0.001f);
        }
        */
    }
    //Scene 3:
    else if (mRenderWindow->mSelectedScene == 2)
    {
        for (unsigned int i{0}; i<5; i++) //hides objects that are picked up
        {
            mVisualObjects[i]->bIsHidden = mVisualObjects[i]->bIsPicked;
        }

        mRenderWindow->mSurfaces[0]->bIsHidden = true;
        mRenderWindow->mSurfaces[1]->bIsHidden = true;
        mRenderWindow->mSurfaces[2]->bIsHidden = false;

        //moves the npc along the path and places characters on top of terrain
        if (mRenderWindow->mEnemy->bIsMoving) mRenderWindow->NPCmovement();
        mRenderWindow->followTerrain(*mRenderWindow->mPlayer);
        mRenderWindow->followTerrain(*mRenderWindow->mEnemy);

        for (unsigned int j{0}; j < mRenderWindow->mTrophies.size(); j++)
            mRenderWindow->followTerrain(*mRenderWindow->mTrophies[j]);

        updateColliders(); //updates collider positions

        for (unsigned int k{0}; k < mRenderWindow->mTrophies.size(); k++)
        {
            if (mRenderWindow->mPlayer->testOBBcollision(mRenderWindow->mTrophies[k]))
            {
                //picks up trophy when colliding with player
                mRenderWindow->mTrophies[k]->bIsPicked = true;
            }
        }

        //resets game when player collides with enemy
        if (mRenderWindow->mPlayer->testOBBcollision(mRenderWindow->mEnemy)) mRenderWindow->reset();

        //patrol mode:
        //switches between B-spline curves based on the number of hidden
        //trophies in order to patrol between remaining items
        //whenever end point is reached
        if (mRenderWindow->mEnemy->mState == gsl::PATROL)
        {
            if (!mRenderWindow->mTrophies[0]->bIsHidden && !mRenderWindow->mTrophies[1]->bIsHidden && !mRenderWindow->mTrophies[2]->bIsHidden)
            {
                if (mRenderWindow->mSelectedCurve == 0)
                {
                    if (mRenderWindow->mEnemy->bReachedEndPoint)
                    {
                        mRenderWindow->mSelectedCurve = 7;
                    }
                }
                else if (mRenderWindow->mSelectedCurve == 7)
                {
                    if (mRenderWindow->mEnemy->bReachedEndPoint)
                    {
                        mRenderWindow->mSelectedCurve = 8;
                    }
                }
                else if (mRenderWindow->mSelectedCurve == 8)
                {
                    if (mRenderWindow->mEnemy->bReachedEndPoint)
                    {
                        mRenderWindow->mSelectedCurve = 9;
                    }
                }
                else
                {
                    if (mRenderWindow->mEnemy->bReachedEndPoint)
                    {
                        mRenderWindow->mSelectedCurve = 0;
                    }
                }
            }
            else if (!mRenderWindow->mTrophies[1]->bIsHidden && !mRenderWindow->mTrophies[2]->bIsHidden)
            {
                if (mRenderWindow->mSelectedCurve == 1)
                {
                    if (mRenderWindow->mEnemy->bReachedEndPoint)
                    {
                        mRenderWindow->mSelectedCurve = 10;
                    }
                }
                else
                {
                    if (mRenderWindow->mEnemy->bReachedEndPoint)
                    {
                        mRenderWindow->mSelectedCurve = 1;
                    }
                }
            }
            else if (!mRenderWindow->mTrophies[0]->bIsHidden && !mRenderWindow->mTrophies[2]->bIsHidden)
            {
                if (mRenderWindow->mSelectedCurve == 2)
                {
                    if (mRenderWindow->mEnemy->bReachedEndPoint)
                    {
                        mRenderWindow->mSelectedCurve = 11;
                    }
                }
                else
                {
                    if (mRenderWindow->mEnemy->bReachedEndPoint)
                    {
                        mRenderWindow->mSelectedCurve = 2;
                    }
                }
            }
            else if (!mRenderWindow->mTrophies[0]->bIsHidden && !mRenderWindow->mTrophies[1]->bIsHidden)
            {
                if (mRenderWindow->mSelectedCurve == 3)
                {
                    if (mRenderWindow->mEnemy->bReachedEndPoint)
                    {
                        mRenderWindow->mSelectedCurve = 12;
                    }
                }
                else
                {
                    if (mRenderWindow->mEnemy->bReachedEndPoint)
                    {
                        mRenderWindow->mSelectedCurve = 3;
                    }
                }
            }
            else if (!mRenderWindow->mTrophies[0]->bIsHidden)
            {
                mRenderWindow->mSelectedCurve = 6;
            }
            else if (!mRenderWindow->mTrophies[1]->bIsHidden)
            {
                mRenderWindow->mSelectedCurve = 5;
            }
            else if (!mRenderWindow->mTrophies[2]->bIsHidden)
            {
                mRenderWindow->mSelectedCurve = 4;
            }
            else
                mRenderWindow->mSelectedCurve = 13;
        }
    }
}

void Scene::render()
{
    for (auto asset : mVisualObjects)
    {
        if (!asset->bIsHidden) asset->draw();
    }
}

void Scene::renderNormals(GLint matrixUniform)
{
    for (auto asset : mVisualObjects)
    {
        if (!asset->bIsHidden) asset->drawNormals(matrixUniform);
    }
}

void Scene::updateColliders()
{
    for (unsigned int i{0}; i < mVisualObjects.size(); i++)
    {
        if (mVisualObjects[i]->bCollidable)
        {
            mVisualObjects[i]->setOBB(mVisualObjects[i]->mMatrix.getPosition() - mVisualObjects[i]->mMatrix.getScale(),
                                      mVisualObjects[i]->mMatrix.getPosition() + mVisualObjects[i]->mMatrix.getScale());
        }
    }
}
