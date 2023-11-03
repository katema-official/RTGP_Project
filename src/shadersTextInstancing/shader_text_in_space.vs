#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec2 offsetTex;
layout (location = 3) in mat4 model;

out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(vertex, 1.0);
    TexCoords = tex + offsetTex;
}