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
	// Space to add extra data (normals, etc.) later. on
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};