#version 330 core

uniform bool flat_flag = true;
flat in vec3 flatColor; // Resulting color from lighting calculations
smooth in vec3 smoothColor; // Resulting color from lighting calculations

out vec4 color;

void main()
{
   vec3 tColor;

   if (flat_flag)
        tColor = flatColor;
   else
        tColor = smoothColor;

   color = vec4(tColor, 1.0f);
}
