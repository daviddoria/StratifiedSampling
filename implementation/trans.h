#ifndef TRANS_H
#define TRANS_H
//=======================================================================
// Translation class implementation.
// 19/7/2002, Diego Nehab
//=======================================================================

//=======================================================================
// Trans Class
//=======================================================================
class Trans {
public:
    //---------------------------------------------------------------------
    // Constructor
    //---------------------------------------------------------------------
    Trans(float w = 1, float h = 1, float x0 = 0, float y0 = 0);

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
    // Set translation value
    // Input
    //     t: translation amount
    //---------------------------------------------------------------------
    void Set(const Vector3 &t = Vector3(0,0,0));

    //---------------------------------------------------------------------
    // Reset viewport and projection parameters
    //---------------------------------------------------------------------
    void Reset(float w, float h, float x0, float y0);

    //---------------------------------------------------------------------
    // Mouse down event
    // Input
    //     x, y: coordinates
    //---------------------------------------------------------------------
    void Up(float x, float y);

    //---------------------------------------------------------------------
    // Get cumulative translation
    //---------------------------------------------------------------------
    Vector3 GetTranslation(void) const;
    void GetTranslation(float *x, float *y, float *z) const;

private:
    Vector3 getv(float x, float y);
    // are we dragging?
    int drag;
    // cumulative translation, mouse down position and current position
    Vector3 t, p, m; 
    // viewport
    float w, h, x0, y0;
};

//=======================================================================
// Public Methods
//=======================================================================
inline 
Trans::Trans(float w, float h, float x0, float y0) {
    Reset(x0, y0, w, h);
    drag = false;
}

inline void 
Trans::Reset(float w_, float h_, float x0_, float y0_) {
    x0 = x0_;
    y0 = y0_;
    w = w_;
    h = h_;
}

inline void 
Trans::Down(float x, float y) {
    p = m = getv(x, y);
    drag = true;
}

inline void
Trans::Up(float x, float y) {
    t += getv(x, y) - p;
    drag = false;
    m = p = Vector3(0,0,0); 
}

inline void
Trans::Motion(float x, float y) {
    if (drag) m = getv(x, y);
}

inline void
Trans::Set(const Vector3 &ut) {
    t = ut;
}

inline Vector3
Trans::GetTranslation(void) const {
    return t + m - p;
}

inline void 
Trans::GetTranslation(float *x, float *y, float *z) const {
    Vector3 r = t + m - p;
    *x = r[0];
    *y = r[1];
    *z = r[2];
}

inline Vector3
Trans::getv(float x, float y) {
    x = (x - x0)/w - 0.5;
    y = (y - y0)/h - 0.5;
    return Vector3(x, y, 1);
}

#endif // TRANS_H
