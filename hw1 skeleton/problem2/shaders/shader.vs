#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;

out vec3 ourColor;

// declare uniform vairable
uniform vec3 offset;
uniform vec3 size;
uniform float rotateZ;

mat4  rotationMatrix4(float angle){
    float s = sin(angle);
    float c = cos(angle);
    return mat4(c, -s, 0.0f, 0.0f,
                s, c,  0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
}

mat3  rotationMatrix3(float angle){
    float s = sin(angle);
    float c = cos(angle);
    return mat3(c, -s, 0.0f,
                s, c,  0.0f,
                0.0f, 0.0f, 1.0f);
}



void main()
{    
    // fill in
    vec3 rotated = rotationMatrix3(rotateZ) * pos;
    gl_Position = vec4(size * rotated + offset, 1.0);
    ourColor = col;
}

