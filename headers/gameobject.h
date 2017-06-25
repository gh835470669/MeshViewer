#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"

namespace makai
{
    class GameObject
    {
    public:
        GameObject();

        glm::vec3 position() const;
        void setPosition(const glm::vec3 &position);

        glm::vec3 scalar() const;
        void setScalar(const glm::vec3 &scalar);

        glm::vec3 rotation() const;
        void setRotation(const glm::vec3 &rotation);

        void setMesh(Mesh *mesh);

        void setShaderProgram(ShaderProgram *shaderProgram);

        void paint(const std::vector<Light> &lights);
        ShaderProgram *shaderProgram();

    private:
        //the ownership of Mesh is not this gameObject, temperatly
        Mesh *m_mesh;
        //the ownership of ShaderProgram is not this gameObject
        ShaderProgram *m_shaderProgram;

        glm::vec3 m_position;
        glm::vec3 m_scalar;
        glm::vec3 m_rotation;
    };
}



#endif // GAMEOBJECT_H
