#include <glad/glad.h>
#include "CustomObject.h"
#include <glm/gtc/type_ptr.hpp>

glm::mat4 CustomObject::getModelMatrix() const
{
    return modelMatrix;
}

CustomObject::CustomObject(const Mesh& mesh, GLuint vaoID, GLuint vboID, GLuint eboID) 
{
    vao = vaoID;
    vbo = vboID;
    ebo = eboID;
    indexCount = mesh.indices.size();
    glBindVertexArray(vao);

    // vert, idx data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glNamedBufferStorage(vbo, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glNamedBufferStorage(ebo, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), 0);

    // binding
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
    glVertexArrayElementBuffer(vao, ebo);

    // position data
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribBinding(vao, 0, 0);

    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
    // Colour  data
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, colour));
    glVertexArrayAttribBinding(vao, 1, 0);

    glBindVertexArray(0);

    updateModelMatrix();
}

void CustomObject::draw(GLuint shaderProgram)
{
    GLuint modelLocation = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void CustomObject::update(float deltaTime)
{
    return;
}

/*
    Applies transformations in correct order to keep proper shape
*/

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
