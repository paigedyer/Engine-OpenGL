#ifndef MODEL_H
#define MODEL_H

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
using namespace std;

class Model
{
public:
    Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
    {
        LoadModel(path);
    }
    void Draw(Shader &shader);

    bool gammaCorrection;
private:
    // model data
    vector<Mesh> meshes;
    vector<Texture> textures_loaded;
    string directory;

    void LoadModel(string path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
    vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
    unsigned int TextureFromFile(const char *path, const string &directory);
};


#endif