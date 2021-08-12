#include "landscape.h"
#include "material.h"

Landscape::Landscape(std::string fileName) : mFileName{fileName}
{
    readFile();

    mIndices.clear();
    mTriangles.clear();

    Triangulate();

    init();
    mMatrix.setToIdentity();
}

//Delaunay Triangulation
void Landscape::Triangulate()
{
    //lowers the height of surface by 90
    for (unsigned int i{0}; i < mVertices.size(); ++i)
        mVertices[i].setY(mVertices[i].getY() - 90);

    //min and max points
    gsl::vec2 min, max;

    min.x = mVertices[0].getX();
    min.y = mVertices[0].getZ();
    max = min;

    //calculates min and max values
    for (auto v : mVertices)
    {
        if (v.getX() < min.x) min.x = v.getX();
        else if (v.getX() > max.x) max.x = v.getX();

        if (v.getZ() < min.y) min.y = v.getZ();
        else if (v.getZ() > max.y) max.y = v.getZ();
    }

    qDebug() << "\nBefore Triangulation:";
    qDebug() << "LAS vertex count: " << mVertices.size();
    qDebug() << "LAS min max values: \n X: " << min.x << " " << max.x << "\n Z: " << min.y << " " << max.y;

    //calculates length between mac and min and sets the center position
    gsl::vec2 length = max - min;
    gsl::vec2 center;

    center.x = min.x + (length.x / 2);
    center.y = min.y + (length.y / 2);

    gsl::vec2 dir = -center;

    for (auto &v : mVertices)
    {
        v.setX(v.getX() + dir.x);
        v.setZ(v.getZ() + dir.y);
    }

    min += dir;
    max += dir;

    //offset and table vector
    gsl::vec2 offset{110, 156};
    std::vector<Vertex> table[10][10];

    for (unsigned int i{0}; i < mVertices.size(); ++i)
    {
        int row{-1}, column{-1};

        for (int j{0}; j < 10; ++j)
        {
            if (mVertices[i].getZ() > -730 + (offset.y * j)) ++row;
            if (mVertices[i].getX() > -550 + (offset.x * j)) ++column;
        }
        table[row][column].push_back(mVertices[i]);
    }

    for (int i{0}; i < 10; ++i)
    {
        for (int j{0}; j < 10; ++j)
        {
            int size = table[i][j].size();
            float sh{0};

            if (size > 0)
            {
                for (int k{0}; k < size; ++k)
                {
                    sh += table[i][j][k].getY();

                    if (k == size-1)
                    {
                        table[i][j].clear();
                        table[i][j].push_back(Vertex{(j > 0) ? (-550.f + offset.x * j * offset.x/2) : (-550.f + offset.x/2),
                                                     sh/size,
                                                     (i > 0) ? (-550.f + offset.y * i * offset.y/2) : (-550.f + offset.y/2),
                                                     0.33, 0.33, 0.33,
                                                     (float)j/10, (float)i/10});
                    }
                }
            }
            else
            {
                table[i][j].push_back(Vertex{(j > 0) ? (-550.f + offset.x * j * offset.x/2) : (-550.f + offset.x/2),
                                             0,
                                             (i > 0) ? (-550.f + offset.y * i * offset.y/2) : (-550.f + offset.y/2),
                                             0.33, 0.33, 0.33,
                                             (float)j/10, (float)i/10});
            }
        }
    }


    for (unsigned int i{0}; i < 10; ++i)
    {
        for (unsigned int j{0}; j < 10; ++j)
        {
            for (unsigned int k{0}; k < table[i][j].size(); ++k)
            {
                std::vector<Vertex>& ref = table[i][j];

                ref[k].setX(ref[k].getX() / 5);
                ref[k].setZ(ref[k].getZ() / 5);
            }
        }
    }

    mVertices.clear();
    mIndices.clear();

    for (unsigned int i{0}; i < 10; ++i)
    {
        for (unsigned int j{0}; j < 10; ++j)
        {
            for (unsigned int k{0}; k < table[i][j].size(); ++k)
            {
                std::vector<Vertex>& ref = table[i][j];

                mVertices.push_back(ref[k]);
            }
        }
    }
    qDebug() << "\nAfter Triangulation:";
    qDebug() << "LAS vertex count: " << mVertices.size();;


    //calculates indices
    for (int i{0}; i < 9; ++i)
    {
        for (int j{0}; j < 9; ++j)
        {
            mIndices.push_back(j + 10 + i*10);
            mIndices.push_back(j + 11 + (i*10));
            mIndices.push_back(j + (i*10));

            mIndices.push_back(j + (i*10));
            mIndices.push_back(j + 11 +(i*10));
            mIndices.push_back(j + 1 + (i*10));
        }
    }

    qDebug() << "LAS index count: " << mIndices.size();

    setTriangles();
    //calcNormals(&mVertices, true);

    qDebug() << "LAS triangle count: " << mTriangles.size();

    min.x = mVertices[0].getX();
    min.y = mVertices[0].getZ();
    max = min;

    //calculates min and max values
    for (auto v : mVertices)
    {
        if (v.getX() < min.x) min.x = v.getX();
        else if (v.getX() > max.x) max.x = v.getX();

        if (v.getZ() < min.y) min.y = v.getZ();
        else if (v.getZ() > max.y) max.y = v.getZ();
    }

    qDebug() << "LAS min max values: \n X: " << min.x << " " << max.x << "\n Z: " << min.y << " " << max.y;

}

void Landscape::init()
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
}

void Landscape::readFile()
{
    std::ifstream in;
    in.open(mFileName.c_str());

    if (in.is_open())
    {
        int n;
        in >> n;

        mVertices.reserve(n);
        for (int i{0}; i < n/3; i++)
        {
            for (int j{0}; j < 3; ++j)
            {
                static float x, y, z;
                static float uv{0};
                uv = j;
                uv /= 3;

                in >> x; in >> z; in >> y;

                mVertices.push_back(Vertex{x, y, z, 0, 0, 0, uv, uv});
            }
        }
        in.close();
        qDebug() << "LAS data " << QString::fromStdString(mFileName) << "successfully read";
    }
}

void Landscape::draw()
{
    glBindVertexArray( mVAO );
    mMaterial->mShader->use(mMaterial);
    glUniformMatrix4fv( mMaterial->mShader->mMatrixUniform, 1, GL_TRUE, mMatrix.constData());   //move to shader???
    glDrawElements(GL_TRIANGLES, mIndiceCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
