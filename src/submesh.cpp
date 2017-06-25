#include "submesh.h"

using namespace makai;

SubMesh::SubMesh() : SubMesh(std::vector<float>(),
                             std::vector<unsigned>(),
                             std::vector<unsigned>(),
                             0)
{

}

SubMesh::SubMesh(const std::vector<float> &vertices,
                 const std::vector<unsigned> &indices,
                 const std::vector<unsigned> &texIndices,
                 unsigned step) :
    VAO(0), VBO(0), EBO(0)
{
    this->step = step;
    this->vertices = vertices;
    this->indices = indices;
    this->texIndices = texIndices;
}
