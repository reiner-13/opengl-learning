#version 330 core
in vec2 TexCoord;

uniform sampler2D noiseTexture;
uniform vec2 scrollOffset;

out vec4 FragColor;

void main()
{
    // Scroll the noise texture
    vec2 scrolledTexCoord = TexCoord + scrollOffset;
    // Sample noise
    float noiseValue = texture(noiseTexture, scrolledTexCoord).r;

    FragColor = vec4(vec3(noiseValue), 1.0);
    //FragColor = texture(noiseTexture, TexCoord);
}