#ifndef MODELWINDOW_H
#define MODELWINDOW_H

#include "window.h"
#include "arcball.h"
#include "trans.h"
#include "zoom.h"
#include "bbox.h"
#include "vector.h"
#include "buffer.h"
#include "time.h"

class ModelWindow: public Window {
    public:

        typedef Window Superclass;

        ModelWindow();
        virtual ~ModelWindow();

        virtual void Init(int id);
        virtual void Display(void);
        virtual void Button(int button, int state, int x, int y); 
        virtual void Motion(int x, int y); 
        virtual void Reshape(int w, int h, int x0, int y0);
        virtual void Keyboard(unsigned char c, int x, int y);
        virtual void Idle(void); 

        virtual void SetupLight(void);
        virtual void SetupModelView(void);
        virtual void SetupProjection(void);
        virtual void SetupViewport(void);
        virtual void SetupDraw(void);
        virtual void SetupPick(int mx, int my);
        virtual void SetupMagDraw(float i, float j, float mag);
        virtual void SetupMagProjection(float i, float j, float mag);
        virtual void SetCenter(const Vector3 &center);
        virtual void SetSize(float size);
        virtual float GetSize(void);
        virtual const Vector3& GetCenter(void);
        virtual void SetName(const char *name);
        virtual const char *GetName(void) const;

        virtual float GetLightRotation(float *x, float *y, float *z) const;
        virtual float GetRotation(float *x, float *y, float *z) const;
        virtual void SetRotation(float r, float x, float y, float z);
        virtual float GetScale(void) const;
        virtual void GetTranslation(float *x, float *y, float *z) const;

        virtual void SetupUniforms(void);
        virtual void Clear(void);
        virtual void DrawModel(void);
        virtual void Fit(void);

        virtual int Store(BufferUchar &other, int mag = 1);

    private:
        const char *name;
        Vector3 center;
        float size;
        Arcball model_ball, light_ball;
        Zoom model_zoom, shininess_zoom, specular_zoom;
        Trans trans;
        bool bRotating;
        Time time;
};

#endif // MODELWINDOW_H
