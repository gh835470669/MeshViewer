#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light
{
public:
    enum LightType {
        Point,
        Directional,
        Spot
    };

    Light();

    int type() const;
    void setType(const LightType &type);
    glm::vec4 direction() const;
    void setDirection(float x, float y, float z);
    glm::vec4 position() const;
    void setPosition(float x, float y, float z);
    glm::vec3 intensity() const;
    void setColor(float r, float g, float b);

private:
    glm::vec4 m_direction;
    glm::vec4 m_position;
    glm::vec3 m_intensity;
    LightType m_type;

    float clamp(float input, float min, float max);
};

#endif // LIGHT_H
