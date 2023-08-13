#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};


// Default camera values
const double YAW         = -90.0f;
const double PITCH       =   0.0f;
const double SPEED       =   2.5f;
const double SENSITIVITY =   0.1f;


class Camera
{
    static bool first;
    static double lastMouseX;
    static double lastMouseY;

public:
    // camera Attributes
    glm::vec3 Up;
    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    glm::vec3 Position;

    // euler Angles
    double Yaw;
    double Pitch;

    // camera options
    double MovementSpeed;
    double MouseSensitivity;

    Camera( glm::vec3 = glm::vec3( 0.0f, 0.0f, 0.0f ), glm::vec3 = glm::vec3( 0.0f, 0.0f, 0.0f ),
        double = 0, double = 0 );

    void ProcessMouseMovement( double, double, GLboolean = true );
    glm::mat4 GetViewMatrix();

private:
    void updateCameraVectors();
};