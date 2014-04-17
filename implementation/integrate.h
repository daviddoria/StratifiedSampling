#ifndef INTEGRATE_H
#define INTEGRATE_H

#include "vector.h"
#include "bbox.h"
#include "tribox.h"

// integrates a function over a triangle.
// the functor is passed all triangles and their area
// integration is constrained to a given box
template <class F>
static 
void
Integrate(const Vector3 &v0, const Vector3 &v1, 
        const Vector3 &v2, BBox3 &box, F &f, 
        double minarea, double area = -1)
{
    if (area < 0) area = Norm(Cross(v1 - v0, v2 - v0)/2);
    if (area > minarea) {
        Vector3 m0 = (v1 + v2)/2.0;
        Vector3 m1 = (v0 + v2)/2.0;
        Vector3 m2 = (v1 + v0)/2.0;
        if (Overlap(m0, m1, m2, box)) 
            Integrate(m0, m1, m2, box, f, minarea, area/4);
        if (Overlap(v0, m2, m1, box)) 
            Integrate(v0, m2, m1, box, f, minarea, area/4);
        if (Overlap(v1, m0, m2, box)) 
            Integrate(v1, m0, m2, box, f, minarea, area/4);
        if (Overlap(v2, m1, m0, box))
            Integrate(v2, m1, m0, box, f, minarea, area/4);
    } else f(v0, v1, v2, area);
}

#endif
