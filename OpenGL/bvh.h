#pragma once
/*
#include <glm/glm.hpp>
#include <vector>
#include "objectStructs.h"
#include "object.h"


struct AABB
{
	glm::vec3 min;
	glm::vec3 max;
};

/*
	BVH Tree Node


struct BVHNode
{
	AABB bounding;
	BVHNode* left = nullptr;
	BVHNode* right = nullptr;

	std::vector<int> objectIndex;
};

/*
	Compute area of bounding box
*/
/*
AABB computeAABB(const Mesh& mesh, const glm::mat4& model)
{
	AABB box;
	box.min = glm::vec3(FLT_MAX);
	box.max = glm::vec3(-FLT_MAX);

	for (auto& v : mesh.vertices) {
		glm::vec3 p = glm::vec3(model * glm::vec4(v.pos, 1.0f));
		box.min = glm::min(box.min, p);
		box.max = glm::max(box.max, p);
	}
	return box;
}


int longestAxis(const AABB& box) {
	glm::vec3 size = box.max - box.min;
	if (size.x > size.y && size.x > size.z) return 0;
	if (size.y > size.z) return 1;
	return 2;
}

AABB computeBounds(const std::vector<Object>& objects,
	int start,
	int end)
{
	AABB box;
	box.min = glm::vec3(FLT_MAX);
	box.max = glm::vec3(-FLT_MAX);

	for (int i = start; i < end; i++) {
		box.min = glm::min(box.min, objects[i].bounds.min);
		box.max = glm::max(box.max, objects[i].bounds.max);
	}

	return box;
}

BVHNode* buildBVH(std::vector<Object>& objects, int start, int end) {
	BVHNode* node = new BVHNode();

	// Compute bounds
	node->bounding = computeBounds(objects, start, end);

	int count = end - start;
	if (count <= 2) {
		// Leaf node
		for (int i = start; i < end; i++)
			node->objectIndex.push_back(i);
		return node;
	}

	int axis = longestAxis(node->bounding);

	std::sort(objects.begin() + start, objects.begin() + end,
		[axis](const Object& a, const Object& b) {
			return a.centroid[axis] < b.centroid[axis];
		});

	int mid = start + count / 2;
	node->left = buildBVH(objects, start, mid);
	node->right = buildBVH(objects, mid, end);

	return node;
}
*/