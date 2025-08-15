#ifndef WORLD_H
#define WORLD_H

#include "../Common/main.h"

#include "../Chunk/main.h"
#include "../Common/main.h"

class Shader;

class World {
    public:
        int range;

        World(int range);
        void create();
        Chunk* getChunk(int x, int y, int z);
        void addBlock(const Vec3i &pos);
        void removeBlock(const Vec3i &pos);
        void draw(Shader &shader, const glm::mat4 &projection, const glm::mat4 &view, const glm::vec3 &cameraPos);
        void highlight(Shader &shader, const Vec3i &pos, const glm::mat4 &projection, const glm::mat4 &view);
        bool hasBlockAt(const Vec3i &pos) const;
    
    private:
        unsigned int VAO, VBO;
        unsigned int border_VAO, border_VBO;
        std::map<Vec3i, Chunk> world;
        void check();
};

#endif // WORLD_H