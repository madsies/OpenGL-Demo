#pragma once
#include <vector>
#include "CustomObject.h"


class ObjectManager
{
public:
public:
	ObjectManager(GLuint* buffers, GLuint* vaos, GLuint* ebos);

	int currentObject = 0;
	std::vector<std::unique_ptr<Object>> opaqueObjectArray;
	std::vector<std::unique_ptr<Object>> transparentObjectArray; // Separated since transparent objects need to be ordered correctly.

	void registerMeshObject(Mesh& mesh);
	void updateObjects(float deltaTime);

	void renderObjects(GLuint program);


private:
	GLuint* buffers;
	GLuint* vaos;
	GLuint* ebos;
};

/*
	Registers an object into the main object array.
*/

void ObjectManager::registerMeshObject(Mesh& mesh)
{
	auto meshObj = std::make_unique<CustomObject>(
		mesh,   // transfer mesh ownership
		vaos[0],
		buffers[0],
		ebos[0]
	);

	meshObj->setScale(glm::vec3(2.0f));
	meshObj->setPosition(glm::vec3(0.0f));

	opaqueObjectArray.push_back(std::move(meshObj));
}
/*
	Renders objects in the correct order.
*/

void ObjectManager::renderObjects(GLuint program)
{
	for (const std::unique_ptr<Object>& obj : opaqueObjectArray)
	{
		obj->draw(program);
	}
}

void ObjectManager::updateObjects(float deltaTime)
{
	for (const std::unique_ptr<Object>& obj : opaqueObjectArray)
	{
		obj->update(deltaTime);
	}
}