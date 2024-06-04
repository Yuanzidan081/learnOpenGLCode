#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
Texture::Texture(const char* picPath, int glTextureID, bool fliped): GLTextureID(glTextureID)
{

        data = stbi_load(picPath, &width, &height, &nrChannels, 0);

        glGenTextures(1, &textureID);//参数1：生成纹理的数量，参数2：储存:的unsigned int数组（这个例子中只是单独的一个unsigned int）
        glActiveTexture(33984 + glTextureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        stbi_set_flip_vertically_on_load(fliped);
        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        //透明使用
        /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        if (data)
        {
            std::cout << "Success" << std::endl;
            if (nrChannels == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            }
            else if (nrChannels == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            }
            else {
                std::cout << "Unsupported image format" << std::endl;
            }
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);//释放图片内存

}

Texture::Texture(const char* picPath, int glTextureID, bool fliped /*= true*/, bool gamma) : GLTextureID(glTextureID)
{
    data = stbi_load(picPath, &width, &height, &nrChannels, 0);

    glGenTextures(1, &textureID);//参数1：生成纹理的数量，参数2：储存:的unsigned int数组（这个例子中只是单独的一个unsigned int）
    


    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;
        std::cout << "Success" << std::endl;
        if (nrChannels == 4) {
            internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA; 
            dataFormat = GL_RGBA;
        }
        else if (nrChannels == 3) {
            internalFormat = gamma ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else {
            std::cout << "Unsupported image format" << std::endl;
        }
        glActiveTexture(33984 + glTextureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        stbi_set_flip_vertically_on_load(fliped);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        //透明使用
        /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);//释放图片内存

}

int Texture::width = 0;
int Texture::height = 0;
int Texture::nrChannels = 0;
unsigned char* Texture::data = nullptr;

unsigned int loadCubeMap(std::vector<std::string> faces, const int glTextureID, const std::string& prefixPath)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(33984 + glTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    
    int width, height, channels;
    for (size_t i = 0; i < faces.size(); ++i)
    {
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load((prefixPath + faces[i]).c_str(), &width, &height, &channels, 0);
        if (data)
        {
            std::cout << "Success" << std::endl;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at: " << faces[i] << std::endl;
        }
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;

}
