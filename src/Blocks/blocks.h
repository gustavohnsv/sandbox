#ifndef BLOCKS_H
#define BLOCKS_H

#include "../Common/common.h"

enum BlockRenderType {
    SOLID,
    TRANSLUCENT,
    WATER,
    CROSS,
    AIR
};

BlockRenderType getBlockRenderType(int type);

class Blocks {
    public:
        Blocks();
        ~Blocks();
        Block getBlockInfo(int type) const;
        std::map<int, Block> getBlockDatabase() const;
    private:
        std::map<int, Block> database;
        void check();
};

#endif // BLOCKS_H