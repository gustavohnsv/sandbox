#ifndef CAMERA_H
#define CAMERA_H

#include "../Common/main.h"

enum cameraAction {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SENS = 0.01f;
const float SPEED = 2.5f;
const float ZOOM = 90.0f;

class World;

struct RayCastResult {
    bool hit;
    Vec3i blockPos;
    Vec3i lastBlockPos;
};

class Camera {
    public:

        Camera(float width = 800.f, float height = 600.0f, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 6.0f));
        glm::mat4 getView() const;
        glm::mat4 getProj() const;
        glm::vec3 getPos() const;
        void processKeyboard(cameraAction action, float deltaTtime); // ?
        void processMouseMovement(double xpos, double ypos, bool constrainPitch = true);
        void updateCamera();
        RayCastResult detectBlock(const World &world, float maxDistance);
    private:
        glm::vec3 cameraPos;
        glm::vec3 cameraFront;
        glm::vec3 cameraUp;
        glm::vec3 cameraRight;
        glm::mat4 proj;
        float lastX, lastY;
        float yaw, pitch;
        bool firstMovement;

        void check();  
};

#endif // CAMERA_H