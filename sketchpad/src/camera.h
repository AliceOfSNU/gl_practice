#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>
// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         =  0.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f; //(FOV)


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        // set position
        Position = position;
        // set Euler Angles
        // yaw defaults to -z direction.
        Yaw = yaw;
        Pitch = pitch;

        WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

        updateCameraVectors();
    }

    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        // TODO : fill in
        WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

        updateCameraVectors();

    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        // TODO : fill in
        glm::mat4 view;
        view = glm::lookAt(Position, Position + Front, Up);
        return view;
    }


    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        // TODO : fill in
        switch (direction) {
        case FORWARD:
            Position += MovementSpeed * deltaTime * Front;
            break;
        case BACKWARD:
            Position -= MovementSpeed * deltaTime * Front;
            break;
        case LEFT:
            Position -= MovementSpeed * deltaTime * Right;
            break;
        case RIGHT:
            Position += MovementSpeed * deltaTime * Right;
            break;
        }


    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        // TODO : fill in
        // pitch between -89 and 89 degree
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch -= yoffset;


        // clipping 
        if (Pitch > 89.f) Pitch = 89.f;
        if (Pitch < -89.f) Pitch = -89.f;

        // calculate front and other vectors
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        // TODO : fill in
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        glm::vec3 dir = glm::vec3(0.f, 0.f, 0.f);
        dir.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        dir.y = -sin(glm::radians(Pitch));
        dir.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = -glm::normalize(dir);

        // also calculate other axes directions.
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::cross(Right, Front);
    }
};


#endif