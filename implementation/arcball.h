//=======================================================================
// Arcball class implementation.
// 19/7/99, Diego Nehab
//=======================================================================

#ifndef ARCBALL_H
#define ARCBALL_H

#include "quaternion.h"

//=======================================================================
// Arcball Class
//=======================================================================
class Arcball {
public:
    //---------------------------------------------------------------------
    // Constructor
    // Input
    //     xc, yc, r: ball center and radius. defaults to origin, unitary.
    //     qt: initial rotation. defaults to identity.
    //---------------------------------------------------------------------
    Arcball(float xc = 0, float yc = 0, float r = 1, 
            const Quaternion &qt = Quaternion(0, 0, 0, 1));

    //---------------------------------------------------------------------
    // Adjust ball to new dimensions
    // Input
    //     xc, yc, r: ball center and radius
    //---------------------------------------------------------------------
    void Reset(float xc, float yc, float r);

    //---------------------------------------------------------------------
    // Mouse down event
    // Input
    //     x, y: coordinates
    //---------------------------------------------------------------------
    void Down(float x, float y);

    //---------------------------------------------------------------------
    // Mouse motion event
    // Input
    //     x, y: coordinates
    //---------------------------------------------------------------------
    void Motion(float x, float y);

    //---------------------------------------------------------------------
    // Mouse down event
    // Input
    //     x, y: coordinates
    //---------------------------------------------------------------------
    void Up(float x, float y);

    //---------------------------------------------------------------------
    // Set rotation to arbitrary value
    // Input
    //     qt: quaternion for desired rotation
    //---------------------------------------------------------------------
    void Set(const Quaternion &qt = Quaternion(0, 0, 0, 1));

    //---------------------------------------------------------------------
    // Get current rotation as a quaternion. 
    //---------------------------------------------------------------------
    Quaternion Get(void) const;

    //---------------------------------------------------------------------
    // Get cumulative rotation
    //---------------------------------------------------------------------
    float GetRotation(Vector3 *v) const;
    float GetRotation(float *x, float *y, float *z) const;

private:

    Vector3 getv(float x, float y);

    // are we dragging?
    int drag;
    // ball center and radius
    float xc, yc, r;
    // initial and final points
    Vector3 ei, ef;
    // current and cumulative rotations
    Quaternion qt, q;
};

//=======================================================================
// Public Methods
//=======================================================================
inline 
Arcball::Arcball(float uxc, float uyc, float ur, const Quaternion &uqt) {
    drag = false;
    Reset(uxc, uyc, ur);
    Set(uqt);
}

inline void
Arcball::Reset(float uxc, float uyc, float ur) {
    xc = uxc;
    yc = uyc;
    r = ur;
    q = Quaternion(0, 0, 0, 1);
}

inline void
Arcball::Set(const Quaternion &uqt) {
    qt = uqt;
}

inline Quaternion
Arcball::Get(void) const {
    return qt;
}

inline void 
Arcball::Down(float x, float y) {
    // get mouse down position in ball
    ei = getv(x, y);
    // reset current rotation
    q = Quaternion(0, 0, 0, 1);
    // begin dragging
    drag = 1;
}

inline void
Arcball::Up(float x, float y) {
    // make sure we move to final position
    Motion(x, y);
    // add current rotation into cumulative rotation
    qt = q * qt;
    // finish dragging
    drag = 0;
    // reset current rotation
    q = Quaternion(0, 0, 0, 1);
}

inline void
Arcball::Motion(float x, float y) {
    // if no dragging, ignore
    if (!drag) return;
    // get mouse position in ball
    ef = getv(x, y);
    // get rotation axis
    Vector3 v = Cross(ei, ef);
    // get rotation angle
    float a = Dot(ei, ef);
    // create rotation from mouse down position to current position
    q = Quaternion(v[0], v[1], v[2], a);
}

inline float
Arcball::GetRotation(Vector3 *v) const {
    // add current rotation to cumulative rotation
    Quaternion qr = q * qt;
    // return total rotation
    return qr.GetRotation(v);
}

inline float
Arcball::GetRotation(float *x, float *y, float *z) const {
    // add current rotation to cumulative rotation
    Quaternion qr = q * qt;
    // return total rotation
    return qr.GetRotation(x, y, z);
}

//=======================================================================
// Private Methods
//=======================================================================
inline Vector3
Arcball::getv(float x, float y) {
    x = (x - xc) / r;
    y = (y - yc) / r;
    float n = x*x + y*y;
    // if point is out of sphere area
    if (n >= 1) {
        n = sqrt(n);
        return Vector3(x/n, y/n, 0);
    // if point is in sphere area
    } else return Vector3(x, y, sqrt(1-n));
}

#endif // ARCBALL_H
