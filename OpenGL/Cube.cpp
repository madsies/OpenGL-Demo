#include "cube.h";

Mesh Cube::generate()
{
	Mesh meshData;

	const float positions[6][4][3] =
	{
		{ {0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, -0.5f} },
		{ {-0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, 0.5f} },
		{ {-0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f} },
		{ {-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f} },
		{ {0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f} },
		{ {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f} }
	};

	const glm::vec3 normals[6] =
	{
		{ 1, 0, 0 },   // +X
		{-1, 0, 0 },   // -X
		{ 0, 1, 0 },   // +Y
		{ 0,-1, 0 },   // -Y
		{ 0, 0, 1 },   // +Z
		{ 0, 0,-1 }    // -Z
	};

	const glm::vec2 uvs[4] =
	{
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
		{0.0f, 1.0f}
	};


	unsigned int vertexOffset = 0;

	for (int face = 0; face < 6; ++face)
	{
		for (int i = 0; i < 4; ++i)
		{
			const auto& pos = positions[face][i];

			Vertex v;
			v.pos = glm::vec3(pos[0], pos[1], pos[2]);
			v.colour = glm::vec3(0.7f, 0.2f, 0.2f);
			v.normal = normals[face];
			v.uv = uvs[i];

			meshData.vertices.push_back(v);
		}

		meshData.indices.push_back(vertexOffset + 2);
		meshData.indices.push_back(vertexOffset + 1);
		meshData.indices.push_back(vertexOffset + 0);

		meshData.indices.push_back(vertexOffset + 0);
		meshData.indices.push_back(vertexOffset + 3);
		meshData.indices.push_back(vertexOffset + 2);

		vertexOffset += 4;
	}
	return meshData;
}
