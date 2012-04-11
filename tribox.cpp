/********************************************************/
/* AABB-triangle overlap test code                      */
/* by Tomas Akenine-Möller                              */
/* Function: int triBoxOverlap(double boxcenter[3],      */
/*          double boxhalfsize[3],double triverts[3][3]); */
/* History:                                             */
/*   2001-03-05: released the code in its first version */
/*   2001-06-18: changed the order of the tests, faster */
/*   2003-12-21: Modified by Diego Nehab                */
/*                                                      */
/* Acknowledgement: Many thanks to Pierre Terdiman for  */
/* suggestions and discussions on how to optimize code. */
/* Thanks to David Hunt for finding a ">="-bug!         */
/********************************************************/
#include <math.h>
#include <stdio.h>
#include "tribox.h"

#define FINDMINMAX(x0,x1,x2,min,max) \
  min = max = x0;   \
  if(x1<min) min=x1;\
  if(x1>max) max=x1;\
  if(x2<min) min=x2;\
  if(x2>max) max=x2;

static 
inline
int planeBoxOverlap(const Vector3 &normal, double d, const Vector3 &maxbox)
{
  int q;
  Vector3 vmin, vmax;
  for(q=0; q<=2 ;q++)
  {
    if(normal[q]>0.0f)
    {
      vmin[q]=-maxbox[q];
      vmax[q]=maxbox[q];
    }
    else
    {
      vmin[q]=maxbox[q];
      vmax[q]=-maxbox[q];
    }
  }
  if (Dot(normal, vmin)+d>0.0f) return 0;
  if (Dot(normal, vmax)+d>=0.0f) return 1;
  
  return 0;
}


/*======================== 0-tests ========================*/
#define AXISTEST_X01(a, b, fa, fb)             \
    p0 = a*v0[1] - b*v0[2];                    \
    p2 = a*v2[1] - b*v2[2];                    \
        if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
    rad = fa * boxhalfsize[1] + fb * boxhalfsize[2];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_X2(a, b, fa, fb)              \
    p0 = a*v0[1] - b*v0[2];                    \
    p1 = a*v1[1] - b*v1[2];                    \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize[1] + fb * boxhalfsize[2];   \
    if(min>rad || max<-rad) return 0;

/*======================== 1-tests ========================*/
#define AXISTEST_Y02(a, b, fa, fb)             \
    p0 = -a*v0[0] + b*v0[2];                   \
    p2 = -a*v2[0] + b*v2[2];                       \
        if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
    rad = fa * boxhalfsize[0] + fb * boxhalfsize[2];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_Y1(a, b, fa, fb)              \
    p0 = -a*v0[0] + b*v0[2];                   \
    p1 = -a*v1[0] + b*v1[2];                       \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize[0] + fb * boxhalfsize[2];   \
    if(min>rad || max<-rad) return 0;

/*======================== 2-tests ========================*/

#define AXISTEST_Z12(a, b, fa, fb)             \
    p1 = a*v1[0] - b*v1[1];                    \
    p2 = a*v2[0] - b*v2[1];                    \
        if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
    rad = fa * boxhalfsize[0] + fb * boxhalfsize[1];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_Z0(a, b, fa, fb)              \
    p0 = a*v0[0] - b*v0[1];                \
    p1 = a*v1[0] - b*v1[1];                    \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize[0] + fb * boxhalfsize[1];   \
    if(min>rad || max<-rad) return 0;

static 
inline
int triBoxOverlap(const Vector3 &boxcenter, const Vector3 &boxhalfsize,
        const Vector3 &t0, const Vector3 &t1, const Vector3 &t2)
{

  /*    use separating axis theorem to test overlap between triangle and box */
  /*    need to test for overlap in these directions: */
  /*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
  /*       we do not even need to test these) */
  /*    2) normal of the triangle */
  /*    3) crossproduct(edge from tri, {x,y,z}-directin) */
  /*       this gives 3x3=9 more tests */
   Vector3 v0, v1, v2;
   double min,max,d,p0,p1,p2,rad,fex,fey,fez;  
   Vector3 normal, e0, e1, e2;

   /* This is the fastest branch on Sun */
   /* move everything so that the boxcenter is in (0,0,0) */
   v0 = t0 - boxcenter;
   v1 = t1 - boxcenter;
   v2 = t2 - boxcenter;

   /* compute triangle edges */
   e0 = v1 - v0;      /* tri edge 0 */
   e1 = v2 - v1;      /* tri edge 1 */
   e2 = v0 - v2;      /* tri edge 2 */

   /* Bullet 3:  */
   /*  test the 9 tests first (this was faster) */
   fex = fabs(e0[0]);
   fey = fabs(e0[1]);
   fez = fabs(e0[2]);
   AXISTEST_X01(e0[2], e0[1], fez, fey);
   AXISTEST_Y02(e0[2], e0[0], fez, fex);
   AXISTEST_Z12(e0[1], e0[0], fey, fex);

   fex = fabs(e1[0]);
   fey = fabs(e1[1]);
   fez = fabs(e1[2]);
   AXISTEST_X01(e1[2], e1[1], fez, fey);
   AXISTEST_Y02(e1[2], e1[0], fez, fex);
   AXISTEST_Z0(e1[1], e1[0], fey, fex);

   fex = fabs(e2[0]);
   fey = fabs(e2[1]);
   fez = fabs(e2[2]);
   AXISTEST_X2(e2[2], e2[1], fez, fey);
   AXISTEST_Y1(e2[2], e2[0], fez, fex);
   AXISTEST_Z12(e2[1], e2[0], fey, fex);

   /* Bullet 1: */
   /*  first test overlap in the {x,y,z}-directions */
   /*  find min, max of the triangle each direction, and test for overlap in */
   /*  that direction -- this is equivalent to testing a minimal AABB around */
   /*  the triangle against the AABB */

   /* test in 0-direction */
   FINDMINMAX(v0[0],v1[0],v2[0],min,max);
   if(min>boxhalfsize[0] || max<-boxhalfsize[0]) return 0;

   /* test in 1-direction */
   FINDMINMAX(v0[1],v1[1],v2[1],min,max);
   if(min>boxhalfsize[1] || max<-boxhalfsize[1]) return 0;

   /* test in 2-direction */
   FINDMINMAX(v0[2],v1[2],v2[2],min,max);
   if(min>boxhalfsize[2] || max<-boxhalfsize[2]) return 0;

   /* Bullet 2: */
   /*  test if the box intersects the plane of the triangle */
   /*  compute plane equation of triangle: normal*x+d=0 */
   normal = Cross(e0 ,e1);
   d=-Dot(normal, v0);  /* plane eq: normal.x+d=0 */
   if(!planeBoxOverlap(&normal[0], d, &boxhalfsize[0])) return 0;

   return 1;   /* box and triangle overlaps */
}

bool 
Overlap(const Vector3 &a, const Vector3 &b, const Vector3 &c, BBox3 &box) 
{
    return triBoxOverlap(box.GetCenter(), box.GetSize()/2, a, b, c);
}
