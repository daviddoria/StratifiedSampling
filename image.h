#ifndef IMAGE_H_
#define IMAGE_H_

#include "buffer.h"

class Image {
    public:
        virtual ~Image() { ; }
        virtual int Load(const char *name) = 0;
        virtual int Load(const BufferUchar &other) = 0;
        virtual int Store(const char *name) = 0;
        virtual int Store(BufferUchar &other) = 0; 
        virtual int GetWidth(void) const = 0;
        virtual int GetHeight(void) const = 0;
};

#endif // IMAGE_H
