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

uniform float time;
uniform float radius=1.0; // wave frequency
uniform float height = 0.01; // wave amplitude

void main()
{
    vec4 PosL = vec4(aPos, 1.0);

    float dist = length(PosL.xy);           // radial distance from center

    // Waves moving INTO the center
    PosL.z += 0.1275 * sin(dist * 1.0 + time * 0.03);
    PosL.z += 0.0725 * sin(dist * 2.0 + time * 0.06);

    gl_Position = projection * view * model * PosL;
    TexCoords = aTexCoords;
}
