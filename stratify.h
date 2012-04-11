#ifndef STRATIFY_H
#define STRATIFY_H

#include "cloud.h"
#include "TriMesh.h"
#include "octree.h"

class SamplerED {
public:
    SamplerED(int i = 0) 
    { 
        index = i; 
        area = 0.0; 
    }
    operator int() { return index; }
    bool operator==(const SamplerED &other) const
    { return index == other.index; }
    int index;
    double area;
};

typedef Octree<SamplerED, double> SamplerOctree;

struct PointData {
    SamplerOctree *origin;
    SamplerOctree *neighbor[27];
    Vector3 point;
};

extern Array<int> st_dbg_from_face;
extern Array<SamplerOctree *> st_dbg_from_node;

extern SamplerOctree *st_dbg_tree_clone;
extern Array<PointData> st_dbg_cache;

SamplerOctree *StratifiedSample(TriMesh *mesh, size_t level, 
        double lambda, double bad, Cloud *cloud, Array<char> &status);

#endif
