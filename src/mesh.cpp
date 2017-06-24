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

void Mesh::paint(ShaderProgram *shader, Light *light)
{
    if (light != nullptr) {
//        GLint lightAmbientLoc = glGetUniformLocationARB(shaderId, "light.ambient");
//        GLint lightDiffuseLoc = glGetUniformLocationARB(shaderId, "light.diffuse");
//        GLint lightSpecularLoc = glGetUniformLocationARB(shaderId, "light.specular");
//        GLint lightPosLoc = glGetUniformLocationARB(shaderId, "light.position");
//        glUniform3fARB(lightAmbientLoc, light->ambient.x, light->ambient.y, light->ambient.z);
//        glUniform3fARB(lightDiffuseLoc, light->diffuse.x, light->diffuse.y, light->diffuse.z);
//        glUniform3fARB(lightSpecularLoc, light->specular.x, light->specular.y, light->specular.z);
//        glUniform3fARB(lightPosLoc, 10.0f, 5.0f, 5.0f);

        shader->setUniform("light.ambient", light->ambient);
        shader->setUniform("light.diffuse", light->diffuse);
        shader->setUniform("light.specular", light->specular);
        shader->setUniform("light.position", 10.0f, 5.0f, 5.0f);
        shader->setUniform("shininess", 32.0f);
    }

    for (size_t i = 0; i < m_meshes.size(); i++) {
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
    m_meshes.clear();
    directoryOfTex.clear();
    m_textures.clear();
}

void Mesh::genVertexBuffers(Submesh &mesh)
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
    glBufferDataARB(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(VertexAttr), &(mesh.vertices[0]), GL_STATIC_DRAW_ARB);

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(GLuint), &(mesh.indices[0]), GL_STATIC_DRAW_ARB);

    // Set the vertex attribute pointers

    // Vertex Positions
    glEnableVertexAttribArrayARB(0);
    glVertexAttribPointerARB(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttr), (GLvoid*)offsetof(VertexAttr, Position));
    // Vertex Normals
    glEnableVertexAttribArrayARB(1);
    glVertexAttribPointerARB(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttr), (GLvoid*)offsetof(VertexAttr, Normal));
    // Vertex Texture Coords
    glEnableVertexAttribArrayARB(2);
    glVertexAttribPointerARB(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttr), (GLvoid*)offsetof(VertexAttr, TexCoords));

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

Submesh *Mesh::processMesh(const aiMesh *mesh, const aiScene *scene)
{
    // Data to fill
    std::vector<VertexAttr> vertices;
    std::vector<GLuint> indices;
    std::vector<GLuint> texIndices;

    // Walk through each of the mesh's vertices
    for(GLuint i = 0; i < mesh->mNumVertices; i++)
    {
        VertexAttr vertex;
        glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // Positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // Normals
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;
        // Texture Coordinates
        if(mesh->HasTextureCoords(0)) // Does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
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
    return new Submesh(vertices, indices, texIndices);
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


