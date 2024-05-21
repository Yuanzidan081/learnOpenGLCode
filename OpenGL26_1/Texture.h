#pragma once
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <vector>

struct SingleTexture {
    unsigned int id;
    std::string type;
};

class Texture
{
public:
    Texture(const char* picPath, int glTextureID, bool fliped = true);
    Texture(const char* picPath, int glTextureID, bool fliped, bool gamma);
    void Activate()
    {
        glActiveTexture(33984 + GLTextureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
    unsigned int& GetTextureID() { return textureID; }
private:
    static int width, height, nrChannels;
    unsigned int textureID;
    static unsigned char* data;
    int GLTextureID;
};

unsigned int loadCubeMap(std::vector<std::string> faces, const int glTextureID, const std::string& prefixPath = "");