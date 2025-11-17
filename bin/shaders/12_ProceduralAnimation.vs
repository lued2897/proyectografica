#version 330 core
layout (location = 0) in vec3  aPos;
layout (location = 1) in vec3  aNormal;
layout (location = 2) in vec2  aTexCoords;
layout (location = 3) in vec3  tangent;
layout (location = 4) in vec3  bitangent;

out vec2 TexCoords;
out vec3 ex_N;

// ---- Camera-space outputs (para iluminación)
out vec3 vertexPosition_cameraspace;
out vec3 Normal_cameraspace;

// ---- World-space position (para fog / water, etc.)
out vec3 WorldPos;

// ===== UNIFORMS =====
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Animación tipo trébol EN EL VÉRTEX SHADER (para basura, etc.)
uniform float time;
uniform float radius;
uniform float height;
uniform int   useTrefoil;   // 0 = no usar, 1 = sí usar

void main()
{
    // Posición en espacio de modelo
    vec4 PosL = vec4(aPos, 1.0);

    // ----- SOLO si useTrefoil == 1 se aplica la animación en el vértice
    if (useTrefoil == 1) {
        float t = time;

        float x = -(sin(t) + 2.0 * sin(2.0 * t));
        float y = -(cos(t) - 2.0 * cos(2.0 * t));

        PosL.x += radius * x;
        PosL.y += radius * y;
        PosL.z += height;
    }

    // ---- Posición en mundo
    vec4 posWorld = model * PosL;
    WorldPos = posWorld.xyz;

    // ---- Camera-space
    vec4 posCam = view * posWorld;
    vertexPosition_cameraspace = posCam.xyz;
    Normal_cameraspace = (view * model * vec4(aNormal, 0.0)).xyz;

    ex_N = aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * posCam;
}
