#ifndef BUFFER_H
#define BUFFER_H

#include <cstdlib>
#include <cfloat>

#include "error.h"
#include "scalar.h"

template <class T> class Buffer {
public:
    Buffer(void) 
    { 
        buffer = NULL; 
        capacity = width = height = dimension = 0;
    }

    ~Buffer() { if (buffer) free(buffer); }

    void Clear(const T &v = T())
    {
        if (buffer) {
            for (int i = 0; i < width*height*dimension; i++)
                buffer[i] = v;
        }
    }

    void Flip(void)
    {
        for (int y = 0; y < height/2; y++) {
            for (int x = 0; x < width; x++) {
                for (int d = 0; d < dimension; d++) {
                    Swap(buffer[(y*width + x)*dimension+d], 
                            buffer[((height-y-1)*width + x)*dimension+d]);
                }
            }
        }
    }

    template <class U>
    int Store(Buffer<U> &other) const
    {
        if (!other.Resize(width, height, dimension)) return 0;
        U *op = other.GetBuffer();
        for (int i = 0; i < width*height*dimension; i++) 
            op[i] = (U) buffer[i];
        return 1;
    }

    template <class U>
    int Store(U *other, int ps = 1, int rs = 0, int d = 0) const
    {
        const T* data = buffer + d;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                *other = (U) *data;
                other += ps;
                data += dimension;
            }
            other += rs;
        }
        return 1;
    }

    template <class U> 
    int Load(const Buffer<U> &other) 
    { 
        return other.Store(*this); 
    }

    template <class U>
    int Load(const U *other, int ps = 1, int rs = 0, int d = 0)
    {
        T* data = buffer + d;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                *data = (T) *other;
                other += ps;
                data += dimension;
            }
            other += rs;
        }
        return 1;
    }

    int Resize(int w, int h, int d)
    {
        int new_capacity = w * h * d;
        if (Resize(new_capacity, 3*new_capacity)) {
            width = w; 
            height = h; 
            dimension = d;
            size = w * h * d;
            return 1;
        } else return 0;
    }

    T* GetBuffer(int *w = NULL, int *h = NULL, int *d = NULL) const
    {
        if (w) *w = width; 
        if (h) *h = height; 
        if (d) *d = dimension; 
        return buffer;
    }

    int GetSize(void) const { return size; }
    int GetWidth(void) const { return width; }
    int GetHeight(void) const { return height; }
    int GetDimension(void) const { return dimension; }
    int GetCapacity(void) const { return capacity; }

    T &operator[](int i) { return buffer[i]; }
    const T &operator[](int i) const { return buffer[i]; }

    T &operator() (int i, int j, int k = 0) 
    { 
        return buffer[(i*width + j)*dimension + k]; 
    }
    const T &operator() (int i, int j, int k = 0) const
    { 
        return buffer[(i*width + j)*dimension + k]; 
    }

private:

    int Resize(int minimum, int maximum = -1)
    {
        if (capacity < minimum || maximum > 0 && capacity > maximum) {
            T *temp = (T *) realloc(buffer, minimum*sizeof(T));
            if (!temp) {
                errorf(("Error resizing buffer"));
                return 0;
            }
            buffer = temp;
            capacity = minimum;
        }
        return 1;
    }

    template <class U>
    Buffer(const Buffer<U> &other)
    {
        assertf(0, ("Buffer copy constructor used"));
    }

    template <class U>
    Buffer<T> &operator=(const Buffer<U> &other)
    {
        assertf(0, ("Buffer assignment operator used"));
    }

    T *buffer;
    int width, height, dimension;
    int size;
    int capacity;
};

typedef Buffer<Uchar> BufferUchar;
typedef Buffer<Ushort> BufferUshort;
typedef Buffer<Ulong> BufferUlong;
typedef Buffer<float> BufferFloat;

#endif
