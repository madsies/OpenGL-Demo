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


	unsigned int vertexOffset = 0;

	for (int face = 0; face < 6; ++face)
	{
		for (int i = 0; i < 4; ++i)
		{
			const auto& pos = positions[face][i];

			Vertex v;
			v.pos = glm::vec3(pos[0], pos[1], pos[2]);
			v.colour = glm::vec3(0.7f, 0.2f, 0.2f);

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
