#version 150

in vec3 verts;

out vec4 pos;

void main()
{
    pos = vec4(verts, 1.0);
    gl_Position = pos;
}
