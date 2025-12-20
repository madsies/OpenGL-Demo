#include "object.h"
#include "objectStructs.h"
#include <GLFW/glfw3.h>


class CustomObject : Object
{
public:
    CustomObject(const Mesh& mesh, GLuint vaoID, GLuint vboID, GLuint eboID);
    void draw(GLuint shaderProgram);
    void update(float deltaTime);

    void setPosition(const glm::vec3& pos)
    {
        position = pos;
        updateModelMatrix();
    }

    void setRotation(const glm::vec3& rot)
    {
        rotation = rot;
        updateModelMatrix();
    }

    void setScale(const glm::vec3& scl)
    {
        scale = scl;
        updateModelMatrix();
    }
       
    glm::mat4 getModelMatrix() const;

private:
    GLuint vao, vbo, ebo;
    GLsizei indexCount;

    glm::vec3 position = {};
    glm::vec3 rotation = {};
    glm::vec3 scale = { 1, 1, 1 };

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    void updateModelMatrix();
};