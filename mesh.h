#ifndef MESH_H
#define MESH_H

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

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
    //vector<Texture> textures;
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
