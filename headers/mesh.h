#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SOIL.h>

#include <vector>
#include <string>
#include <map>

#include "shaderprogram.h"
#include "light.h"

namespace makai
{
    enum TextureType {
        diffuse = 0,
        specular = 1
    };

    struct Texture {
        GLuint objectId;
        TextureType type;
        std::string fileName;
    };

    struct VertexAttr {
        // Position
        glm::vec3 Position;
        // Normal
        glm::vec3 Normal;
        // TexCoords
        glm::vec2 TexCoords;
    };

    struct Submesh {
        std::vector<VertexAttr> vertices;
        std::vector<GLuint> indices;
        std::vector<GLuint> texIndices;

        /*  Render data  */
        GLuint VAO, VBO, EBO;

        Submesh(const std::vector<VertexAttr> &vertices,
                  const std::vector<GLuint> &indices,
                  const std::vector<GLuint> &texIndices) :
            VAO(0), VBO(0), EBO(0)
        {
            this->vertices = vertices;
            this->indices = indices;
            this->texIndices = texIndices;
        }
    };

    class Mesh
    {
    public:
        Mesh();
        ~Mesh();

        // Loads a model with supported ASSIMP extensions from file
        // and stores the resulting meshes in the meshes vector.
        bool loadModelFromFile(const std::string &path);

        //call for rendering
        void paint(ShaderProgram* shader, Light* light = nullptr);

        //generate VAO, VBO, TBOs and upload data
        void genBuffers();

        //delete VAO, VBO, TBOs
        void deleteBuffers();

        //delete VAO, VBO, TBOs and clean all mesh data
        void clear();

        static GLuint textureFromFile(const std::string &fileName);
    private:
        /*  Model Data  */
        std::vector<Submesh*> m_meshes;
        /*  the directory containing texture images  */
        std::string directoryOfTex;
        // Stores all the textures loaded so far,
        // optimization to make sure textures aren't loaded more than once.
        std::vector<Texture> m_textures;
        // map from assing texture type to my Texture type
        std::map<int, int> typeMap;

        void genVertexBuffers(Submesh &mesh);


        // Processes a node in a recursive fashion.
        // Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void processNode(const aiNode *node, const aiScene* scene);
        Submesh *processMesh(const aiMesh* mesh, const aiScene* scene);

        // Checks all material textures of a given type and loads the textures if they're not loaded yet.
        // The required info is returned as a Texture struct.
        std::vector<GLuint> loadMaterialTextures(const aiMaterial* mat,
                                                 aiTextureType type);
    };
}


#endif // MESH_H
