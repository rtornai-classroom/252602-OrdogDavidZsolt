#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

void main() {
    FragPos = vec3(matModel * vec4(aPos, 1.0));
    
    Normal = normalize(transpose(inverse(mat3(matModel))) * aNormal);
    
    TexCoord = aTexCoord;
    
    gl_Position = matProjection * matView * vec4(FragPos, 1.0);
}
