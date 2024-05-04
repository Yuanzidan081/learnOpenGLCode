#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
Texture::Texture(const char* picPath, int glTextureID, bool fliped)
{
    {
        stbi_set_flip_vertically_on_load(fliped);
        data = stbi_load(picPath, &width, &height, &nrChannels, 0);

        glGenTextures(1, &textureID);//����1���������������������2������:��unsigned int���飨���������ֻ�ǵ�����һ��unsigned int��
        glActiveTexture(33984 + glTextureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        // Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        //͸��ʹ��
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
        stbi_image_free(data);//�ͷ�ͼƬ�ڴ�
    }
}
int Texture::width = 0;
int Texture::height = 0;
int Texture::nrChannels = 0;
unsigned char* Texture::data = nullptr;