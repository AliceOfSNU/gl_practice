#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec3 offset;
uniform float rotateZ;

mat3  rotationMatrix3(float angle){
    float s = sin(angle);
    float c = cos(angle);
    return mat3(c, -s, 0.0f,
                s, c,  0.0f,
                0.0f, 0.0f, 1.0f);
}

void main()
{
    vec3 rotated = rotationMatrix3(rotateZ) * aPos;
    gl_Position = vec4(rotated + offset, 1.0);
}