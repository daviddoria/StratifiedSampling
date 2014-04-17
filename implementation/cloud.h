#ifndef CLOUD_H
#define CLOUD_H

#include "array.h"
#include "vector.h"
#include "bbox.h"

class Cloud {
    public:
        Cloud() { ; }
        virtual ~Cloud() { ; }
        Array<Vector3> vertex;
        Array<Vector3> normal;
};

#endif
