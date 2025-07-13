#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Mesh.h>
#include <SHADER.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <Model.h>

void Model::Draw(Shader &shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw(shader);
    }
}

void Model::LoadModel(std::string path)
{
    // Delcare an Importer object
    Assimp::Importer import;
    // Call ReadFile
        // File path
        // Post-processing options as second argument
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        // aiProcess_Triangulate: If the model does not entirely consist of triangles, it should transform
        // all the model's primitive shapes to triangles first.

        // aiProcess_FlipUVs: Flips the texture coords on the y-axis where necessary during processing.
        // Since most images in OpenGL are reversed around y-axis.
        /*
            Other useful options:
            -- aiProcess_GenNormals: Creates normal vectors for each vertex if the model doesn't contain
            normal vectors.
            -- aiProcess_SplitLargeMeshes: Splits large meshes into smaller sub-meshes which is useful if your
            rendering has a maximum number of vertices allowed and can only process smaller meshes.
            -- aiProcess_OptimizeMeshes: Does the reverse by trying to join several meshes into one larger mesh,
            reducing drawing calls for optimization.
            Additional options can be found here: http://assimp.sourceforge.net/lib_html/postprocess_8h.html
         */

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    // Checks if scene and the root node of the scene are not null
    // Check one of its flags to see if the returned data is incomplete
    {
        cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl; // Importer's GetErrorString
        return;
    }
    directory = path.substr(0, path.find_last_of('/')); // Gets directory path of the given file path
    cout << directory << endl;

    // Recursive function, processing the node in question, and then all the node's children.
    ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
    // Process all the meshes (if any) in a node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }
    // Repeat on its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    // Loops for as many vertices there are in the mesh (that's what mesh->mNumVertices does)
    {
        // process vertex positions, normals, and texture coordinates

        // Define a new Vertex struct & add it to the vertices array after each loop
        Vertex vertex;
        glm::vec3 vector; // Temporary vector for storing data from Assimp
        // Process Vertex data
        vector.x = mesh->mVertices[i].x; // mVertices = Assimp's name for vertex position array
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        // Process Normals data
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;

        // Process Texutre coordinates
            // Assimp allows a model to have up to 8 different texture coordinates PER vertex.
        if (mesh->mTextureCoords[0]) // Does the mesh have texture coordinates?
        {

            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
        {

            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }
    if (mesh->mMaterialIndex >= 0)
    {
        // Gett he aiMaterial object from the scene
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        // Load diffuse textures
        vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // Load specular textures
        vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // Load normal maps
        vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // Load height maps
        vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }
    return Mesh(vertices, indices, textures);
}

vector<Texture> Model::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) // Checks the number of textures in the material
    {
        aiString str;
        mat->GetTexture(type, i , &str); // gets the file locations
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            // Compare the texture path with all others in textures_loaded to see if the texture is already being used.
            if (std::strcmp(textures_loaded[j].path.data, str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        { // If the texture isn't already loaded, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str;
            textures_loaded.push_back(texture);
        }

    }
    return textures;
}

unsigned int Model::TextureFromFile(const char *path, const string &directory)
{
    string filename = string(path);
    filename = directory + '/' + filename;
    cout << filename.c_str() << endl;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
        {
            format = GL_RED;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

