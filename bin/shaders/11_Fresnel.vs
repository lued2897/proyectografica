#version 330 core


// Adaptation for OpenGL/GLSL by: PhD Sergio Teodoro-Vite
// Based on: The CG Tutorial, Nvidia developer zone
// Chapter 7. Environment Mapping Techniques
// Theory and Programming: https://developer.download.nvidia.com/CgTutorial/cg_tutorial_chapter07.html

layout (location = 0) in vec3  aPos;
layout (location = 1) in vec3  aNormal;
layout (location = 2) in vec2  aTexCoords;
layout (location = 3) in vec3  tangent;
layout (location = 4) in vec3  bitangent;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 cameraPosition;

// Fresnel parameters
// bias, scale and power are values exposed to allow control over the appearance of the Fresnel effect
uniform float mRefractionRatio;
uniform float _Bias;
uniform float _Scale;
uniform float _Power;
uniform float time;

// Outgoing Fresnel reflection and refraction parameters
out vec3  vReflect;
out vec3  vRefract[3];
out float reflectionCoefficient;


void main(void)
{
    vec4 in_Position = vec4(aPos, 1.0);

    // ------------------------------------------------------------
    // 1) APPLY RADIAL WAVE DISPLACEMENT
    // ------------------------------------------------------------
    float dist = length(in_Position.xy);

    // Waves collapsing into the center:
    in_Position.z += 0.05 * sin(dist * 0.5 + time * 0.03);

    // (Optional: waves expanding outward)
    // in_Position.z += waveHeight * sin(dist * waveRadius - time * 0.03);

    // ------------------------------------------------------------
    // 2) TRANSFORM AFTER DISPLACEMENT
    // ------------------------------------------------------------
    gl_Position = projection * view * model * in_Position;

    // Position in world space after waves
    vec3 posWorld = (model * in_Position).xyz;

    // ------------------------------------------------------------
    // 3) NORMAL RECOMPUTATION
    // ------------------------------------------------------------
    // Using the original normal – optional improvement: compute perturbed normal
    vec3 normWorld = normalize(mat3(model) * aNormal);

    // ------------------------------------------------------------
    // 4) FRESNEL MATH (same as before)
    // ------------------------------------------------------------
    vec3 I = normalize(posWorld - cameraPosition);

    vReflect = reflect(I, normWorld);

    vRefract[0] = refract(I, normWorld, mRefractionRatio * 1.00);
    vRefract[1] = refract(I, normWorld, mRefractionRatio * 0.99);
    vRefract[2] = refract(I, normWorld, mRefractionRatio * 0.98);

    reflectionCoefficient =
        clamp(_Bias + _Scale * pow(1.0 + dot(I, normWorld), _Power), 0.0, 1.0);

    TexCoords = aTexCoords;
}
