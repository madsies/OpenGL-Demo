#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class DirectionalShadow
{
public:
    static constexpr unsigned int SHADOW_WIDTH = 2048;
    static constexpr unsigned int SHADOW_HEIGHT = 2048;

    GLuint depthFBO = 0;
    GLuint depthMap = 0;

    glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

    float orthoSize = 128.0f;
    float nearPlane = 0.1f;
    float farPlane = 256.0f;
    glm::vec3 lightPos;

    DirectionalShadow()
    {
        init();
    }

    ~DirectionalShadow()
    {
        glDeleteFramebuffers(1, &depthFBO);
        glDeleteTextures(1, &depthMap);
    }

    void init()
    {
        glGenFramebuffers(1, &depthFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float borderColour[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColour);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "Error: FBO not complete" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void updateLightSpaceMatrix(const glm::vec3& lightDirection, const glm::vec3& camPos, const glm::vec3& camFront)
    {
        glm::mat4 lightProjection = glm::ortho(
            -orthoSize, orthoSize,
            -orthoSize, orthoSize,
            nearPlane, farPlane
        );

        glm::vec3 center = camPos + camFront * farPlane; 
        glm::vec3 lightPos = center - lightDirection * farPlane * 2.f; 

        glm::mat4 lightView = glm::lookAt(
            lightPos,
            center, glm::vec3(0.0f, 1.0f, 0.0f)
        );

        lightSpaceMatrix = lightProjection * lightView;
    }

    void beginRender()
    {
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void endRender(unsigned int screenWidth, unsigned int screenHeight)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screenWidth, screenHeight);
    }
};