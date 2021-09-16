#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in float aTexID;

out vec3 color;
out vec2 texCoord;
out float texID;

uniform mat4 model;
uniform mat4 camMatrix;

void main()
{
	gl_Position = camMatrix * model * vec4(aPos, 1.0);
	
	color = aColor;
	texCoord = aTex;
	texID = aTexID;
}
