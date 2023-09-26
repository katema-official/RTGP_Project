#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 color;
layout (location = 2) in mat4 model;	//aInstanceMatrix

out vec4 myColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	myColor = color;
}