#version 330 core

struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    float shininess;
};


in vec2 TexCoords;

out vec4 FragColor;

uniform Material material;


void main() {
    FragColor = texture(material.diffuse1, TexCoords) + texture(material.specular1, TexCoords);
}