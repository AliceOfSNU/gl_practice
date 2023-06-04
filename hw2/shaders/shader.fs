#version 330 core
// TODO: define in/out and uniform variables.
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D textureSamp;

void main()
{
    // fill in
    // Hint) you can ignore transparent texture pixel by 
    // if(color.a < 0.5){discard;}
    FragColor = texture(textureSamp, TexCoord);
}