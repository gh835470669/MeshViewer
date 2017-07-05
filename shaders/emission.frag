//fragment shader for nomal tutorial
//calculate the result color with phong lighting model in world space
#version 330

uniform mat4 view_inv;

//flat or smooth shading
uniform bool flat_flag = true;
flat in vec3 FlatNormal;
flat in vec3 FlatFragPos;
smooth in vec3 SmoothNormal;
smooth in vec3 SmoothFragPos;

// light source
#define MAX_LIGHTS 10
uniform int numLights;
uniform struct Light {
   vec4 position;
   vec3 intensity;
   float attenuation;
   float ambientCoefficient;
   float coneAngle;
   vec3 coneDirection;
} allLights[MAX_LIGHTS];

uniform vec3 ambientLight;

//texture
uniform bool texture_flag = false;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_emission;
in vec2 fragTexCoord;

//material
uniform struct Material {
    vec3 specular;
    vec3 diffuse;
    vec3 emission;
    float shininess;
} material;

//time
uniform float time;

//output
out vec4 color;


//caculate multiple light source
vec3 ApplyLight(Light light, Material mat,  vec3 normal, vec3 surfacePos, vec3 surfaceToCamera) {
    vec3 surfaceToLight;
    float attenuation = 1.0;
    if(light.position.w == 0.0) {
        //directional light
        surfaceToLight = normalize(-light.position.xyz);
        attenuation = 1.0; //no attenuation for directional lights
    } else {
        //point light
        surfaceToLight = normalize(light.position.xyz - surfacePos);
        float distanceToLight = length(light.position.xyz - surfacePos);
        attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));

        //cone restrictions (affects attenuation)
        //float lightToSurfaceAngle = degrees(acos(dot(-surfaceToLight, normalize(light.coneDirection))));
        //if(lightToSurfaceAngle > light.coneAngle){
        //    attenuation = 0.0;
        //}
    }

    //diffuse
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
    vec3 diffuse = diffuseCoefficient * light.intensity * mat.diffuse;

    //specular
    float specularCoefficient = 0.0;
    if(diffuseCoefficient > 0.0) //back face need not to calculate
        specularCoefficient = pow(max(0.0, dot(normal, normalize(surfaceToLight + surfaceToCamera))), mat.shininess);
    vec3 specular = specularCoefficient * light.intensity * mat.specular;

    //linear color (color before gamma correction)
    return attenuation * (diffuse + specular);
}

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

    //material
    Material mat;
    mat.shininess = 32.0f;
    if(texture_flag)
    {
        mat.diffuse = texture(texture_diffuse, fragTexCoord).rgb;
        mat.diffuse = vec3(0.0f);
        mat.specular = texture(texture_specular, fragTexCoord).rgb;
        /*Emission */
        vec3 emission = vec3(0.0);
        if (mat.specular.r == 0.0)   /*rough check for blackbox inside spec texture */
        {
            /*apply emission texture */
            //emission = texture(texture_emission, fragTexCoord).rgb;

            /*some extra fun stuff with "time uniform" */
            emission = texture(texture_emission, fragTexCoord + vec2(0.0, time)).rgb;   /*moving */
            emission = emission * (sin(time) * 0.5 + 1.0) * 2.0;                     /*fading */
        }
        mat.emission = emission;
    } else {
        mat.diffuse = material.diffuse;
        mat.specular = material.specular;
        mat.emission = material.emission;
    }

    //diffuse and specular part
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(vec3(view_inv * vec4(0.0, 0.0, 0.0, 1.0)) - FragPos);
    vec3 lights = vec3(0);
    for (int i = 0; i < numLights; i++) {
        lights += ApplyLight(allLights[i], mat, norm, FragPos, viewDir);
    }

    //ambient part and emission part
    vec3 result = mat.emission + ambientLight * mat.diffuse + lights;

    //output
    color = vec4(result, 1.0f);
}