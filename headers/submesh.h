#ifndef SUBMESH_H
#define SUBMESH_H

#include <vector>

namespace makai
{
    class SubMesh
    {
    public:
        SubMesh();
        SubMesh(const std::vector<float> &vertices,
                const std::vector<unsigned> &indices,
                const std::vector<unsigned> &texIndices,
                unsigned step);

        /*  Render data  */
        unsigned VAO, VBO, EBO;

        //VBO step
        unsigned step;

        std::vector<float> vertices;
        std::vector<unsigned> indices;
        std::vector<unsigned> texIndices;
    };
}



#endif // SUBMESH_H
