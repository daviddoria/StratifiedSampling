/*
Adapted from:
Szymon Rusinkiewicz
Princeton University
pointsample.cc
Return n randomly-distributed samples on the surface of a triangle mesh
*/

#include <stdlib.h>
#include <math.h>

#include <numeric>
#include <algorithm>

#include "trimeshutil.h"
#include "array.h"
#include "pointsample.h"

using std::accumulate;
using std::lower_bound;

#ifdef WIN32
#include <cstdlib>
static double drand48(void)
{
    return ((double) rand())/RAND_MAX;
}
#endif

// Compute the cumulative probability distribution over face areas
static void compute_cumul_prob(const Array<float> &faceareas,
			Array<float> &cumul_prob)
{
	float totarea = accumulate(faceareas.begin(), faceareas.end(), 0.0f);
	float scale = 1.0f / totarea;

	int nf = faceareas.size();
	cumul_prob.resize(nf);
	float area_so_far = 0;
	for (int i = 0; i < nf; i++) {
		area_so_far += scale * faceareas[i];
		cumul_prob[i] = area_so_far;
	}
}

// Store a random point on from a random triangle into the cloud
static void output_random_point(const TriMesh *m, 
        const Array<float> &cumul_prob, Cloud *c)
{
	// Pick a random triangle (area-weighted)
	float key = drand48();
	int tri = lower_bound(cumul_prob.begin(), cumul_prob.end(), key) -
		  cumul_prob.begin();

	// Pick a random point
	float w0 = drand48(), w1 = drand48();
	if (w0 + w1 > 1.0f) {
		w0 = 1.0f - w0;
		w1 = 1.0f - w1;
	}
	float w2 = 1.0f - w0 - w1;

	const point &v0 = m->vertices[m->faces[tri][0]];
	const point &v1 = m->vertices[m->faces[tri][1]];
	const point &v2 = m->vertices[m->faces[tri][2]];

	point p(w0*v0[0] + w1*v1[0] + w2*v2[0],
		    w0*v0[1] + w1*v1[1] + w2*v2[1],
		    w0*v0[2] + w1*v1[2] + w2*v2[2]);

	const vec &n0 = m->normals[m->faces[tri][0]];
	const vec &n1 = m->normals[m->faces[tri][1]];
	const vec &n2 = m->normals[m->faces[tri][2]];
	vec n(w0*n0[0] + w1*n1[0] + w2*n2[0],
		  w0*n0[1] + w1*n1[1] + w2*n2[1],
		  w0*n0[2] + w1*n1[2] + w2*n2[2]);
	normalize(n);

    c->vertex.push_back(Vector3(p[0], p[1], p[2]));
    c->normal.push_back(Vector3(n[0], n[1], n[2]));
}

void PointSample(TriMesh *m, size_t n, Cloud *c)
{
	m->need_normals();
	Array<float> faceareas, cumul_prob;
	c->vertex.resize(0);
	c->normal.resize(0);
    FindFaceAreas(m, faceareas);
	compute_cumul_prob(faceareas, cumul_prob);
	for (size_t i = 0; i < n; i++)
		output_random_point(m, cumul_prob, c);
    //debugf(("Created %d samples", n));
}
