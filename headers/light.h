#ifndef LIGHT_H
#define LIGHT_H

#include <glm/vec4.hpp>

class Light
{
public:
    Light() {
        type = POINT;
        homo = glm::vec4(10.0f, 5.0f, 5.0f, 1.0f);
        ambient = glm::vec3(0.1f, 0.1f, 0.1f);
        diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        specular = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    enum LIGHTTYPE {
        POINT,
        DIRECTIONAL,
        SPOT
    };

    glm::vec4 homo;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    LIGHTTYPE type;
};

#endif // LIGHT_H
