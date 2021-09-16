#version 330 core

layout (location = 0) out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in float texID;

uniform sampler2D tex_array[2];

void main()
{
	int index = int(texID);
	FragColor = texture(tex_array[index], texCoord) * vec4(color, 1.0); 
}
