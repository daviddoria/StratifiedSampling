#ifndef BUFFEROPS_H
#define BUFFEROPS_H

#include "buffer.h"
#include "scalar.h"

template <class T, class U> 
inline int ToGray(Buffer<T> &obuf, const Buffer<U> &ibuf)
{
    int w, h, d;
    const U *ip = ibuf.GetBuffer(&w, &h, &d);
    if (!obuf.Resize(w, h, 1)) {
        errorf(("Unable to resize output buffer"));
        return 0;
    }
    T *op = obuf.GetBuffer();
    switch (d) {
        case 1:
            return obuf.Load(ibuf);
        case 2:
            return obuf.Load(ip, 2, 0);
        case 3:
        case 4:
            for (int i = 0; i < w*h; i++) {
                Translate(op[0], (U) ((54*ip[0] + 183*ip[1] + 19*ip[2])/256));
                op++;
                ip += d;
            }
            return 1;
        default:
            errorf(("Don't know how to convert %d dimensions to gray", d));
            return 0;
    }
}

template <class T>
inline int Normalize(Buffer<T> &buffer)
{
    double min = DBL_MAX, max = -DBL_MAX;
    int i, w, h, d; 
    T *p = buffer.GetBuffer(&w, &h, &d);
    // first pass to get minimum and maximum
    for (i = 0; i < w*h*d; i++) {
        if (min > *p) min = *p;
        if (max < *p) max = *p;
        p++;
    }
    // second pass to normalize
    p = buffer.GetBuffer();
    double range = max - min;
    if (Zero(range)) {
        min = 0;
        range = Limit(*p);
    }
    for (i = 0; i < w*h*d; i++) {
        *p = (T) (Limit(*p)*(*p - min)/range);
        p++;
    }
    return 1;
}

template <class T>
inline int Negate(Buffer<T> &buffer)
{
    int i, w, h, d; 
    T *p = buffer.GetBuffer(&w, &h, &d);
    // first pass to get minimum and maximum
    for (i = 0; i < w*h*d; i++) {
        *p = Limit(*p) - *p;
        p++;
    }
    return 1;
}

template <class T, class U>
inline int Translate(Buffer<T> &obuf, const Buffer<U> &ibuf)
{
    int w, h, d;
    U *ip = ibuf.GetBuffer(&w, &h, &d);
    if (!obuf.Resize(w, h, d)) return 0;
    T *op = obuf.GetBuffer();
    for (int i = 0; i < w*h*d; i++)
        Translate(op[i], ip[i]);
    return 1;
}

template <class T, class U> 
inline int Normalize(Buffer<T> &obuf, const Buffer<U> &ibuf)
{
    double min = DBL_MAX, max = -DBL_MAX;
    int i, w, h, d; 
    U *ip = ibuf.GetBuffer(&w, &h, &d);
    // allocate output buffer
    if (!obuf.Resize(w, h, d)) return 0;
    // first pass to get minimum and maximum from input buffer
    for (i = 0; i < w*h*d; i++) {
        if (min > *ip) min = *ip;
        if (max < *ip) max = *ip;
        ip++;
    }
    // second pass to normalize and copy
    ip = ibuf.GetBuffer();
    T* op = obuf.GetBuffer();
    double range = max - min;
    if (Zero(range)) {
        min = 0;
        range = Limit(*ip);
    }
    for (i = 0; i < w*h*d; i++) {
        *op = (T) (Limit(*op)*(*ip - min)/range);
        ip++; op++;
    }
    return 1;
}

#endif
