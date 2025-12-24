#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>

class FCulling
{
public:
	FCulling(glm::mat4 *proj, glm::mat4 *vw) : projection(proj), view(vw) {
	
        glm::mat4 clip = *proj * *vw; // VP
        transform(clip);
	};

    FCulling(glm::mat4 mtrx) {
        transform(mtrx);
    };

    void transform(glm::mat4 clip)
    {
        planes[0] = glm::vec4(
            clip[0][3] + clip[0][0],
            clip[1][3] + clip[1][0],
            clip[2][3] + clip[2][0],
            clip[3][3] + clip[3][0]
        ); // Left

        planes[1] = glm::vec4(
            clip[0][3] - clip[0][0],
            clip[1][3] - clip[1][0],
            clip[2][3] - clip[2][0],
            clip[3][3] - clip[3][0]
        ); // Right

        planes[2] = glm::vec4(
            clip[0][3] - clip[0][1],
            clip[1][3] - clip[1][1],
            clip[2][3] - clip[2][1],
            clip[3][3] - clip[3][1]
        ); // Top

        planes[3] = glm::vec4(
            clip[0][3] + clip[0][1],
            clip[1][3] + clip[1][1],
            clip[2][3] + clip[2][1],
            clip[3][3] + clip[3][1]
        ); // Bottom

        planes[4] = glm::vec4(
            clip[0][3] + clip[0][2],
            clip[1][3] + clip[1][2],
            clip[2][3] + clip[2][2],
            clip[3][3] + clip[3][2]
        ); // Near

        planes[5] = glm::vec4(
            clip[0][3] - clip[0][2],
            clip[1][3] - clip[1][2],
            clip[2][3] - clip[2][2],
            clip[3][3] - clip[3][2]
        ); // Far


        for (int i = 0; i < 6; i++) {
            float len = glm::length(glm::vec3(planes[i]));
            planes[i] /= len;
        }
    }




	bool isInFrustum(glm::vec3 center, float radius)
	{
		for (int i = 0; i < 6; ++i)
		{
			float distance = glm::dot(glm::vec3(planes[i]), center) + planes[i].w;
			if (distance < -radius)
				return false; 
		}
		return true;
	}

private:
	glm::mat4* projection;
	glm::mat4* view;
	glm::mat4 VP;
	glm::vec4 planes[6];
};