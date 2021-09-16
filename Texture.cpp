#include"Texture.h"

Texture::Texture(const char* image, GLenum texType, GLenum texSlot, GLenum format, GLenum pixelType)
{
	type = texType;
	slot = texSlot;

	glGenTextures(1, &ID);
	glActiveTexture(texSlot);
	glBindTexture(texType, ID);

	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(texType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	int widthImg, heightImg, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(image, &widthImg, &heightImg, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(texType, 0, GL_RGB, widthImg, heightImg, 0, format, pixelType, data);
		glGenerateMipmap(texType);
	} 
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(texType, 0);
}

void Texture::Bind()
{
	glActiveTexture(slot);
	glBindTexture(type, ID);
}

void Texture::Unbind()
{
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}