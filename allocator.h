#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

struct Scratch_Allocator {
    char *data = nullptr;
    int used = 0;
    int size = 0;

    inline void *alloc(int n) {
        if(used + n >= size) {
            assert(0);
            return nullptr;
        }

        void *ret = data + used;
        used += n;
        return ret;
    }

    inline void reset() {
        used = 0;
    }

    inline void init(int s) { 
        if(data) shutdown();

        data = (char *)malloc(s);
        size = s;
        used = 0;
    }
    
    inline void shutdown() { 
        if(data) free(data);
        size = 0;
        used = 0;
    }
};

#endif