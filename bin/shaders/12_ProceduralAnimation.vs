#version 330 core
layout (location = 0) in vec3  aPos;
layout (location = 1) in vec3  aNormal;
layout (location = 2) in vec2  aTexCoords;
layout (location = 3) in vec3  tangent;
layout (location = 4) in vec3  bitangent;

out vec2 TexCoords;
out vec3 ex_N;

// ---- Camera-space outputs (you still use them for lighting)
out vec3 vertexPosition_cameraspace;
out vec3 Normal_cameraspace;

// ---- NEW: world-space position output (for fog!!)
out vec3 WorldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time = 0.0;
uniform float radius = 0.0;
uniform float height = 0.0;

void main()
{
    float t = time;

    // Trefoil knot displacement
    float x = -1*(sin(t) + 2.0 * sin(2.0 * t));
    float y = -1*(cos(t) - 2.0 * cos(2.0 * t));
    
    vec4 PosL = vec4(aPos, 1.0);
    PosL.x += radius * x;
    PosL.y += radius * y;
    PosL.z += height;

    // ---- Compute world-space position
    vec4 posWorld = model * PosL;
    WorldPos = posWorld.xyz;

    // ---- Camera-space values used by your lighting
    vec4 posCam = view * posWorld;
    vertexPosition_cameraspace = posCam.xyz;
    Normal_cameraspace = (view * model * vec4(aNormal, 0.0)).xyz;

    ex_N = aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * posCam;
}
