#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <cfloat>

#include "objectStructs.h"
#include "object.h"

class Object;
/*
	BVH Tree Node
*/

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
class BVH {

public:
	static AABB computeAABB(const Mesh& mesh, const glm::mat4& model);

	int longestAxis(const AABB& box);

	AABB computeBounds(const std::vector<Object*>& objects, int start, int end);

	BVHNode* buildBVH(std::vector<Object*>& objects, int start, int end);
};