#include <GL/glew.h>
#include <algorithm>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <sstream>
#include <stb/stb_image.h>
#include <fstream>
#include <string>
#include <vector>


class Mesh{
public:
    float anglex = 0;
    float angley = 0;
    float anglez = 0;
    float scaleX = 1.0;
    float scaleY = 1.0;
    float scaleZ = 1.0;

    float x = 0.0, y = 0.0, z = 0.0;
    GLuint textureID;
    std::string name = "object";

    Mesh(const std::vector<float>& verts, const std::vector<unsigned int>& inds, const std::string& texturepath = "")
        : vertices(verts), indices(inds){
        
        SetupMesh();
        if (!texturepath.empty()) LoadTexture(texturepath);
    } 

    void Render(GLuint shaderProgram, glm::mat4 view, glm::mat4 projection){


        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));

        model = glm::rotate(model, glm::radians(anglez), glm::vec3(0, 0, 1));
        model = glm::rotate(model, glm::radians(angley), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(anglex), glm::vec3(1, 0, 0));
    
        model = glm::scale(model, glm::vec3(scaleX, scaleY, scaleZ));
        
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
    
        if (textureID) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glUniform1i(glGetUniformLocation(shaderProgram, "diffuseTex"), 0);
        }
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void SetPosX(float nx) { x = nx; }
    void SetPosY(float ny) { y = ny; }
    void SetPosZ(float nz) { z = nz; }

    float GetPosX() const { return x; }
    float GetPosY() const { return y; }
    float GetPosZ() const { return z; }

    void SetAngleX(float nxa) { anglex = nxa; }
    void SetAngleY(float nya) { angley = nya; }
    void SetAngleZ(float nza) { anglez = nza; }

    float GetAngleX() const { return anglex; }
    float GetAngleY() const { return angley; }
    float GetAngleZ() const { return anglez; }

    void SetScaleX(float nsx) { scaleX = nsx; }
    void SetScaleY(float nsy) { scaleY = nsy; }
    void SetScaleZ(float nsz) { scaleZ = nsz; }

    float GetScaleX() const { return scaleX; }
    float GetScaleY() const { return scaleY; }
    float GetScaleZ() const { return scaleZ; }

void SetTexture(const std::string& path) {
    glDeleteTextures(1, &textureID); // delete previous texture

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (data) {
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
}


private:

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    GLuint VAO, VBO, EBO;
    glm::mat4 modelMatrix;

    void SetupMesh() 
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Assuming vertices: position (3) + UV (2)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
    void LoadTexture(const std::string& path){
        
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cerr << "Failed to load texture: " << path << std::endl;
        }
        stbi_image_free(data);
        }
};




GLuint CompileShader(const char* vertexPath, const char* fragmentPath) {
    std::ifstream vShaderFile(vertexPath);
    std::ifstream fShaderFile(fragmentPath);
    std::stringstream vShaderStream, fShaderStream;

    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    std::string vertexCode = vShaderStream.str();
    std::string fragmentCode = fShaderStream.str();

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "Vertex Shader compilation error:\n" << infoLog << std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "Fragment Shader compilation error:\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Shader Linking Error:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return shaderProgram;
}

Mesh loadOBJ(const std::string& path, const std::string& texturePath = "") {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open OBJ file: " + path);
    }

    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec2> temp_texCoords;
    std::vector<unsigned int> indices;
    std::vector<float> vertices;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") { // vertex position
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            temp_positions.push_back(pos);
        }
        else if (type == "vt") { // texture coord
            glm::vec2 uv;
            ss >> uv.x >> uv.y;
            temp_texCoords.push_back(uv);
        }
        else if (type == "f") { // face
            std::string vtx;
            for (int i = 0; i < 3; i++) { // assume triangles
                ss >> vtx;
                std::replace(vtx.begin(), vtx.end(), '/', ' ');
                std::stringstream vs(vtx);
                int v, t, n;
                vs >> v >> t >> n; // obj format: v/t/n
                glm::vec3 pos = temp_positions[v - 1];
                glm::vec2 uv = (t > 0 && t <= temp_texCoords.size()) ? temp_texCoords[t - 1] : glm::vec2(0.0f);

                // push into vertex buffer layout [x,y,z,u,v]
                vertices.push_back(pos.x);
                vertices.push_back(pos.y);
                vertices.push_back(pos.z);
                vertices.push_back(uv.x);
                vertices.push_back(uv.y);

                indices.push_back(indices.size());
            }
        }
    }

    return Mesh(vertices, indices, texturePath);
}

