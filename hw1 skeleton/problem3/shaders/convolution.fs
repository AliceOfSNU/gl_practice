#version 330 core
out vec4 FragColor;
in vec2 textureCoords;

uniform mat3 kernel;
uniform sampler2D uSampler;
uniform vec2 texSize;

varying vec2 vTexCoord;

void main()
{
    // fill in
    vec2 stepSize = 1.0 / vec2(float(texSize.x), float(texSize.y));

    vec4 sum = vec4(0.0);
    sum += texture(uSampler, vec2(vTexCoord.x - stepSize.x, vTexCoord.y - stepSize.y))
            * kernel[2][0];
    sum += texture(uSampler, vec2(vTexCoord.x, vTexCoord.y - stepSize.y))
            * kernel[2][1];
    sum += texture(uSampler, vec2(vTexCoord.x + stepSize.x, vTexCoord.y - stepSize.y))
            * kernel[2][2];
 
    sum += texture(uSampler, vec2(vTexCoord.x - stepSize.x, vTexCoord.y))
            * kernel[1][0];
    sum += texture(uSampler, vec2(vTexCoord.x, vTexCoord.y))
            * kernel[1][1];
    sum += texture(uSampler, vec2(vTexCoord.x + stepSize.x, vTexCoord.y))
            * kernel[1][2];
 
    sum += texture(uSampler, vec2(vTexCoord.x - stepSize.x, vTexCoord.y + stepSize.y))
            * kernel[0][0];
    sum += texture(uSampler, vec2(vTexCoord.x, vTexCoord.y + stepSize.y))
            * kernel[0][1];
    sum += texture(uSampler, vec2(vTexCoord.x + stepSize.x, vTexCoord.y + stepSize.y))
            * kernel[0][2];
 
    sum.a = 1.0;
    FragColor = sum;
}