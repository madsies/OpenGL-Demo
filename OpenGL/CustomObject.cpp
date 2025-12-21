#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include "CustomObject.h"

CustomObject::CustomObject(const Mesh& mesh, GLuint vaoID, GLuint vboID, GLuint eboID) 
{
    vao = vaoID;
    vbo = vboID;
    ebo = eboID;
    indexCount = mesh.indices.size();
    glBindVertexArray(vao);

    // vert, idx data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glNamedBufferStorage(vbo, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_DYNAMIC_STORAGE_BIT);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glNamedBufferStorage(ebo, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_DYNAMIC_STORAGE_BIT);

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
    glVertexArrayElementBuffer(vao, ebo);

    // Position
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
    glVertexArrayAttribBinding(vao, 0, 0);

    // Colour
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, colour));
    glVertexArrayAttribBinding(vao, 1, 0);

    // Normal
    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glVertexArrayAttribBinding(vao, 2, 0);

    // UV
    glEnableVertexArrayAttrib(vao, 3);
    glVertexArrayAttribFormat(vao, 3, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
    glVertexArrayAttribBinding(vao, 3, 0);


    glBindVertexArray(0);

    updateModelMatrix();
}

glm::mat4 CustomObject::getModelMatrix() const
{
    return modelMatrix;
}

void CustomObject::draw(GLuint shaderProgram)
{
    GLuint modelLocation = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniform1i(glGetUniformLocation(shaderProgram, "useInstance"), GL_FALSE);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void CustomObject::update(float deltaTime)
{
    return;
}

void CustomObject::updateModelMatrix()
{
    glm::mat4 mat = glm::mat4(1.0f);
    mat = glm::translate(mat, position);
    mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    mat = glm::scale(mat, scale);
    modelMatrix = mat;
}

