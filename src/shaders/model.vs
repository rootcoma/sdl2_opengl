#version 150

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec3 verts;
varying vec3 normal;

out vec4 pos;

void main()
{
    pos = projection * view * model * vec4(verts, 1.0);
    gl_Position = pos;
}
