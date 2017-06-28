#include "gameobject.h"

using namespace makai;

GameObject::GameObject() : m_position(0), m_scalar(1), m_rotation()
{

}

glm::vec3 GameObject::position() const
{
    return m_position;
}

void GameObject::setPosition(const glm::vec3 &position)
{
    m_position = position;
}

glm::vec3 GameObject::scalar() const
{
    return m_scalar;
}

void GameObject::setScalar(const glm::vec3 &scalar)
{
    m_scalar = scalar;
}

void GameObject::setRotation(const glm::vec3 &rotation)
{
    glm::quat qx = glm::angleAxis(glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat qy = glm::angleAxis(glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat qz = glm::angleAxis(glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    m_rotation = qy * qx * qz;
}

void GameObject::setRotation(float x, float y, float z)
{
    setRotation(glm::vec3(x, y, z));
}

void GameObject::setMesh(Mesh *mesh)
{
    m_mesh = mesh;
}

void GameObject::setShaderProgram(ShaderProgram *shaderProgram)
{
    m_shaderProgram = shaderProgram;
}

void GameObject::paint(const std::vector<Light> &lights)
{
    glm::mat4 model = glm::translate(glm::mat4( 1.0f ), m_position);
    model = glm::scale(model, m_scalar);
    model = model * glm::mat4_cast(m_rotation);

    if (m_shaderProgram == nullptr || m_mesh == nullptr) return;
    m_shaderProgram->bind();
    m_shaderProgram->setUniform("model", model);
    m_mesh->paint(m_shaderProgram, lights);
}

ShaderProgram *GameObject::shaderProgram()
{
    return m_shaderProgram;
}

void GameObject::rotate(float angle, glm::vec3 axis)
{
    glm::quat q = glm::angleAxis(glm::radians(-angle), glm::normalize(axis));
    m_rotation = q * m_rotation;
}
