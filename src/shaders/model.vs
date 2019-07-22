#version 330 core

struct Material {
    sampler2D diffuseSampler;
    sampler2D specularSampler;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    int type; // 0 = ambient, float diffuse
              // 1 = Sampler Texture0(diffuse) Texture1(specular)
};

struct Light {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // https://learnopengl.com/Lighting/Light-casters
    float innerCutOff;
    float outerCutOff; // Angle
    // http://www.ogre3d.org/tikiwiki/tiki-index.php?page=-Point+Light+Attenuation
    float constant;
    float linear;
    float quadratic;
    int type; // 0 Directional (sun)
              // 1 Point (360 lamp)
              // 2 Spotlight
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPositon;
uniform int numLights;
#define MAX_LIGHTS 16
uniform Light lights[MAX_LIGHTS];
uniform Material material;

in vec3 facetVertex;
in vec3 facetNormal;
in vec2 facetUV;

//out vec3 inVertex;
out vec3 inFragPos;
out vec3 inNormal;
out vec2 inUV;

void main()
{
    inFragPos = vec3(model * vec4(facetVertex, 1.0));
    //inVertex = facetVertex;
    inNormal = normalize(mat3(transpose(inverse(model))) * facetNormal);
    inUV = vec2(facetUV.x, 1.0-facetUV.y); // y-coord flipped
    gl_Position = projection * view * model * vec4(facetVertex, 1.0f);
}
