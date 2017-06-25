#include "mesh.h"

using namespace makai;

Mesh::Mesh() : m_meshes(), directoryOfTex(), m_textures()
{
    typeMap = std::map<int, int>();
    typeMap.insert(std::pair<int, int>(aiTextureType_DIFFUSE, TextureType::diffuse));
    typeMap.insert(std::pair<int, int>(aiTextureType_SPECULAR, TextureType::specular));
}

Mesh::~Mesh()
{
    clear();
}

bool Mesh::loadModelFromFile(const std::string &path)
{
    // Read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate |
                                                   aiProcess_FlipUVs     |
                                                   aiProcess_GenNormals);
    // Check for errors
    if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        return false;
    }
    // Retrieve the directory path of the filepath
    directoryOfTex = path.substr(0, path.find_last_of('/'));

    // Process ASSIMP's root node recursively
    this->processNode(scene->mRootNode, scene);

    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}

void Mesh::addSubMesh(const SubMesh *subMesh)
{
    SubMesh* newMesh = new SubMesh(*subMesh);
    m_meshes.push_back(newMesh);
}

void Mesh::addTexture(const Texture &texture)
{
    m_textures.push_back(texture);
}

void Mesh::paint(ShaderProgram *shader, const std::vector<Light> &lights)
{

    shader->setUniform("numLights", (int)lights.size());
    for (unsigned i = 0; i < lights.size(); i++)
    {
        shader->setArrayUniform("allLights", i, lights.at(i).intensity(), "intensity");
        shader->setArrayUniform("allLights", i, lights.at(i).position(), "position");
        shader->setArrayUniform("allLights", i, 0.1f, "attenuation");
    }

    for (size_t i = 0; i < m_meshes.size(); i++)
    {
        // Bind appropriate textures
        for(GLuint j = 0; j < m_meshes.at(i)->texIndices.size(); j++)
        {
            GLuint index = m_meshes.at(i)->texIndices.at(j);
            // Active proper texture unit before binding
            glActiveTextureARB(GL_TEXTURE0 + j);

            // Retrieve texture number (the N in diffuse_textureN)
            // here, i assume N is always 1
            std::string nameAttr;
            if (m_textures.at(index).type == TextureType::diffuse)
                nameAttr = "texture_diffuse1";
            else if (m_textures.at(index).type == TextureType::specular)
                nameAttr = "texture_specular1";

            shader->setUniform(nameAttr.c_str(), j);

            // And finally bind the texture
            glBindTexture(GL_TEXTURE_2D, m_textures.at(index).objectId);

        }

        glBindVertexArray(m_meshes.at(i)->VAO);
        glDrawElements(GL_TRIANGLES, m_meshes.at(i)->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Always good practice to set everything back to defaults once configured.
        for (GLuint i = 0; i < m_meshes.at(i)->texIndices.size(); i++)
        {
            glActiveTextureARB(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}

void Mesh::genBuffers()
{
    for (size_t i = 0; i < m_meshes.size(); i++) {
        genVertexBuffers(*m_meshes.at(i));
    }
    for (auto& t : m_textures) {
        t.objectId = textureFromFile(t.fileName);
    }
}

void Mesh::deleteBuffers()
{
    for (size_t i = 0; i < m_meshes.size(); i++) {
        glDeleteBuffersARB(1, &(m_meshes.at(i)->VBO));
        glDeleteBuffersARB(1, &(m_meshes.at(i)->EBO));
        glDeleteVertexArrays(1, &(m_meshes.at(i)->VAO));
    }
    for (size_t i = 0; i < m_textures.size(); i++) {
        glDeleteTextures(1, &(m_textures.at(i).objectId));
    }
}

void Mesh::clear()
{
    deleteBuffers();
    for (unsigned i = 0; i < m_meshes.size(); i++) {
        delete m_meshes.at(i);
    }
    m_meshes.clear();
    directoryOfTex.clear();
    m_textures.clear();
}

void Mesh::genVertexBuffers(SubMesh &mesh)
{
    // Create buffers/arrays
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffersARB(1, &mesh.VBO);
    glGenBuffersARB(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    // Load data into vertex buffers
    glBindBufferARB(GL_ARRAY_BUFFER, mesh.VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferDataARB(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), &(mesh.vertices[0]), GL_STATIC_DRAW_ARB);

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned), &(mesh.indices[0]), GL_STATIC_DRAW_ARB);

    // Set the vertex attribute pointers

    // Vertex Positions
    glEnableVertexAttribArrayARB(0);
    glVertexAttribPointerARB(0, 3, GL_FLOAT, GL_FALSE, mesh.step * sizeof(float), (GLvoid*)0);
    // Vertex Normals
    glEnableVertexAttribArrayARB(1);
    glVertexAttribPointerARB(1, 3, GL_FLOAT, GL_FALSE, mesh.step * sizeof(float), (GLvoid*)(sizeof(float) * 3));
    // Vertex Texture Coords
    glEnableVertexAttribArrayARB(2);
    glVertexAttribPointerARB(2, 2, GL_FLOAT, GL_FALSE, mesh.step * sizeof(float), (GLvoid*)(sizeof(float) * 6));

    glBindVertexArray(0);
}

GLuint Mesh::textureFromFile(const std::string &fileName)
{
    //Generate texture ID and load texture data
   GLuint textureID;
   glGenTextures(1, &textureID);
   int width,height;
   unsigned char* image = SOIL_load_image(fileName.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
   // Assign texture to ID
   glBindTexture(GL_TEXTURE_2D, textureID);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
   glGenerateMipmap(GL_TEXTURE_2D);

   // Parameters
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glBindTexture(GL_TEXTURE_2D, 0);
   SOIL_free_image_data(image);

   return textureID;
}

/* functions for load Mesh using Assimp */

void Mesh::processNode(const aiNode *node, const aiScene *scene)
{
    // Process each mesh located at the current node
    for(GLuint i = 0; i < node->mNumMeshes; i++)
    {
        // The node object only contains indices to index the actual objects in the scene.
        // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(this->processMesh(mesh, scene));
    }
    // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(GLuint i = 0; i < node->mNumChildren; i++)
    {
        this->processNode(node->mChildren[i], scene);
    }
}

SubMesh *Mesh::processMesh(const aiMesh *mesh, const aiScene *scene)
{
    // Data to fill
    std::vector<float> vertices;
    std::vector<GLuint> indices;
    std::vector<GLuint> texIndices;

    // Walk through each of the mesh's vertices
    for(GLuint i = 0; i < mesh->mNumVertices; i++)
    {
        // Positions
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);

        // Normals
        vertices.push_back(mesh->mNormals[i].x);
        vertices.push_back(mesh->mNormals[i].y);
        vertices.push_back(mesh->mNormals[i].z);

        // Texture Coordinates
        if(mesh->HasTextureCoords(0)) // Does the mesh contain texture coordinates?
        {
            // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
        }
        else
        {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }

    }

    // Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for(GLuint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // Retrieve all indices of the face and store them in the indices vector
        for(GLuint j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Process materials
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // We assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
        // Same applies to other texture as the following list summarizes:
        // Diffuse: texture_diffuseN
        // Specular: texture_specularN
        // Normal: texture_normalN

        // 1. Diffuse maps
        std::vector<GLuint> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE);
        texIndices.insert(texIndices.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. Specular maps
        std::vector<GLuint> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR);
        texIndices.insert(texIndices.end(), specularMaps.begin(), specularMaps.end());
    }

    // Return a mesh object created from the extracted mesh data
    return new SubMesh(vertices, indices, texIndices, 8);
}

std::vector<GLuint> Mesh::loadMaterialTextures(const aiMaterial *mat, aiTextureType type)
{
    std::vector<GLuint> texIndices;
    for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        //Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        GLboolean skip = false;
        for(GLuint j = 0; j < m_textures.size(); j++)
        {
            if(std::strcmp(m_textures[j].fileName.c_str(), str.C_Str()) == 0)
            {
                texIndices.push_back(j);
                skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   // If texture hasn't been loaded already, load it
            Texture texture;
            texture.objectId = 0;
            texture.type = (TextureType)typeMap.at(type);
            texture.fileName = directoryOfTex + '/' + std::string(str.C_Str());
            texIndices.push_back(m_textures.size());
            m_textures.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }

    }
    return texIndices;
}


