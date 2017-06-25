#include "gameobject.h"

using namespace makai;

GameObject::GameObject() : m_position(0), m_scalar(1), m_rotation(0)
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

glm::vec3 GameObject::rotation() const
{
    return m_rotation;
}

void GameObject::setRotation(const glm::vec3 &rotation)
{
    m_rotation = rotation;
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
    model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    m_shaderProgram->bind();
    m_shaderProgram->setUniform("model", model);
    m_mesh->paint(m_shaderProgram, lights);
    m_shaderProgram->release();
}

ShaderProgram *GameObject::shaderProgram()
{
    return m_shaderProgram;
}
