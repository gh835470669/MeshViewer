//fragment shader for nomal tutorial
//calculate the result color with phong lighting model in world space
#version 330

uniform vec3 objectColor;

uniform mat4 view_inv;

uniform bool flat_flag = true;
uniform bool texture_flag = false;

flat in vec3 FlatNormal;
flat in vec3 FlatFragPos;
smooth in vec3 SmoothNormal;
smooth in vec3 SmoothFragPos;

in vec2 TexCoords;

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

out vec4 color;

void main()
{
    vec3 Normal;
    vec3 FragPos;
    if (flat_flag)
    {
        Normal = FlatNormal;
        FragPos = FlatFragPos;
    }
    else
    {
        Normal = SmoothNormal;
        FragPos = SmoothFragPos;
    }

    //ambient lighting
    float ambientStrength = 0.1f;
    vec3 ambient = light.ambient * (texture_flag ? texture(texture_diffuse1, TexCoords).xzy : ambientStrength * objectColor);

    //diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    float diffuseStrength = 1.0f;
    vec3 diffuse = light.diffuse * diff * (texture_flag ? texture(texture_diffuse1, TexCoords).xzy : diffuseStrength * objectColor);


    //specular lighting
//    vec3 viewDir = normalize(vec3(view_inv * vec4(0.0, 0.0, 0.0, 1.0)) - FragPos);
//    vec3 reflectDir = reflect(-lightDir, norm); //negate the lightDir because the reflect function expects the first vector to point from the light source towards the fragment's position
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
//    float specularStrength = 1.0f;
//    vec3 specular = light.specular * spec *(texture_flag ? texture(texture_specular1, TexCoords).xzy : specularStrength);

    //specular lighting , blinn
    vec3 viewDir = normalize(vec3(view_inv * vec4(0.0, 0.0, 0.0, 1.0)) - FragPos);
    vec3 halfway = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfway, norm), 0.0), shininess);
    float specularStrength = 1.0f;
    vec3 specular = light.specular * spec *(texture_flag ? texture(texture_specular1, TexCoords).xzy : specularStrength);

    vec3 result = (ambient + diffuse + specular);
    color = vec4(result, 1.0f);
}


