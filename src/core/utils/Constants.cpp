#include "Constants.h"
using namespace Constants;

std::vector<Vertex> Primitives::CubeVertices = 
{
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},

    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},

    {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},

    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},

    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},

    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
};

std::vector<Vertex> Primitives::PlaneVertices = 
{
    {{-1.5f, 0.0f, -1.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.5f, 0.0f, -1.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{-1.5f, 0.0f, 1.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{1.5f, 0.0f, 1.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
    {{-1.5f, 0.0f, 1.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{1.5f, 0.0f, -1.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}
};

void Primitives::Initialize()
{
#pragma region CALCULATE_TANGENTS_FOR_CUBE
  {
    for (int i = 0; i < CubeVertices.size(); i += 3) 
    {
      auto &vArray = CubeVertices;
      auto &v0 = const_cast<Vertex &>(vArray[i]);
      auto &v1 = const_cast<Vertex &>(vArray[i + 1]);
      auto &v2 = const_cast<Vertex &>(vArray[i + 2]);

      auto edge1 = v1.position - v0.position;
      auto edge2 = v2.position - v0.position;

      auto deltaU1 = v1.texCoords.x - v0.texCoords.x;
      auto deltaV1 = v1.texCoords.y - v0.texCoords.y;
      auto deltaU2 = v2.texCoords.x - v0.texCoords.x;
      auto deltaV2 = v2.texCoords.y - v0.texCoords.y;

      auto f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

      auto tangent = glm::vec3{f * (deltaV2 * edge1.x - deltaV1 * edge2.x),
                               f * (deltaV2 * edge1.y - deltaV1 * edge2.y),
                               f * (deltaV2 * edge1.z - deltaV1 * edge2.z)};

      v0.tangent = normalize(tangent);
      v1.tangent = normalize(tangent);
      v2.tangent = normalize(tangent);
    }
  }
#pragma endregion

#pragma region CALCULATE_TANGENTS_FOR_PLANE
  {
    for (int i = 0; i < PlaneVertices.size(); i += 3) 
    {
      auto &vArray = PlaneVertices;
      auto &v0 = const_cast<Vertex &>(vArray[i]);
      auto &v1 = const_cast<Vertex &>(vArray[i + 1]);
      auto &v2 = const_cast<Vertex &>(vArray[i + 2]);

      auto edge1 = v1.position - v0.position;
      auto edge2 = v2.position - v0.position;

      auto deltaU1 = v1.texCoords.x - v0.texCoords.x;
      auto deltaV1 = v1.texCoords.y - v0.texCoords.y;
      auto deltaU2 = v2.texCoords.x - v0.texCoords.x;
      auto deltaV2 = v2.texCoords.y - v0.texCoords.y;

      auto f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

      auto tangent = glm::vec3{f * (deltaV2 * edge1.x - deltaV1 * edge2.x),
                               f * (deltaV2 * edge1.y - deltaV1 * edge2.y),
                               f * (deltaV2 * edge1.z - deltaV1 * edge2.z)};

      v0.tangent = normalize(tangent);
      v1.tangent = normalize(tangent);
      v2.tangent = normalize(tangent);
    }
  }
#pragma endregion
}