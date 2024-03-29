#version 150

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec3 verts;

out vec4 pos;

void main()
{
    pos = vec4(verts, 1.0);
    gl_Position = projection * view * model * pos;
}
