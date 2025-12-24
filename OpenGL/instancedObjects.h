#pragma once
#include <glad/glad.h>
#include "objectStructs.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "frustum_culling.h"

struct InstanceChunk {
    glm::vec3 center;
    size_t startIndex;  
    size_t count;       
    float radius;

    std::vector<size_t> indices; 
};

class InstancedObjectBatch {
public:
    static constexpr int MAX_CHUNKS = 32; 
    static constexpr int CHUNK_COUNT = MAX_CHUNKS * MAX_CHUNKS;

    InstancedObjectBatch(const Mesh& mesh, size_t maxInstances,
        const glm::vec3& gridMin, const glm::vec3& gridMax)
        : indexCount(mesh.indices.size()), maxInstances(maxInstances),
        gridMin(gridMin), gridMax(gridMax)
    {
        allInstances.reserve(maxInstances);

        chunkSizeX = (gridMax.x - gridMin.x) / MAX_CHUNKS;
        chunkSizeZ = (gridMax.z - gridMin.z) / MAX_CHUNKS;

        //prealloc chunks
        chunks.resize(CHUNK_COUNT);
        for (auto& chunk : chunks) {
            chunk.startIndex = 0;
            chunk.count = 0;
            chunk.center = glm::vec3(0.0f);
            chunk.radius = 0.0f;
            chunk.indices.clear();
        }

        // Setup
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glGenBuffers(1, &instanceVBO);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0); // pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
        glEnableVertexAttribArray(1); // colour
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, colour));
        glEnableVertexAttribArray(2); // normal
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(3); // uv
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, maxInstances * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

        constexpr GLuint INSTANCE_ATTRIB_START = 4;
        for (unsigned int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(INSTANCE_ATTRIB_START + i);
            glVertexAttribPointer(INSTANCE_ATTRIB_START + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
            glVertexAttribDivisor(INSTANCE_ATTRIB_START + i, 1);
        }

        glBindVertexArray(0);
    }

    void addInstance(const glm::mat4& model) {
        glm::vec3 pos(model[3]);

        int chunkX = static_cast<int>((pos.x - gridMin.x) / chunkSizeX);
        int chunkZ = static_cast<int>((pos.z - gridMin.z) / chunkSizeZ);

        chunkX = glm::clamp(chunkX, 0, MAX_CHUNKS - 1);
        chunkZ = glm::clamp(chunkZ, 0, MAX_CHUNKS - 1);

        int chunkIndex = chunkZ * MAX_CHUNKS + chunkX;

        allInstances.push_back(model);
        size_t idx = allInstances.size() - 1;

        InstanceChunk& chunk = chunks[chunkIndex];
        chunk.indices.push_back(idx);  
        chunk.count++;

        if (chunk.count == 1) {
            chunk.center = pos;
            chunk.radius = 0.0f;
        }
        else {
            float dist = glm::length(pos - chunk.center);
            chunk.radius = std::max(chunk.radius, dist);
        }
    }

    void finalizeChunks() {
        std::vector<glm::mat4> reordered;
        reordered.reserve(allInstances.size());

        size_t currentIndex = 0;
        for (auto& chunk : chunks) {
            chunk.startIndex = currentIndex;

            for (size_t idx : chunk.indices) {
                reordered.push_back(allInstances[idx]);
                currentIndex++;
            }

            chunk.indices.clear(); 
        }

        allInstances = std::move(reordered);
    }

    void draw(GLuint shaderProgram, FCulling* culler) {
        glBindVertexArray(vao);
        glUniform1i(glGetUniformLocation(shaderProgram, "useInstance"), GL_TRUE);

        // Upload instance buffer once per frame
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, allInstances.size() * sizeof(glm::mat4), allInstances.data());

        for (auto& chunk : chunks) {
            if (chunk.count == 0) continue;
            if (culler != nullptr)
                if (!culler->isInFrustum(chunk.center, chunk.radius)) continue;

            glDrawElementsInstancedBaseInstance(
                GL_TRIANGLES,
                indexCount,
                GL_UNSIGNED_INT,
                nullptr,
                static_cast<GLsizei>(chunk.count),
                static_cast<GLuint>(chunk.startIndex)
            );
        }

        glBindVertexArray(0);
    }

private:
    GLuint vao, vbo, ebo;
    GLuint instanceVBO;
    GLsizei indexCount;
    size_t maxInstances;

    std::vector<glm::mat4> allInstances;
    std::vector<InstanceChunk> chunks;

    glm::vec3 gridMin;
    glm::vec3 gridMax;
    float chunkSizeX;
    float chunkSizeZ;
};
