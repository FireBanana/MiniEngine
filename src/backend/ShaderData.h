#ifndef SHADERDATA_H
#define SHADERDATA_H

#include <glm/glm.hpp>

struct SceneBlock
{
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 cameraPosition;
    float testUniform;
};

#endif // SHADERDATA_H
