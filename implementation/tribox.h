#ifndef TRIBOX_H
#define TRIBOX_H

#include "bbox.h"
#include "vector.h"

bool Overlap(const Vector3 &a, const Vector3 &b, const Vector3 &c, 
        BBox3 &box); 

#endif
