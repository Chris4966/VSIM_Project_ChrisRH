#include "bsplinecurve.h"

BSplineCurve::BSplineCurve(std::vector<gsl::vec3> controls, std::vector<float> knots, int degree)
{
    mControls = controls;
    mKnots = knots;
    d = degree;
    n = mControls.size() + d + 1;

    float t{0.f};
    while (t < 1.f)
    {
        gsl::vec3 curve = evaluateBSpline(t);
        mVertices.push_back(Vertex{curve.x, curve.y, curve.z, 1.f, 0.f, 1.f});
        mCurvePts.push_back(curve);
        t += 0.005;
    }

    for (unsigned int i{0}; i < mControls.size(); i++)
    {
        mVertices.push_back(Vertex{mControls[i].x, mControls[i].y, mControls[i].z, 0.f, 0.f, 0.f});
    }

    mMatrix.setToIdentity();
}

BSplineCurve::~BSplineCurve()
{

}

void BSplineCurve::init()
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

    glBindVertexArray(0);
}

void BSplineCurve::draw()
{
    glBindVertexArray( mVAO );
    glDrawArrays(GL_LINE_STRIP, 0, mVertices.size());
}

//finds the current interval between knots
int BSplineCurve::findKnotInterval(float x)
{
    int my = n - 1;
    while (x < mKnots[my] && my > d) my--;
    return my;
}

//Calculates a point on the B-spline using DeBoor's algorithm
gsl::vec3 BSplineCurve::evaluateBSpline(float x)
{
    int my = findKnotInterval(x);

    std::vector<gsl::vec3> pts;
    pts.reserve(n+d+1);

    for (int j{0}; j <= d; j++)
        pts[d-j] = mControls[my-j];

    for (int k{d}; k > 0; k--)
    {
        int j = my - k;
        for (int i{0}; i < k; i++)
        {
            j++;
            float w = (x - mKnots[j])/(mKnots[j+k]-mKnots[j]);
            pts[i] = pts[i] * (1-w) + pts[i+1] * w;
        }
    }
    return pts[0];
}
