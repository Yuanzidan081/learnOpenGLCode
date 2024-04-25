#pragma once
#include <glad/glad.h>

#include <iostream>
class Texture
{
public:
    Texture(const char* picPath, int glTextureID);
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