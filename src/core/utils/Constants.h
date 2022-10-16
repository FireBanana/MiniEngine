#pragma once
#include <vector>
#include <glm/glm/glm.hpp>

namespace Constants
{
	struct Vertex final
	{
		glm::vec3 position;
		glm::vec3 normals;
		glm::vec2 texCoords;
		glm::vec3 tangent;
	};

	class Primitives
	{	
        public:

        static std::vector<Vertex> CubeVertices;
        static std::vector<Vertex> PlaneVertices;

		static void Initialize();
	};
}