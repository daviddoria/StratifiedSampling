#ifndef BOXBOX_H
#define BOXBOX_H

#include "bbox.h"

static inline
bool Overlap(const double &amin, const double &amax,
        const double &bmin, const double &bmax) 
{
    if (bmax < amin || bmin > amax) return false;
    else return true;
}

template <int N>
static inline
bool Overlap(const BBox<N> &a, const BBox<N> &b) 
{
    const Matrix<N, 1> &amin = a.GetMin();
    const Matrix<N, 1> &amax = a.GetMax();
    const Matrix<N, 1> &bmin = b.GetMin();
    const Matrix<N, 1> &bmax = b.GetMax();
    for (int i = 0; i < N; i++) 
        if (!Overlap(amin[i], amax[i], bmin[i], bmax[i]))
            return false;
    return true;
}

#endif
