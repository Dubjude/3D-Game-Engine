#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>

class VBO
{
public:
	GLuint ID;
	VBO(const void* vertices, GLsizeiptr size);

	void Bind();
	void Unbind();
	void Delete();
};

#endif 
