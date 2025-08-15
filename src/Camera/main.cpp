#include "main.h"
#include "../World/main.h"

Camera::Camera(float width, float height, glm::vec3 pos) {
    yaw = YAW;
    pitch = PITCH;
    lastX = width/2;
    lastY = height/2;
    firstMovement = true;
    cameraPos = pos;
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
    proj = glm::perspective(glm::radians(ZOOM), width / height, 0.1f, 100.0f); 
    updateCamera();
}

glm::mat4 Camera::getView() const {
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

glm::mat4 Camera::getProj() const {
    return this->proj;
}

glm::vec3 Camera::getPos() const {
    return this->cameraPos;
}

void Camera::processKeyboard(cameraAction action, float deltaTtime) {
    float velocity = SPEED * deltaTtime;
    switch (action) {
    case cameraAction::FORWARD:
        cameraPos += cameraFront * velocity;
        break;
    case cameraAction::BACKWARD:
        cameraPos -= cameraFront * velocity;
        break;
    case cameraAction::LEFT:
        cameraPos -= cameraRight * velocity;
        break;
    case cameraAction::RIGHT:
        cameraPos += cameraRight * velocity;
        break;
    case cameraAction::UP:
        cameraPos.y += velocity;
        break;
    case cameraAction::DOWN:
        cameraPos.y -= velocity;
        break;
    default:
        break;
    }
}   

void Camera::processMouseMovement(double xpos, double ypos, bool constrainPitch) {
    if (firstMovement) {
        lastX = xpos;
        lastY = ypos;
        firstMovement = false;
    }
    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    xOffset *= SENS;
    yOffset *= SENS;
    yaw += xOffset;
    pitch += yOffset;
    if (constrainPitch) {
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }
    updateCamera();
}

void Camera::updateCamera() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
    cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}

RayCastResult Camera::detectBlock(const World &world, float maxDistance) {
    Vec3i lastPos = {999, 999, 999};
    float step = 0.5f;
    for (float i = 0.0f; i < maxDistance; i += step) {
        glm::vec3 currentRayPos = cameraPos + (cameraFront * i);
        int bx = (int)round(currentRayPos.x);
        int by = (int)round(currentRayPos.y);
        int bz = (int)round(currentRayPos.z);
        Vec3i key = {bx, by, bz};
        if (world.hasBlockAt(key)) {
            return {true, key, lastPos};
        }
        lastPos = key;
    }
    return {false, {0,0,0}, {0,0,0}};
}

void Camera::check() {
    return;
}