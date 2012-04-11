#include <stdlib.h>
#include <math.h>
#include <algorithm>

#include "sample.h"
#include "time.h"
#include "heap.h"
#include "integrate.h"
#include "stratify.h"
#include "boxbox.h"

#ifdef WIN32
#include <cstdlib>
static double drand48(void)
{
    return ((double) rand())/RAND_MAX;
}
#endif

// the global variables are used for debugging
// these can be elminated
Array<int> st_dbg_from_face;
Array<SamplerOctree *> st_dbg_from_node;
// these can all be moved to CleanData
SamplerOctree *st_dbg_tree_clone = NULL;
Array<PointData> st_dbg_cache;

class ExponentialWeight {
    public:
        // box is the bounding box the triangle hits
        // centering tells how much should samples be biased to the center
        ExponentialWeight(const Vector3 &_center, double _lambda, 
                double _divisor) 
        { 
            lambda = _lambda;
            center = _center;
            divisor = _divisor; 
            total = 0;
        }

        void operator() (const Vector3 &v0, const Vector3 &v1, 
                const Vector3 &v2, double a)
        {
            Vector3 v = (v0 + v1 + v2)/3;
            double d = Norm(v - center)/divisor;
            total += a*lambda*exp(-lambda*d);
        }

        double GetTotal() const 
        {
            return total;
        }

    private:
        double lambda;
        Vector3 center;
        double total;
        double divisor;
        double size;
};


class SpreadWeights {
public:
    SpreadWeights(TriMesh *_mesh, double _lambda, double _divisor)
    { 
        mesh = _mesh; 
        lambda = _lambda; 
        divisor = _divisor;
    }
    void operator()(SamplerOctree *node, int level)
    {
        double s = 0;
        if (node->leaf) {
            for (size_t i = 0; i < node->element.size(); i++) {
                int f = node->element[i].index;
                Vector3 v0(&mesh->vertices[mesh->faces[f][0]][0]);
                Vector3 v1(&mesh->vertices[mesh->faces[f][1]][0]);
                Vector3 v2(&mesh->vertices[mesh->faces[f][2]][0]);
                ExponentialWeight g(node->box.GetCenter(), lambda, divisor);
                Integrate(v0, v1, v2, node->box, g, divisor*divisor/25);
                double r = g.GetTotal();
                node->element[i].area = r; 
                s += r; 
            }
        } else {
            for (size_t i = 0; i < 8; i++)
                s += node->child[i]->data;
        }
        node->data = s;
    }
private:
    TriMesh *mesh;
    double lambda;
    double divisor;
};

class SpreadSamples {
public:
    SpreadSamples(TriMesh *_mesh, double _lambda, double _divisor, 
            Cloud* _cloud)
    { 
        mesh = _mesh; 
        cloud = _cloud; 
		lambda = _lambda;
        divisor = _divisor;
    }

    void operator()(SamplerOctree *node, int level)
    {
        if (node->element.size() > 0 && node->data > 0) {
            int j = Roulette(node);
            int f = node->element[j].index;
            int i0 = mesh->faces[f][0];
            int i1 = mesh->faces[f][1];
            int i2 = mesh->faces[f][2];
            // output point
            Vector3 v0(&mesh->vertices[i0][0]);
            Vector3 v1(&mesh->vertices[i1][0]);
            Vector3 v2(&mesh->vertices[i2][0]);
            Vector3 n0(&mesh->normals[i0][0]);
            Vector3 n1(&mesh->normals[i1][0]);
            Vector3 n2(&mesh->normals[i2][0]);
            BBox3 box = node->box;
            Vector3 p = box.GetCenter();
            Vector3 s;
            if (ExponentialPoint(p, lambda, divisor, v0, v1, v2, box, &s)) {
                Vector3 n = InterpolateNormal(s, n0, v0, n1, v1, n2, v2);
                cloud->vertex.push_back(s);
                cloud->normal.push_back(n);
                st_dbg_from_node.push_back(node);
                st_dbg_from_face.push_back(node->element[j].index);
            }
        }
    }

    int Roulette(SamplerOctree *node)
    {
        double target = drand48()*node->data;
        int j = -1;
        while (target > 0) {
            j++;
            target -= node->element[j].area;
        }
        return j;
    }

private:
    TriMesh *mesh;
    Cloud *cloud;
	double lambda;
    double divisor;
};

class FaceIn {
public:
    FaceIn(const TriMesh *_mesh) 
    { mesh = _mesh; }
    bool operator()(SamplerOctree::ElementData i, const BBox3 &box)
    { 
        BBox3 tribox;
        tribox.Merge(Vector3(&mesh->vertices[mesh->faces[(int)i][0]][0])); 
        tribox.Merge(Vector3(&mesh->vertices[mesh->faces[(int)i][1]][0])); 
        tribox.Merge(Vector3(&mesh->vertices[mesh->faces[(int)i][2]][0])); 
        return Overlap(tribox, box);
    }
private:
    const TriMesh *mesh;
};

class SplitUntil {
public:
    SplitUntil(const TriMesh *mesh, int _level): in(mesh)
    { stop = _level; }
    void operator()(SamplerOctree *node, int level)
    {
        if (node->element.size() > 0 && level < stop)
            node->Split(in);
    }
private:
    int stop;
    FaceIn in;
};

SamplerOctree *BuildOctree(TriMesh *m, int level, double *size)
{
    SamplerOctree *tree;
    m->need_bsphere();
    Vector3 center(
        m->bsphere.center[0], 
        m->bsphere.center[1],
        m->bsphere.center[2]
    ); 
    *size = m->bsphere.r / pow(2.0, (level-1.0));
    Vector3 radius(m->bsphere.r, m->bsphere.r, m->bsphere.r);
    BBox3 box(center - radius, center + radius);
    tree = new SamplerOctree(box);
    // Add all items to root
    FaceIn in(m);
    for (size_t i = 0; i < m->faces.size(); i++)
        tree->Insert(SamplerOctree::ElementData(i), in);
    // Split until done target level reached
    SplitUntil split(m, level);
    tree->PreOrder(split);
    return tree;
}

SamplerOctree *CloneOctreeStructure(SamplerOctree *tree)
{
    if (tree) {
        SamplerOctree *clone = new SamplerOctree(tree->box);
        clone->leaf = tree->leaf;
        if (!tree->leaf) {
            for (int i = 0; i < 8; i++)  {
                clone->child[i] = CloneOctreeStructure(tree->child[i]);
                clone->child[i]->parent = clone;
            }
        } 
        return clone;
    } else return NULL;
}

SamplerOctree *PointTree(SamplerOctree *tree, const Array<Vector3> &sample)
{
    SamplerOctree *clone = CloneOctreeStructure(tree);
    for (int i = 0; i < sample.GetSize(); i++) {
        SamplerOctree *n = clone->Find(sample[i]); 
        n->element.push_back(SamplerOctree::ElementData(i));
    }
    return clone;
}

// compute the variance of the distance between the point and
// everyone in it's neighborhood
#if 0
double Metric(int i, Array<PointData> &cache)
{
    double s = 0;
    double s2 = 0;
    int n = 0;
    Vector3 point = cache[i].point; 
    SamplerOctree **neighbor = cache[i].neighbor;
    for (int i = 0; i < 27; i++) {
        SamplerOctree *neigh = neighbor[i];
        if (!neigh) continue;
        for (int j = 0; j < neigh->element.GetSize(); j++) {
            int k = neigh->element[j].index;
            double d2 = Norm2(point - cache[k].point);
            s2 += d2;
            s += sqrt(d2);
            n++;
        }
    }
    return (s2 - s*s)/n;
}

// variance in positions of all points in neighborhood
// after removal of candidate
// we want to remove points that cause variance to reduce 
// the most
double Metric(int i, Array<PointData> &cache)
{
    Vector3 s = -cache[i].point; 
    Vector3 s2 = -Product(s, s); 
    int n = -1;
    SamplerOctree **neighbor = cache[i].neighbor;
    for (int j = 0; j < 27; j++) {
        SamplerOctree *neigh = neighbor[j];
        if (!neigh) continue;
        for (int k = 0; k < neigh->element.GetSize(); k++) {
            Vector3 p = cache[neigh->element[k].index].point;
            s += p;
            s2 += Product(p, p);
            n++;
        }
    }
    Vector3 v = (s2 - Product(s, s))/n;
    return -(v[0] + v[1] + v[2]);
}
#endif

// The closer a point is to the center of mass of the points in it's
// neighborhood, the better. 
double Metric(int i, Array<PointData> &cache)
{
    Vector3 c = -cache[i].point; 
    int n = -1;
    SamplerOctree **neighbor = cache[i].neighbor;
    for (int j = 0; j < 27; j++) {
        SamplerOctree *neigh = neighbor[j];
        if (!neigh) continue;
        for (int k = 0; k < neigh->element.GetSize(); k++) {
            c += cache[neigh->element[k].index].point;
            n++;
        }
    }
    return n > 0? -Norm2(c/n - cache[i].point): 0;
}


// a point is candidate for removal if it is closer than size/2
// to any of it's neighbors
double CandidateForRemoval(int i, Array<PointData> &cache, double size)
{
    double min_d = size*size;
    Vector3 point = cache[i].point; 
    SamplerOctree **neighbor = cache[i].neighbor;
    for (int n = 0; n < 27; n++) {
        SamplerOctree *neigh = neighbor[n];
        if (!neigh) continue;
        for (int j = 0; j < neigh->element.GetSize(); j++) {
            int k = neigh->element[j].index;
            if (k == i) continue;
            double d = Norm2(point - cache[k].point);
            if (d < min_d) return true;
        }
    }
    return false;
}

class PointCache {
public:
    PointCache(Array<PointData> &_cache, const Array<Vector3> &_sample)
    { cache = &_cache; sample = &_sample; }
    void operator()(SamplerOctree *node, int level)
    {
        if (node->leaf) {
            for (int i = 0; i < node->element.GetSize(); i++) {
                int p = node->element[i].index;
                cache->at(p).origin = node;
                cache->at(p).point = sample->at(p);
                node->GetNeighbors(cache->at(p).neighbor);
            }
        }
    }
private:
    Array<PointData> *cache;
    const Array<Vector3> *sample;
};

void RemovePoint(int i, Array<PointData> &cache, double size, Heap &heap)
{
    SamplerOctree **neighbor = cache[i].neighbor;
    SamplerOctree *origin = cache[i].origin;
    origin->element.erase(
            std::remove(origin->element.begin(), origin->element.end(), 
            SamplerOctree::ElementData(i)), origin->element.end());
    for (int n = 0; n < 27; n++) {
        SamplerOctree *neigh = neighbor[n];
        if (!neigh) continue;
        for (int j = 0; j < neigh->element.GetSize(); j++) {
            int k = neigh->element[j].index;
            if (k == i) continue;
            if (heap.Erase(k) && CandidateForRemoval(k, cache, size))
                heap.Push(k, (Metric(k, cache)));
        }
    }
}

// we don't want samples that are less than a minimum distance
// away from some neighbor. we will get rid first of points 
// whose distance to neighbors have the biggest variance
void CleanData(SamplerOctree *tree, double size, 
        const Cloud *cloud, Array<char> &mask)
{
    // clear mask vector
    mask.resize(cloud->vertex.size());
    for (int i = 0; i < mask.GetSize(); i++) 
        mask[i] = 1;
    // clone tree structure and insert points
    if (st_dbg_tree_clone) delete st_dbg_tree_clone;
    st_dbg_tree_clone = PointTree(tree, cloud->vertex);
    // create neighborhood cache
    st_dbg_cache.Resize(cloud->vertex.size());
    PointCache pc(st_dbg_cache, cloud->vertex);
    st_dbg_tree_clone->PostOrder(pc);
    // insert candidates into heap
    Heap heap(cloud->vertex.size());
    for (int i = 0; i < cloud->vertex.GetSize(); i++) {
        if (CandidateForRemoval(i, st_dbg_cache, size)) {
            heap.Push(i, Metric(i, st_dbg_cache));
            mask[i] = 2;
        }
    }
//    debugf(("%d candidates", heap.GetSize()));
    int c = 0;
    while (heap.GetSize() > 0) {
        int k = heap.Pop();
        RemovePoint(k, st_dbg_cache, size, heap);
        mask[k] = 0;
        c++;
    }
  //  debugf(("%d removed", c));
}

SamplerOctree *StratifiedSample(TriMesh *mesh, size_t level, 
        double lambda, double bad, Cloud *cloud, 
        Array<char> &mask)
{
    mesh->need_normals();
    st_dbg_from_face.resize(0);
    st_dbg_from_node.resize(0);
    cloud->vertex.resize(0);
    cloud->normal.resize(0);
//    Time tt, t;
  //  tt.Reset();
//    debugf(("Building tree"));
   // t.Reset();
    double size;
    SamplerOctree *tree = BuildOctree(mesh, level, &size);
  //  debugf(("Done in %gs", t.GetElapsed()));
  //  debugf(("Spreading weights"));
    //t.Reset();
    SpreadWeights spreadweights(mesh, lambda, size);
    tree->PostOrder(spreadweights);
  //  debugf(("Done in %gs", t.GetElapsed()));
  //  debugf(("Spreading samples"));
    //t.Reset();
    SpreadSamples spreadsamples(mesh, lambda, size, cloud);
    tree->PreOrder(spreadsamples);
  //  debugf(("Spread %d samples in in %gs", cloud->vertex.GetSize(), 
//                t.GetElapsed()));
  //  debugf(("Getting rid of bad samples"));
    //t.Reset();
    CleanData(tree, bad*size, cloud, mask);
 //   debugf(("Done in %gs", t.GetElapsed()));
    return tree;
}
