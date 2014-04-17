#ifndef SAMPLE_H
#define SAMPLE_H

#include "vector.h"
#include "bbox.h"

Vector3 UniformPoint(const Vector3 &v0, const Vector3 &v1, 
        const Vector3 &v2);

Vector3 InterpolateNormal(const Vector3 &p, 
        const Vector3 &n0, const Vector3 &v0, const Vector3 &n1, 
        const Vector3 &v1, const Vector3 &n2, const Vector3 &v2);

Vector3 FurthestToPointInTriangle(const Vector3 &p, const Vector3 &v0, 
        const Vector3 &v1, const Vector3 &v2);

bool PointInTriangle(const Vector3 &p, const Vector3 &v0, 
        const Vector3 &v1, const Vector3 &v2);

Vector3 ClosestToPointInTriangle(const Vector3 &p, const Vector3 &v0, 
        Vector3 v1, Vector3 v2);

double ExponentialSample(double a, double min, double max);

bool ExponentialPoint(const Vector3 &p, double lambda, 
        double divisor, const Vector3 &v0, const Vector3 &v1, 
        const Vector3 &v2, BBox3 &box, Vector3 *v);

#endif
