#pragma once
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>


class FlyingCamera : public Camera 
{
public:
    float yaw,pitch;
    float Speed = 4.0f;
    float mouseSensitivity = 0.15f;

    FlyingCamera(glm::vec3 pos) {
        Position = pos;
        Up = glm::vec3(0.f, 1.f, 0.f);
        yaw = -90.f;
        pitch = 0.f;
        UpdateVectors();
    }

    FlyingCamera() {
        Position = glm::vec3(256.0f, 64.0f, 256.0f);
        Front = glm::vec3(0.0f, 0.0f, -1.0f);
        Up = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    /*
        Generic WASD Movement.
    */

    void ProcessKeyboardInput(GLFWwindow* window, float deltaTime) override 
    {
        float velocity = Speed * deltaTime;
        glm::vec3 right = glm::normalize(glm::cross(Front, Up));
        // Speed boost
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            velocity = Speed * deltaTime * 5;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            Position += Front * velocity;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            Position -= Front * velocity;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            Position -= right * velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            Position += right * velocity;
    }

    /*
        WIP Mouse Movement.
    */

    void ProcessMouseMovement(float xOffset, float yOffset) override
    {
        xOffset *= mouseSensitivity;
        yOffset *= mouseSensitivity;
        yaw += xOffset;
        pitch += yOffset;

        if (pitch > 89.f) pitch = 89.f;
        if (pitch < -89.f) pitch = -89.f;

        UpdateVectors();
    }


    glm::mat4 GetViewMatrix() const override
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

private:
    void UpdateVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        Front = glm::normalize(front);
    }
};