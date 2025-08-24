#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in float BlockType;
in vec2 TexCoord;
in float Skylight;

uniform sampler2D textureAtlas;
//uniform sampler2D blockTextures[32];
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;

void main()
{
    int type = int(BlockType);
    vec4 texColor = texture(textureAtlas, TexCoord);
    
    vec3 objectColor = texColor.rgb;
    
    // SISTEMA DE ILUMINAÇÃO CORRIGIDO
    
    // 1. Luz ambiente base (sempre presente)
    float ambientStrength = 0.15; // Luz ambiente mínima
    vec3 ambient = ambientStrength * objectColor;
    
    // 2. Skylight (luz do céu) - usa o valor calculado no CPU
    vec3 skylight = Skylight * objectColor;
    
    // 3. Luz direcional do sol (apenas para blocos expostos ao céu)
    vec3 sunlight = vec3(0.0);
    if (Skylight > 0.21) { // Apenas blocos com boa exposição ao céu recebem luz solar direta
        vec3 sunDir = normalize(vec3(-0.3, -1.0, 0.2)); // Direção do sol
        float sunDiff = max(dot(Normal, -sunDir), 0.0);
        
        // Intensidade do sol baseada na exposição
        float sunIntensity = (Skylight - 0.2) * 2.0; // Normaliza a intensidade
        sunIntensity = clamp(sunIntensity, 0.0, 1.0);
        
        sunlight = sunDiff * lightColor * sunIntensity * 0.6;
    }
    
    // 4. Combinação final das luzes
    // O skylight substitui parcialmente a luz ambiente para blocos expostos
    vec3 finalAmbient = mix(ambient, skylight, min(Skylight * 2.0, 1.0));
    vec3 finalColor = finalAmbient + sunlight;
    
    // 5. Fog (neblina)
    float distanceCamera = length(cameraPos - FragPos);
    float fogStart = 30.0;
    float fogEnd = 80.0;
    float fogFactor = clamp((distanceCamera - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
    vec3 fogColor = vec3(0.53, 0.81, 0.92);
    finalColor = mix(finalColor, fogColor, fogFactor);
    
    // 6. Transparência para água
    if (type == 5) {
        FragColor = vec4(finalColor, 0.8); // Água mais opaca para melhor visibilidade
    } else {
        FragColor = vec4(finalColor, 1.0);
    }
}