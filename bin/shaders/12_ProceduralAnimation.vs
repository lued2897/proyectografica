#version 330 core
layout (location = 0) in vec3  aPos;
layout (location = 1) in vec3  aNormal;
layout (location = 2) in vec2  aTexCoords;
layout (location = 3) in vec3  tangent;
layout (location = 4) in vec3  bitangent;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time = 0.0;
uniform float radius = 0.0;
uniform float height = 0.0;

void main()
{
    float t = time;

    // Nudo de trébol
    float x = -1*(sin(t) + 2.0 * sin(2.0 * t));
    float y = -1*(cos(t) - 2.0 * cos(2.0 * t));
    //float z = -sin(3.0 * t);

    vec4 PosL = vec4(aPos, 1.0);
    PosL.x += radius * x;
    PosL.y += radius * y;
    //PosL.z += height * z;
    PosL.z += height;

    gl_Position = projection * view * model * PosL;
    TexCoords = aTexCoords;
}