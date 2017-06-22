#ifndef MESH_H
#define MESH_H

// Std. Includes
#include <string>
#include <vector>
using namespace std;

struct Vertex {
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // TexCoords
    glm::vec2 TexCoords;
};

struct Texture {
    GLuint id;
    string type;
    string path;
};

class Mesh {
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<GLuint> textures;

    /*  Render data  */
    GLuint VAO, VBO, EBO;

    /*  Functions  */
    // Constructor
    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<GLuint> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
    }
};

#endif // MESH_H
