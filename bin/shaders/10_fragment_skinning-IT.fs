#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 ex_N; 
in vec3 EyeDirection_cameraspace;
in vec3 vertexPosition_cameraspace;

uniform vec4 MaterialAmbientColor;
uniform vec4 MaterialDiffuseColor;
uniform vec4 MaterialSpecularColor;

#define MAX_LIGHTS 10
uniform int numLights;

uniform sampler2D texture_diffuse1;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


// ----- Underwater uniforms -----
uniform vec3 cameraPos;          // camera world-space position
uniform vec3 fogColor = vec3(0.0, 0.25, 0.45);  // bluish tone
uniform float fogDensity = 0.04;                // how fast it fades
uniform float waterLevel = 0.0;                 // y = surface level
uniform float depthAttenuation = 0.06;          // how dark it gets with depth
uniform float time;
uniform float transparency = 1.0;
uniform float caustic_intensity = 1.0;


float causticPattern(vec3 pos, float t)
    {
        float wave1 = sin(pos.x * 4.0 + t * 2.0) * cos(pos.z * 4.0 + t * 2.5);
        float wave2 = sin(pos.x * 3.3 - t * 1.7) * cos(pos.z * 5.1 - t * 2.3);
        float wave3 = sin(pos.x * 6.7 + t * 1.9) * cos(pos.z * 3.5 + t * 2.8);
    
        float pattern = (wave1 + wave2 + wave3) / 3.0;
        pattern = abs(pattern);
        pattern = pow(pattern, 3.0);
        return pattern;
    }

uniform struct Light {
   vec3  Position;
   vec3  Direction;
   vec4  Color;
   vec4  Power;
   int   alphaIndex;
   float distance;
} allLights[MAX_LIGHTS];

vec4 ApplyLight(Light light, vec3 N, vec3 L, vec3 E) {
    vec4 K_a = MaterialAmbientColor * light.Color;

    float cosTheta = clamp(dot(N, L), 0.0, 1.0);
    vec4 K_d = MaterialDiffuseColor * light.Color * cosTheta;

    vec3 R = reflect(-L, N);
    float cosAlpha = clamp(dot(E, R), 0.0, 1.0);
    vec4 K_s = MaterialSpecularColor * light.Color * pow(cosAlpha, light.alphaIndex);

    vec4 l_contribution =
        (K_a + K_d + K_s) * light.Power / (light.distance * light.distance);

    return l_contribution;
}

void main()
{    
    vec3 Normal_cameraspace = ( view * model * vec4(ex_N,0)).xyz;
    vec3 n = normalize(Normal_cameraspace);
    vec4 ex_color = vec4(0.0);

    for (int i = 0; i < numLights; ++i) {
        vec3 EyeDirection_cameraspace = vec3(0.0) - vertexPosition_cameraspace;
        vec3 LightPosition_cameraspace = (view * vec4(allLights[i].Position, 1)).xyz;
        vec3 LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

        vec3 e = normalize(EyeDirection_cameraspace);
        vec3 l = normalize(LightDirection_cameraspace);
        ex_color += ApplyLight(allLights[i], n, l, e);
    }
    ex_color.a = transparency; //TODO add transparency
    vec4 texel = texture(texture_diffuse1, TexCoords);
    vec4 baseColor = texel * ex_color;
    // // FragColor = texture(texture_diffuse1, TexCoords);
    
    //vec4 MaterialAmbientColor = vec4(0.5, 0.5, 0.5, 1.0);
    //vec3 lightDirection = vec3(0.0, -1.0, 0.0);

    //vec3 LightPosition_cameraspace = ( view * vec4(lightDirection,1.0f)).xyz;

    //vec3 LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
    //vec3 l = normalize( LightDirection_cameraspace );

    //float intensity = clamp(dot(ex_N, l),0, 1);
    //vec4 MaterialDiffuseColor = intensity * vec4(1.0, 1.0, 1.0, 1.0);

    // Intensidad del punto brilloso especular
    //vec4 LightPower = vec4(1.0f,1.0f,1.0f,1.0f);
    // Normal de la superficie en cooridenadas de cámara
    //vec3 Normal_cameraspace = ( view * model * vec4(ex_N,0)).xyz; 
    // Normal a la superficie normalizada
    //vec3 n = normalize( Normal_cameraspace );
    // Rayo reflejado en la superficie
    //vec3 R = reflect(-l,n);
    // Normalizamos el vector de posición del usuario
    // Eye
    //vec3 E = normalize(EyeDirection_cameraspace);
    // Transición del lóbulo de la componente especular
    //float cosAlpha = clamp( dot( E,R ), 0, 1 );

    // Cálculo de la componente especular
    //vec4 MaterialSpecularColor =  vec4(1.0, 1.0, 1.0, 1.0) * LightPower * pow(cosAlpha,5);
    

    //ex_color.a = transparency;

    //TODO check this
    //vec4 texel = texture(texture_diffuse1, TexCoords);
    //vec4 baseColor =  texel * (MaterialAmbientColor + MaterialDiffuseColor + MaterialSpecularColor);


     //Water effects
        // --- World-space reconstruction (so caustics don't follow camera) ---
        vec3 worldPos = (inverse(view) * vec4(vertexPosition_cameraspace, 1.0)).xyz;

        // --- Apply caustics pattern in world space ---
        //float caustics = causticPattern(vec3(worldPos.xz, 0.0) * 0.15, time);
        float caustics = causticPattern(worldPos * 0.15, time)*caustic_intensity;
        baseColor.rgb += vec3(caustics) * 0.05;

        // --- Optional: make caustics fade with depth ---
        float depth = clamp((waterLevel - worldPos.y) * depthAttenuation, 0.0, 1.0);
        baseColor.rgb *= mix(1.0, 0.5, depth);
        baseColor.rgb += vec3(caustics) * 0.25 * (1.0 - depth);

        // --- Fog (same as before) ---
        float dist = length(cameraPos - worldPos);
        float fogFactor = exp(-fogDensity * dist);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        vec3 finalColor = mix(fogColor, baseColor.rgb, fogFactor);
    

    // FragColor = texel * (MaterialAmbientColor + MaterialDiffuseColor + MaterialSpecularColor);
    //FragColor = finalColor*( MaterialAmbientColor + MaterialDiffuseColor + MaterialSpecularColor);
    FragColor = vec4(finalColor, baseColor.a);
}