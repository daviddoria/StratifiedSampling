#include <cstdio>
#include <cstdlib>
#include <algorithm>

#include "glutwm.h"
#include "matrix.h"
#include "pngimage.h"
#include "modelwindow.h"

ModelWindow::ModelWindow(void) {
    light_ball.Set(Quaternion(0, Vector3(1, 0, 0)));
    name = "dump.png";
    bRotating = false;
}

ModelWindow::
~ModelWindow() {
    ;
}

void
ModelWindow::
Idle(void) {
    float a, x, y, z;
    float b, u, v, w;
    Quaternion r(3, Vector3(0, 1, 0));
    a = GetRotation(&x, &y, &z);
    Quaternion q(a, Vector3(x, y, z));
    q = q * r;
    b = q.GetRotation(&u, &v, &w);
    model_ball.Set(q);
    if (z*w < 0 || y*v < 0 || x*u < 0) {
        debugf(("%g", time.GetElapsed()));
        time.Reset();
    }
    PostRedisplay();
}

void
ModelWindow::
Init(int id) {
    Superclass::Init(id);
}

int 
ModelWindow::
Store(BufferUchar &other, int mag) {
    int old = glutGetWindow();
    glutSetWindow(GetId());
    int width = GetWidth();
    int height = GetHeight();
    if (!other.Resize(width*mag, height*mag, 3)) return 0;
    BufferUchar tmp;
    if (!tmp.Resize(width, height, 3)) return 0;
    glReadBuffer(GL_BACK);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    debugf(("Tiling %dx%d", mag, mag));
    for (int i = 0; i < mag; i++) {
        for (int j = 0; j < mag; j++) {
            SetupMagDraw(i, j, mag);
            DrawModel();
            GlutWMReadPixels(0, 0, width, height, GL_RGB, tmp.GetBuffer());
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    other(i*height + y, j*width + x, 0) = tmp(y, x, 0);
                    other(i*height + y, j*width + x, 1) = tmp(y, x, 1);
                    other(i*height + y, j*width + x, 2) = tmp(y, x, 2);
                }
            }
            debugf(("Tile (%d, %d) done", i, j));
        }
    }
    glReadBuffer(GL_FRONT);
    if (old > 0) glutSetWindow(old);
    return 1;
}


float
ModelWindow::
GetRotation(float *x, float *y, float *z) const {
    return model_ball.GetRotation(x, y, z);
}

void
ModelWindow::
SetRotation(float r, float x, float y, float z) {
    model_ball.Set(Quaternion(r, Vector3(x, y, z)));
}

float 
ModelWindow::
GetLightRotation(float *x, float *y, float *z) const {
    return light_ball.GetRotation(x, y, z);
}

void
ModelWindow::
GetTranslation(float *x, float *y, float *z) const {
    trans.GetTranslation(x, y, z);
}

float
ModelWindow::
GetScale(void) const {
    return model_zoom.GetScale();
}

void 
ModelWindow::
SetCenter(const Vector3& center) {
    this->center = center;
}

void 
ModelWindow::
SetSize(float size) {
    this->size = size;
}

float
ModelWindow::
GetSize(void) {
    return size;
}

const Vector3 &
ModelWindow::
GetCenter(void) {
    return center;
}

void
ModelWindow::
SetupLight(void) {
    GLfloat la[] = {0.0, 0.0, 0.0, 1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, la);
    glEnable(GL_LIGHT0);
    GLfloat ld[] = {0.7, 0.7, 0.7, 0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, ld);
    glLightfv(GL_LIGHT0, GL_SPECULAR, ld);
    glLightfv(GL_LIGHT0, GL_AMBIENT, la);
    // setup light positions from arcball
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float a, x, y, z;
    a = light_ball.GetRotation(&x, &y, &z);
    Matrix3 rotation = Rodrigues(Vector3(x, y, z)*(a*SCALAR_TORADIAN));
    float light[4] = {rotation[2], rotation[5], rotation[8], 0};
    glLightfv(GL_LIGHT0, GL_POSITION, light); 
}

void 
ModelWindow::
Display(void) {
    SetupDraw();
	DrawModel();
}

void
ModelWindow::
SetupPick(int mx, int my) {
    int w = GetWidth(), h = GetHeight();
    // adjust viewport as usual
    SetupViewport();
    // adjust projection matrix for picking
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluPickMatrix(mx, my, 10, 10, viewport);
    float a = model_zoom.GetScale();
    if (w < h) glOrtho(-0.5/a, 0.5/a, -0.5*h/(w*a), 0.5*h/(w*a), -1, 1);
    else glOrtho(-0.5*w/(h*a), 0.5*w/(h*a), -0.5/a, 0.5/a, -1, 1);
    // adjust modelview matrix as usual
    SetupModelView();
}

void
ModelWindow::
SetupModelView(void) {
    int w = GetWidth(), h = GetHeight();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // apply translation, rotation and scale to model
    float a, x, y, z;
    trans.GetTranslation(&x, &y, &z);
    a = model_zoom.GetScale();
	if (w < h) glTranslated(x/a, y*h/(w*a), z/a);
    else glTranslated(x*w/(h*a), y/a, z/a);
    a = model_ball.GetRotation(&x, &y, &z);
    glRotated(a, x, y, z);
    glScaled(1/size, 1/size, 1/size);
    glTranslated(-center[0], -center[1], -center[2]);
}

void
ModelWindow::
SetupProjection(void) {
    SetupMagProjection(0, 0, 1);
}

void
ModelWindow::
SetupMagProjection(float i, float j, float mag) {
    int w = GetWidth(), h = GetHeight();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float a = model_zoom.GetScale();
    if (w < h) glOrtho(
            (-0.5+j/mag)/a, 
            (-0.5+(j+1)/mag)/a, 
            (-0.5+i/mag)*h/(w*a), 
            (-0.5+(i+1)/mag)*h/(w*a), -1, 1);
    else glOrtho(
            (-0.5+j/mag)*w/(h*a), 
            (-0.5+(j+1)/mag)*w/(h*a), 
            (-0.5+i/mag)/a, 
            (-0.5+(i+1)/mag)/a, -1, 1);
}

void
ModelWindow::
SetupViewport(void) {
    int w = GetWidth(), h = GetHeight();
    int x0 = GetX0(), y0 = GetY0();
    glViewport(x0, y0, w, h);
}

void 
ModelWindow::
SetupDraw(void) {
    SetupViewport();
    SetupProjection();
    SetupLight();
    SetupModelView();
    SetupUniforms();
    Clear();
}

void 
ModelWindow::
SetupMagDraw(float i, float j, float mag) {
    SetupViewport();
    SetupMagProjection(i, j, mag);
    SetupLight();
    SetupModelView();
    SetupUniforms();
}

void 
ModelWindow::
SetupUniforms(void) {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    float s = (float) (0.3 * specular_zoom.GetScale());
    GLfloat sv[] = {s, s, s, 1};
    glMaterialfv(GL_FRONT, GL_SPECULAR, sv);
    float e = (float) (shininess_zoom.GetScale());
    glMaterialf(GL_FRONT, GL_SHININESS, e);
    float ab[] = {0.5, 0.2, 0.2, 1};
    glMaterialfv(GL_BACK, GL_DIFFUSE, ab);
    float af[] = {0.8, 0.8, 0.8, 1};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, af);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void ModelWindow::
DrawModel(void) {
    ;
}

void ModelWindow::
Clear(void) {
    ;
}

void ModelWindow::
Fit(void)
{
    center = Vector3(0, 0, 0);
    size = 1;
}

void
ModelWindow::
Button(int button, int state, int x, int y) {
    switch (button) {
        case GLUT_LEFT_BUTTON:
			if (glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
            	if (state == GLUT_DOWN) light_ball.Down(x, y);
            	else light_ball.Up(x, y);
			} else {
            	if (state == GLUT_DOWN) model_ball.Down(x, y);
            	else model_ball.Up(x, y);
			}
            break;
        case GLUT_MIDDLE_BUTTON:
            if (state == GLUT_DOWN) trans.Down(x, y);
            else trans.Up(x, y);
            break;
        case GLUT_RIGHT_BUTTON:
			if (glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
                if (state == GLUT_DOWN) shininess_zoom.Down(x, y);
                else shininess_zoom.Up(x, y);
            } else if (glutGetModifiers() & GLUT_ACTIVE_CTRL) {
                if (state == GLUT_DOWN) specular_zoom.Down(x, y);
                else specular_zoom.Up(x, y);
            } else {
                if (state == GLUT_DOWN) model_zoom.Down(x, y);
                else model_zoom.Up(x, y);
            }
            break;
    }
    if (state == GLUT_UP) PostRedisplay();
}

void
ModelWindow::
SetName(const char *name) {
    this->name = name;
}

const char *
ModelWindow::
GetName(void) const {
    return name;
}

void
ModelWindow::
Keyboard(unsigned char c, int x, int y) {
    switch (c)  {
        case 'r':
            bRotating = !bRotating; 
            if (bRotating) StartIdle();
            else StopIdle();
            time.Reset();
            break;
        case 'f':
            // reset viewing transformation
            light_ball.Set(); 
			model_ball.Set(); 
			model_zoom.Set(); 
			trans.Set();
            Fit();
            break;
        case 'W': {
            BufferUchar b;
            Store(b, 5);
            PngImage img;
            img.Load(b);
            debugf(("Saving %s", name));
            img.Store(name);
            break;
        }
        case 'w': {
            BufferUchar b;
            Store(b, 1);
            PngImage img;
            img.Load(b);
            debugf(("Saving %s", name));
            img.Store(name);
            break;
        }
        default:
            Window::Keyboard(c, x, y);
            break;
    }
    PostRedisplay();
}

void
ModelWindow::Motion(int x, int y) {
    model_ball.Motion(x, y);
    light_ball.Motion(x, y);
    model_zoom.Motion(x, y);
    shininess_zoom.Motion(x, y);
    specular_zoom.Motion(x, y);
    trans.Motion(x, y);
    PostRedisplay();
}

void
ModelWindow::
Reshape(int w, int h, int x0, int y0) {
    Window::Reshape(w, h, x0, y0);
    model_ball.Reset(w/2, h/2, MIN(w/2, h/2));
    light_ball.Reset(w/2, h/2, MIN(w/2, h/2));
    model_zoom.Reset(w, h, x0, y0);
    shininess_zoom.Reset(w, h, x0, y0);
    specular_zoom.Reset(w, h, x0, y0);
    trans.Reset(w, h, x0, y0); 
}
