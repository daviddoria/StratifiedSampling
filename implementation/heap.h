//=======================================================================
// Heap Implementation
// 15/11/2002, Diego Nehab
//=======================================================================

#ifndef _HEAP_
#define _HEAP_

#include <cstdio>
#include <assert.h>

class Heap { 
public: 
    Heap(size_t _max): top(0) 
    { 
        heap = new Node[_max+1]; 
        position = new size_t [_max+1];
        max = _max + 1;
        for (size_t i = 0; i < max; i++) {
            position[i] = 0;
            heap[i].key = 0;
            heap[i].priority = 0;
        }
    }

    ~Heap(void)
    {
        delete heap;
        delete position;
    }

    void Swap(size_t i, size_t j) 
    { 
        assert(i > 0 && i <= top);
        assert(j > 0 && j <= top);
        assert(position[heap[i].key] == i && position[heap[j].key] == j);
        Node temp = heap[i]; heap[i] = heap[j]; heap[j] = temp; 
        position[heap[i].key] = i; position[heap[j].key] = j;
    }

    size_t GetSize(void) const
    {
        return top;
    }

    void Down(size_t i) 
    {
        size_t c;
    loop:
        c = i << 1;
        // find smallest child
        if (c < top && heap[c+1].priority < heap[c].priority) c++;
        if (c <= top && heap[i].priority > heap[c].priority) {
            Swap(i, c);
            /* Down(c); tail call. */
            i = c; goto loop;
        }
    }

    void Up(size_t i) 
    {
        size_t c;
    loop:
        c = i >> 1;
        if (c > 0 && heap[i].priority < heap[c].priority) {
            Swap(i, c);
            /* Up(c); tail call. */
            i = c; goto loop;
        }
    }

    void Push(size_t key, double priority)
    {
        top++;
        assert(top < max);
        heap[top].key = key;
        heap[top].priority = priority;
        position[key] = top;
        Up(top);
    }

    size_t Pop(void)
    {
        assert(top > 0);
        size_t key = heap[1].key;
        Swap(1, top);
        top--;
        Down(1);
        position[key] = 0;
        return key;
    }

    int Erase(size_t key)
    {
        // element not there
        assert(key < max);
        size_t where = position[key];
        assert(where < max);
        // element not there
        if (where == 0) return 0;
        // erase it
        Swap(where, top);
        top--;
        if (where < top) { 
            Down(where); 
            Up(where); 
        } 
        position[key] = 0;
        return 1;
    }

    size_t GetTop(double &priority) const
    {
        priority = heap[1].priority;
        return heap[1].key;
    }

    size_t GetTop(void) const
    {
        return heap[1].key;
    }

    void Update(size_t key, double priority)
    {
        assert(key < max);
        size_t where = position[key];
        assert(where > 0);
        assert(where <= top);
        heap[where].priority = priority;
        Up(where); 
        Down(where);
    }

    size_t GetPosition(size_t key)
    {
        return position[key];
    }

private:
    struct Node {
        size_t key;
        double priority;
    };
    size_t top;
    Node *heap;
    size_t *position;
    size_t max;
};

#endif // _HEAP
