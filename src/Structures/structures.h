#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "../Common/common.h"

class World;

class Structures {
    public:
        Structures();
        void setBlockAcrossChunks(int type, World &world, const Vec3i &pos);
        void placeStructure(int type, World &world, const Vec3i &pos);
    private:
        // std::vector<int> structures; -> seria necessário caso eu quisesse adicionar outro tipo de valor no vetor
        void check();
};

#endif // STRUCTURES_H