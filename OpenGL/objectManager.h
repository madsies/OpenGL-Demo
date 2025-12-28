#pragma once
#include <vector>
#include "CustomObject.h"


class ObjectManager
{
public:
	ObjectManager();

	int currentObject = 0;
	std::vector<std::unique_ptr<Object>> opaqueObjectArray;
	std::vector<std::unique_ptr<Object>> transparentObjectArray; // Separated since transparent objects need to be ordered correctly.

	inline int registerMeshObject(Mesh& mesh);
	void inline updateObjects(float deltaTime);

	void inline renderObjects(GLuint program);
	Object* getObject(int id);

};

/*
	Registers an object into the main object array.
*/

inline ObjectManager::ObjectManager()
{
}

int ObjectManager::registerMeshObject(Mesh& mesh)
{
	auto meshObj = std::make_unique<CustomObject>(mesh);
	currentObject++;

	opaqueObjectArray.push_back(std::move(meshObj));
	return currentObject-1;
}

/*
	Grabs Object from array
*/

Object* ObjectManager::getObject(int id)
{
	if (id >= opaqueObjectArray.size())
		return nullptr; // invalid ID
	return opaqueObjectArray[id].get();
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