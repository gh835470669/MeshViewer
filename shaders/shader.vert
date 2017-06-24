//vertex shader for nomal tutorial
//transform position and normal to world space and send to fragment shader
#version 330
layout (location = 0) in vec3 posAttr;
layout (location = 1) in vec3 norAttr;
layout (location = 2) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool flat_flag = true;

flat out vec3 FlatNormal;
flat out vec3 FlatFragPos;
smooth out vec3 SmoothNormal;
smooth out vec3 SmoothFragPos;

out vec2 TexCoords;

void main()
{  
    gl_Position = projection * view * model * vec4(posAttr, 1.0f);

    TexCoords = texCoords;

    /*nomal matrix*/
    /*should compute in cpu, however it is easy to understand by coding here*/
    FlatNormal = mat3(transpose(inverse(model))) * norAttr;
    FlatFragPos = vec3(model * vec4(posAttr, 1.0f));

    if (flat_flag == false)
    {
        SmoothNormal = FlatNormal;
        SmoothFragPos = FlatFragPos;
    }

}
