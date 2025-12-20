#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

/*
    Camera Base class, Multiple camera types can be built off of it

*/
class Camera {
public:
    virtual ~Camera() = default;

    virtual void ProcessKeyboardInput(GLFWwindow* window, float deltaTime) = 0;
    virtual void ProcessMouseMovement(float xoffset, float yoffset) = 0;
    virtual glm::mat4 GetViewMatrix() const = 0;

    // Key Vectors
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
};