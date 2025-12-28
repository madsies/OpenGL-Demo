#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include "CustomObject.h"
#include "bvh.h"

CustomObject::CustomObject(const Mesh& m)
{
    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &ebo);

    mesh = m;
    indexCount = mesh.indices.size();
    glBindVertexArray(vao);

    // vert, idx data
    glNamedBufferStorage(vbo,mesh.vertices.size() * sizeof(Vertex),mesh.vertices.data(),0);

    glNamedBufferStorage(ebo,mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(),0);

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

void CustomObject::updateModelMatrix(){
    Object::updateModelMatrix();
    bounds = BVH::computeAABB(mesh, modelMatrix);
    centroid = (bounds.min + bounds.max) * 0.5f;
}

void CustomObject::update(float deltaTime)
{
    return;
}