#pragma once
#include <glad/glad.h>
#include <string>
#include <iostream>

struct SingleTexture {
    unsigned int id;
    std::string type;
};

class Texture
{
public:
    Texture(const char* picPath, int glTextureID, bool fliped = true);
    void Activate(int glTextureID)
    {
        glActiveTexture(33984 + glTextureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
    unsigned int& GetTextureID() { return textureID; }
private:
    static int width, height, nrChannels;
    unsigned int textureID;
    static unsigned char* data;
};