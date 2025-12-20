#pragma once
#include <glad/glad.h>
#include "objectStructs.h"


class InstancedObjectBatch {
public:
    InstancedObjectBatch(const Mesh& mesh, GLuint vaoID, GLuint vboID, GLuint eboID, size_t maxInstances)
        : vao(vaoID), vbo(vboID), ebo(eboID), indexCount(mesh.indices.size()), maxInstances(maxInstances)
    {
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

        // vert attr
        glEnableVertexAttribArray(0); // pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
        glEnableVertexAttribArray(1); // colour
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, colour));

        // instance buffer
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, maxInstances * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

        for (unsigned int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(2 + i);
            glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
            glVertexAttribDivisor(2 + i, 1);
        }

        glBindVertexArray(0);
    }

    void addInstance(const glm::mat4& model) {
        if (instances.size() < maxInstances)
            instances.push_back(model);
    }

    void updateInstance(size_t i, const glm::mat4& model) {
        if (i < instances.size())
            instances[i] = model;
    }

    void draw(GLuint shaderProgram) {
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glUniform1i(glGetUniformLocation(shaderProgram, "useInstance"), GL_TRUE); // Instance Toggle.
        glBufferSubData(GL_ARRAY_BUFFER, 0, instances.size() * sizeof(glm::mat4), instances.data());

        glBindVertexArray(vao);
        glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, instances.size());
        glBindVertexArray(0);
    }

private:
    GLuint vao, vbo, ebo;
    GLuint instanceVBO;
    GLsizei indexCount;
    size_t maxInstances;
    std::vector<glm::mat4> instances;
};