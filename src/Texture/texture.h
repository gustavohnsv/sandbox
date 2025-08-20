#ifndef TEXTURE_H
#define TEXTURE_H

#include "../Common/common.h"

class Texture {
    public:
        unsigned int ID;
        
        Texture(const char* texture);
        void bind(unsigned int slot = 0) const;

    private:
        void check();
};

#endif // TEXTURE_H