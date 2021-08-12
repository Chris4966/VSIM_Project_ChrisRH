#ifndef BSPLINECURVE_H
#define BSPLINECURVE_H

#include <vector>
#include "vec3.h"
#include "visualobject.h"

class BSplineCurve : public VisualObject
{
public:
    BSplineCurve(std::vector<gsl::vec3> controls, std::vector<float> knots, int degree);
    ~BSplineCurve() override;

    void init() override;
    void draw() override;

    int findKnotInterval(float x);
    gsl::vec3 evaluateBSpline(float x);

    std::vector<gsl::vec3> mControls, mCurvePts;
    std::vector<float> mKnots;
    int n, d;

};

#endif // BSPLINECURVE_H
