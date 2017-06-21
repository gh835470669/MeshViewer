#version 330 core
layout (location = 0) in vec3 posAttr;
layout (location = 1) in vec3 norAttr;
layout (location = 2) in vec2 texCoords;

uniform bool flat_flag = true;
flat out vec3 flatColor; // Resulting color from lighting calculations
smooth out vec3 smoothColor; // Resulting color from lighting calculations

uniform bool texture_flag = false;

uniform mat4 model;
uniform mat4 view;
uniform mat4 view_inv;
uniform mat4 projection;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float shininess;

uniform vec3 objectColor;

void main()
{
    gl_Position = projection * view * model * vec4(posAttr, 1.0f);

    // Gouraud Shading
    // ------------------------
    vec3 Position = vec3(model * vec4(posAttr, 1.0f));
    vec3 Normal = mat3(transpose(inverse(model))) * norAttr;

    //ambient lighting
    float ambientStrength = 0.1f;
    vec3 ambient = light.ambient * (texture_flag ? texture(texture_diffuse1, texCoords).xzy : ambientStrength * objectColor);

    //diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - Position);
    float diff = max(dot(norm, lightDir), 0.0);
    float diffuseStrength = 1.0f;
    vec3 diffuse = light.diffuse * diff * (texture_flag ? texture(texture_diffuse1, texCoords).xzy : diffuseStrength * objectColor);


    //specular lighting
    vec3 viewDir = normalize(vec3(view_inv * vec4(0.0, 0.0, 0.0, 1.0)) - Position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    float specularStrength = 1.0f;
    vec3 specular = light.specular * spec *(texture_flag ? texture(texture_specular1, texCoords).xzy : specularStrength);

    flatColor = ambient + diffuse + specular;
    if (flat_flag == false) smoothColor = flatColor;
}
