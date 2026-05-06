#version 330 core

out vec4 FragColor;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform bool lightingEnabled;
uniform bool isSun;
uniform sampler2D sunTexture;

void main() {
    if (isSun) {
        FragColor = texture(sunTexture, TexCoord) * vec4(lightColor, 1.0);
    } else {
        vec3 objectColor = vec3(1.0, 1.0, 1.0);
        
        if (!lightingEnabled) {
            FragColor = vec4(objectColor, 1.0);
        } else {
            float ambientStrength = 0.15; 
            vec3 ambient = ambientStrength * lightColor;
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;         
            float specularStrength = 0.5;
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
            vec3 specular = specularStrength * spec * lightColor;       
            float distance = length(lightPos - FragPos);
            float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
            vec3 result = (ambient + (diffuse + specular) * attenuation) * objectColor;         
            FragColor = vec4(result, 1.0);
        }
    }
}
