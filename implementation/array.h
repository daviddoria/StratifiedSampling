#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <vector>
#include "error.h"

template <class T>
class Array: public std::vector<T> {
public:
	Array(void) { ; }
	Array(int size): std::vector<T>(size) { ; }
    int GetSize(void) const { 
        return (int) this->size(); 
    }
    int PushBack(const T &value = T()) {
        try { this->push_back(value); }
        catch (...) { return 0; }
        return 1;
    }
    int Reserve(int size) {
        try { this->reserve((size_t) size); }
        catch (...) { return 0; }
        return 1;
    }
    int Resize(int size) {
        try { this->resize((size_t) size); }
        catch (...) { return 0; }
        return 1;
    }
};

#endif
