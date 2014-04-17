#include "sample.h"
#include "matrix.h"
#include "array.h"
#include "integrate.h"


#include <cstdlib>

Vector3 UniformPoint(const Vector3 &v0, const Vector3 &v1, 
        const Vector3 &v2)
{
	// Pick a random point
	double w0 = drand48(), w1 = drand48();
    // Reflect if needed
	if (w0 + w1 > 1.0f) {
		w0 = 1.0f - w0;
		w1 = 1.0f - w1;
	}
	double w2 = 1.0f - w0 - w1;
	return Vector3(w0*v0[0] + w1*v1[0] + w2*v2[0],
		    w0*v0[1] + w1*v1[1] + w2*v2[1],
		    w0*v0[2] + w1*v1[2] + w2*v2[2]);
}

Vector3 InterpolateNormal(const Vector3 &p, 
        const Vector3 &n0, const Vector3 &v0, const Vector3 &n1, 
        const Vector3 &v1, const Vector3 &n2, const Vector3 &v2)
{
    Vector3 e0 = v2 - v1;
    Vector3 e1 = v0 - v2;
    Vector3 e2 = v1 - v0;
    double w0 = Norm(Cross(e0, p - v2));
    double w1 = Norm(Cross(e1, p - v0));
    double w2 = Norm(Cross(e2, p - v1));
    Vector3 s = w0*n0 + w1*n1 + w2*n2;
    return Normalize(s);
}

class Rank {
public:
    Rank(const Vector3 &_p, double _lambda, double _divisor)
    {
        p = _p;
        lambda = _lambda;
        divisor = _divisor;
        total = 0;
    }
    void operator() (const Vector3 &v0, const Vector3 &v1, 
            const Vector3 &v2, double a)
    {
        Vector3 c = (v0 + v1 + v2)/3;
        double distance = Norm(p - c)/divisor;
        double probability = a*lambda*exp(-lambda*distance);
        candidate.push_back(Candidate(v0, v1, v2, probability));
    }
    void Done(void)
    {
        total = 0;
        for (int i = 0; i < candidate.GetSize(); i++) {
            total += candidate[i].probability;
        }
    }
    bool Sample(Vector3 *v)
    {
        double target = total*drand48();
        int c = 0;
        while (c < candidate.GetSize()) {
            target -= candidate[c].probability;
            if (target < 0) break;
            c++;
        }
        if (c < candidate.GetSize()) {
            *v = UniformPoint(candidate[c].v0,candidate[c].v1,candidate[c].v2);
            return true;
        } else return false;
    }
private:
    class Candidate {
    public:
        Candidate(const Vector3 &_v0, const Vector3 &_v1, const Vector3 &_v2,
                double _probability): v0(_v0), v1(_v1), v2(_v2), 
                probability(_probability) { ; }
        Vector3 v0, v1, v2;
        double probability;
    };
    Array<Candidate> candidate;
    Vector3 p;
    double lambda;
    double divisor;
    double total;

};

bool ExponentialPoint(const Vector3 &p, double lambda, 
        double divisor, const Vector3 &v0, const Vector3 &v1, 
        const Vector3 &v2, BBox3 &box, Vector3 *v)
{
    Rank rank(p, lambda, divisor);
    Integrate(v0, v1, v2, box, rank, divisor*divisor/25);
    rank.Done();
    return rank.Sample(v);
}

// Computes the furthest point in a triangle to a reference point. 
Vector3 FurthestToPointInTriangle(const Vector3 &p, const Vector3 &v0, 
        const Vector3 &v1, const Vector3 &v2)
{
	double d0 = Norm2(v0 - p);
	double d1 = Norm2(v1 - p);
	double d2 = Norm2(v2 - p);
	if (d0 > d1) {
		if (d0 > d2) return v0;
		else return v2;
	} else if (d1 > d2) return v1;
	else return v2;
}

// Checks if a point is inside a triangle
bool PointInTriangle(const Vector3 &p, const Vector3 &v0, 
        const Vector3 &v1, const Vector3 &v2)
{
    // Get edges
    Vector3 e0 = v2 - v1;
    Vector3 e1 = v0 - v2;
    Vector3 e2 = v1 - v0;
    // Test if point is in the same plane as the triangle
    Vector3 n = Cross(e0, e1);
    if (!Equal(Dot(n, v0), Dot(n, p))) return false;
    // Test if point is in correct side of each edge
    Vector3 t0 = Cross(e2, p - v0);
    Vector3 t1 = Cross(e0, p - v1);
    Vector3 t2 = Cross(e1, p - v2);
    double s0 = Dot(t0, t1); 
    double s1 = Dot(t0, t2);
    if (Negative(s0) || Negative(s1)) return false;
    else return true;
}

// Computes the closest point in a triangle to a reference point. 
Vector3 ClosestToPointInTriangle(const Vector3 &p, const Vector3 &v0, 
        Vector3 v1, Vector3 v2)
{
    v1 = v1 - v0;
    v2 = v2 - v0;
    Vector3 pv0 = v0 - p;
    double a = Dot(v1, v1);
    double b = Dot(v1, v2);
    double c = Dot(v2, v2);
    double d = Dot(v1, pv0);
    double e = Dot(v2, pv0);
    double det = a*c - b*b;
    double s = b*e - c*d;
    double t = b*d - a*e;
    if (s+t <= det) {
        if (s < 0) {
            if (t < 0) {
                // minimum on edge t=0 
                if (d < 0) { 
                    t = 0; 
                    s = (-d >= a ? 1 : (d >= 0 ? 0 : -d/a)); 
                // minimum on edge s=0 
                } else { 
                    s = 0; 
                    t = (-e >= c ? 1 : (e >= 0 ? 0 : -e/c)); 
                } 
                // debugf(("%d", 4));
            } else {
                s = 0; 
                t = (e >= 0 ? 0 : (-e >= c ? 1 : -e/c)); 
                // debugf(("%d", 3));
            }
        } else if (t < 0) {
            t = 0; 
            s = (d >= 0 ? 0 : (-d >= a ? 1 : -d/a)); 
            // debugf(("%d", 5));
        } else {
            double invDet = 1/det; 
            s *= invDet; 
            t *= invDet; 
            // debugf(("%d", 0));
        }
    } else {
        if (s < 0) {
            double tmp0 = b + d; 
            double tmp1 = c + e; 
            // minimum on edge s+t=1 
            if (tmp1 > tmp0 ) { 
                double numer = tmp1 - tmp0; 
                double denom = a - 2*b + c; 
                s = (numer >= denom ? 1 : numer/denom); 
                t = 1-s; 
            // minimum on edge s=0 
            } else { 
                s = 0; 
                t = (tmp1 <= 0 ? 1 : (e >= 0 ? 0 : -e/c)); 
            } 
            // debugf(("%d", 2));
        } else if (t < 0) {
            double tmp1 = a + d;
            // minimum on edge s+t=1 
            if (tmp1 > b + e) { 
                double numer = c - d + e - b; 
                double denom = a - 2*b + c; 
                s = (numer >= denom ? 1 : numer/denom); 
                t = 1-s; 
            // minimum on edge t=0 
            } else { 
                t = 0; 
                s = (tmp1 <= 0 ? 1 : (d >= 0 ? 0 : -d/a)); 
            } 
            // debugf(("%d", 6));
        } else {
            double numer = c + e - b - d;
            if ( numer <= 0 ) { 
                s = 0; 
            } else { 
                double denom = a - 2*b + c; // positive quantity 
                s = ( numer >= denom ? 1 : numer/denom ); 
            } 
            t = 1 - s; 
            // debugf(("%d", 1));
        }
    }
    return v0 + s*v1 + t*v2;
}
