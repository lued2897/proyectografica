#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec2 TexCoords;
out vec3 ex_N;
out vec3 EyeDirection_cameraspace;
out vec3 vertexPosition_cameraspace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

void main()
{
    // Animación tipo ola en espacio local
    vec4 PosL = vec4(aPos, 1.0);
    PosL.y += 0.5 * sin(PosL.z + time) * PosL.z;

    // Posición en mundo y en cámara
    vec4 worldPos = model * PosL;
    vertexPosition_cameraspace = (view * worldPos).xyz;
    EyeDirection_cameraspace   = vec3(0.0, 0.0, 0.0) - vertexPosition_cameraspace;

    // La normal se deja en espacio de modelo,
    // igual que en 10_vertex_skinning-IT.vs
    ex_N = aNormal;

    gl_Position = projection * view * worldPos;
    TexCoords   = aTexCoords;
}

