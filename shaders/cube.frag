#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in float BlockType;
in vec2 TexCoord;

uniform sampler2D blockTextures[32];

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;

void main()
{

    int type = int(BlockType);
    vec4 texColor;

    if (type == 1) {
        texColor = texture(blockTextures[0], TexCoord); // Textura de Grama
    } 
    else if (type == 2) {
        texColor = texture(blockTextures[1], TexCoord); // Textura de Terra
    } 
    else if (type == 3) {
        texColor = texture(blockTextures[2], TexCoord); // Textura de Pedra
    } 
    else if (type == 4) {
        texColor = texture(blockTextures[3], TexCoord); // Textura de Pedra (y = 0)
    }
    else if (type == 5) {
        texColor = texture(blockTextures[4], TexCoord);
    } else {
        // Cor padrão para debug, caso o tipo seja desconhecido
        texColor = vec4(1.0, 0.0, 1.0, 1.0); // Rosa
    }

    vec3 objectColor = texColor.rgb;

    float ambient = 0.5;
    // if (lightPos.y < 64) {
    //     ambient = 0.01;
    // }

    // intensidade da luz -> quanto menor, maior a intensidade
    float linear = 0.05;

    // suavidade em distância maiores -> quanto menor, mais discrepante será a iluminação a grande distâncias
    float square = 0.01;

    // cálculo da atenuação
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + linear * distance + square * distance * distance); 
    
    // cálculo da difusão da luz
    vec3 lightDir = normalize(lightPos - FragPos); // calcula a direção da luz
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // cálculo da cor final da face
    //vec3 finalColor = objectColor * (ambient + diffuse);
    vec3 finalColor = objectColor * (ambient + (diffuse * attenuation));

    float distanceCamera = length(cameraPos - FragPos);
    float fogStart = 0.0;
    float fogEnd = 80.0;
    float fogFactor = clamp((distanceCamera - fogStart)/(distanceCamera - fogEnd), 0.0, 1.0);
    vec3 fogColor = vec3(0.53, 0.81, 0.92); 
    finalColor = mix(finalColor, fogColor, fogFactor);
    if (type == 5) {
        FragColor = vec4(finalColor, 0.1);
    } else {
        FragColor = vec4(finalColor, 1.0);
    }
}