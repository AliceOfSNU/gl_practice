#version 330 core
// TODO: define in/out and uniform variables.
out vec4 FragColor;
in vec3 TexCoords;

uniform float dayfactor;

uniform samplerCube skyboxDay;
uniform samplerCube skyboxNight;

void main()
{   
    // mix two texture
    FragColor = mix(texture(skyboxDay, TexCoords),
        texture(skyboxNight, TexCoords), 1.0-dayfactor);

}
