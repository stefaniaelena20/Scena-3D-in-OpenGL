#include "Camera.hpp"

namespace gps {

    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->worldUp = cameraUp;

        // Store initial values for reset
        this->initialPosition = cameraPosition;
        this->initialTarget = cameraTarget;

        // Calculate initial front vector
        this->cameraFront = glm::normalize(cameraTarget - cameraPosition);

        // Calculate initial yaw and pitch from front vector
        this->yaw = -90.0f; // Default yaw
        this->pitch = 0.0f; // Default pitch

        // Recalculate from front vector
        glm::vec3 front = this->cameraFront;
        this->yaw = glm::degrees(atan2(front.z, front.x));
        this->pitch = glm::degrees(asin(front.y));

        // Store initial rotation
        this->initialYaw = this->yaw;
        this->initialPitch = this->pitch;

        updateCameraVectors();
    }

    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUpDirection);
    }

    void Camera::move(MOVE_DIRECTION direction, float speed) {
        switch (direction) {
        case MOVE_FORWARD:
            cameraPosition += cameraFront * speed;
            break;
        case MOVE_BACKWARD:
            cameraPosition -= cameraFront * speed;
            break;
        case MOVE_RIGHT:
            cameraPosition += cameraRight * speed;
            break;
        case MOVE_LEFT:
            cameraPosition -= cameraRight * speed;
            break;
        case MOVE_UP:
            cameraPosition += worldUp * speed;
            break;
        case MOVE_DOWN:
            cameraPosition -= worldUp * speed;
            break;
        }
    }

    void Camera::rotate(float pitch, float yaw) {
        this->yaw += yaw;
        this->pitch += pitch;

        // Constrain pitch to prevent flip
        if (this->pitch > 89.0f)
            this->pitch = 89.0f;
        if (this->pitch < -89.0f)
            this->pitch = -89.0f;

        updateCameraVectors();
    }

    void Camera::updateCameraVectors() {
        // Calculate new front vector
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        cameraFront = glm::normalize(front);

        // Recalculate right and up vectors
        cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
        cameraUpDirection = glm::normalize(glm::cross(cameraRight, cameraFront));

        // Update camera target (optional, for compatibility)
        cameraTarget = cameraPosition + cameraFront;
    }

    void Camera::resetCamera() {
        cameraPosition = initialPosition;
        cameraTarget = initialTarget;
        yaw = initialYaw;
        pitch = initialPitch;
        updateCameraVectors();
    }

}