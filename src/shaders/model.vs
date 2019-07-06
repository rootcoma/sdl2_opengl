#version 330

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec3 verts;
in vec3 normal;

out vec4 pos;
out vec3 aNorm;

void main()
{
    pos = vec4(verts, 1.0);
    aNorm = normal;
    gl_Position = projection * view * model * pos;
}
