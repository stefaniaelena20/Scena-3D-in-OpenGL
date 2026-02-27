#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gps {

    enum MOVE_DIRECTION { MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT, MOVE_UP, MOVE_DOWN };

    class Camera
    {
    public:
        // Constructor
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);

        // Get the view matrix
        glm::mat4 getViewMatrix();

        // Camera movement
        void move(MOVE_DIRECTION direction, float speed);

        // Camera rotation
        void rotate(float pitch, float yaw);

        // Get camera position
        glm::vec3 getPosition() const { return cameraPosition; }

        // Get camera front vector
        glm::vec3 getFront() const { return cameraFront; }

        // Reset camera to initial position
        void resetCamera();

    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraFront;
        glm::vec3 cameraRight;
        glm::vec3 cameraUpDirection;
        glm::vec3 worldUp;

        // Rotation angles
        float yaw;
        float pitch;

        // Initial values for reset
        glm::vec3 initialPosition;
        glm::vec3 initialTarget;
        float initialYaw;
        float initialPitch;

        // Update camera vectors based on yaw and pitch
        void updateCameraVectors();
    };

}