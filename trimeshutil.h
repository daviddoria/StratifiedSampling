#ifndef TRIMESHUTIL_H
#define TRIMESHUTIL_H

#include "TriMesh.h"
#include "array.h"
#include "octree.h"
#include "tribox.h"
#include "boxbox.h"

template <class T>
static inline
T FindFaceAreas(TriMesh *m, Array<T> &faceareas)
{
    m->need_faces();
    m->need_normals();
	int nf = m->faces.size();
	faceareas.resize(nf);
    T s = 0;
	for (int i = 0; i < nf; i++) {
		const TriMesh::Face &f = m->faces[i];
        vec v0 = m->vertices[f[0]];
        vec v1 = m->vertices[f[1]];
        vec v2 = m->vertices[f[2]];
		vec facenormal = (v1-v0) CROSS (v2-v0); 
        s += (faceareas[i] = 0.5f * len(facenormal));
	}
    return s;
}

static inline
void Normalize(TriMesh *m)
{
    vec c(0, 0, 0);
    int nv = m->vertices.size();
    for (int i = 0; i < nv; i++)
        c += m->vertices[i];
    c /= nv;
    float s = 0;
    for (int i = 0; i < nv; i++) {
        m->vertices[i] -= c;
        s += len2(m->vertices[i]);
    }
    s /= nv;
    for (int i = 0; i < nv; i++)
        m->vertices[i] /= s;
}

#endif
