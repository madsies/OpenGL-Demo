#pragma once
#include <glm/glm.hpp>
#include <vector>

/*
	Useful important structs for rendering 3D Objects

*/

struct Vertex
{
	glm::vec3 pos;  
	glm::vec3 colour;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

struct AABB // Bounding box
{
	glm::vec3 min;
	glm::vec3 max;
};

struct Ray // For raycasting etc.
{
	glm::vec3 origin;
	glm::vec3 direction; // n
};
