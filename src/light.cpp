#include "light.h"

Light::Light() : m_type(LightType::Directional),
    m_direction(-1.0f, -1.0f, -1.0f, 0.0f),
    m_position(1.0f, 1.0f, 1.0f, 1.0f),
    m_intensity(1.0f, 1.0f, 1.0f)
{

}

void Light::setColor(float r, float g, float b)
{
    clamp(r, 0.0f, 1.0f);
    clamp(g, 0.0f, 1.0f);
    clamp(b, 0.0f, 1.0f);
    m_intensity = glm::vec3(r, g, b);
}

void Light::setPosition(float x, float y, float z)
{
    m_position = glm::vec4(x, y, z, 1.0f);
}

void Light::setDirection(float x, float y, float z)
{
    m_direction = glm::vec4(x, y, z, 0.0f);
}

int Light::type() const
{
    return m_type;
}

void Light::setType(const LightType &type)
{
    m_type = type;
}

glm::vec4 Light::direction() const
{
    return m_direction;
}

glm::vec4 Light::position() const
{
    return m_position;
}

glm::vec3 Light::intensity() const
{
    return m_intensity;
}

float Light::clamp(float input, float min, float max)
{
    if (input < min) return min;
    if (input > max) return max;
    return input;
}
