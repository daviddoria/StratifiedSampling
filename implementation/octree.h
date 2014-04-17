#ifndef OCTREE_H
#define OCTREE_H

#include "array.h"
#include "bbox.h"
#include "vector.h"
#include "error.h"

template <class T, class U>
class Octree {
public:

    typedef T ElementData;
    typedef U NodeData;

    Octree(BBox3 _box, Octree *_parent = NULL)
    {
        leaf = true; 
        box = _box;
        parent = _parent;
    }

    ~Octree()
    {
        if (!leaf) 
            for (int i = 0; i < 8; i++)
                delete child[i];
    }

    template <class I>
    void Insert(const T &e, I &in)
    {
        // if element is not in our box, ignore
        if (in(e, box)) {
            // if we are in a middle node, pass element to children
            if (!leaf) {
                for (int c = 0; c < 8; c++)
                    child[c]->Insert(e, in); 
            // if we are in a leaf, store new element
            } else element.push_back(e); 
        }
    }

    template <class I>
    int Split(I &in)
    {
        if (!leaf) return 1;
        // get coorner coordinates of all children
        double x[3]; double y[3]; double z[3];
        x[0] = box.GetMin()[0]; y[0] = box.GetMin()[1]; z[0] = box.GetMin()[2];
        x[2] = box.GetMax()[0]; y[2] = box.GetMax()[1]; z[2] = box.GetMax()[2];
        x[1] = (x[0] + x[2])/2; y[1] = (y[0] + y[2])/2; z[1] = (z[0] + z[2])/2;
        // create each of the eight children
        for (int k = 0; k < 2; k++) {
            for (int j = 0; j < 2; j++) {
                for (int i = 0; i < 2; i++) {
                    Vector3 min(x[i], y[j], z[k]);
                    Vector3 max(x[i+1], y[j+1], z[k+1]);
                    child[k*4 + j*2 + i] = new Octree(BBox3(min, max), this);
                }
            }
        }
        // insert the elements into all children
        for (size_t i = 0; i < element.size(); i++)
            for (size_t j = 0; j < 8; j++) 
                child[j]->Insert(element[i], in);
        // make leaf into a sub-node 
        leaf = false;
        // get rid of element data (it is in children)
        element.resize(0);
        // all set
        return 1;
    }

    Octree *Find(const Vector3 &target, int maxdepth = 1000, int depth = 1)
    {
        if (box.In(target)) {
            if (depth < maxdepth && !leaf) {
                Vector3 c = box.GetCenter();
                int i, j, k;
                i = target[0] > c[0];
                j = target[1] > c[1];
                k = target[2] > c[2];
                return child[k*4 + j*2 + i]->Find(target, maxdepth, depth+1);
            } else return this;
        } else return NULL;
    }

    // I know this is inefficient and dirty, but it is simple and effective
    void GetNeighbors(Octree **neighbor)
    {
        // find root
        Octree *root = this;
        int depth = 1;
        while (root->parent) {
            root = root->parent;
            depth++;
        }
        // generate centers of all neighbor bouding boxes
        // for each center, descend from root to 
        // find node in tree that contains the center
        Vector3 center = box.GetCenter();
        Vector3 size = box.GetSize();
        Vector3 dx(size[0], 0, 0);
        Vector3 dy(0, size[1], 0);
        Vector3 dz(0, 0, size[2]);
        for (int k = 0; k < 3; k++) {
            for (int j = 0; j < 3; j++) {
                for (int i = 0; i < 3; i++) {
                    Vector3 target = center + 
                        (i-1.0)*dx + (j-1.0)*dy + (k-1.0)*dz;
                    neighbor[k*9 + j*3 + i] = root->Find(target, depth);
                }
            }
        }
    }

    template <class F>
    void PostOrder(F &f, int level = 1)
    {
        if (!leaf) 
            for (int i = 0; i < 8; i++)
                child[i]->PostOrder(f, level+1);
        f(this, level);
    }

    template <class F>
    void PreOrder(F &f, int level = 1)
    {
        f(this, level);
        if (!leaf) 
            for (int i = 0; i < 8; i++)
                child[i]->PreOrder(f, level+1);
    }

    Array<ElementData> element;
    NodeData data;
    Octree *parent;
    Octree *child[8];
    bool leaf;
    BBox3 box;

};

#endif
