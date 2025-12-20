#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/*

	General Object Interface 

*/

class Object // Interface
{
public:
	virtual ~Object() = default;

	virtual void update(float deltaTime) {}
	virtual void draw(GLuint program) = 0;

	GLuint vao, vbo, ebo;
	GLsizei indexCount;

	glm::vec3 position = {};
	glm::vec3 rotation = {};
	glm::vec3 scale = { 1, 1, 1 };
	
protected:
	glm::mat4 modelMatrix{ 1.0f };

	virtual inline void updateModelMatrix();

};

void Object::updateModelMatrix()
{
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), { 1,0,0 });
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), { 0,1,0 });
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), { 0,0,1 });
	modelMatrix = glm::scale(modelMatrix, scale);
}