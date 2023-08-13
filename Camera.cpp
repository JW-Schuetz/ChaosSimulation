
#include "Camera.h"


bool Camera::first;
double Camera::lastMouseX;
double Camera::lastMouseY;


Camera::Camera( glm::vec3 position, glm::vec3 up, double yaw, double pitch )
{
    Front            = glm::vec3( 0.0f, 0.0f, -1.0f );
    MovementSpeed    = SPEED;
    MouseSensitivity = SENSITIVITY;
    Position         = position;
    WorldUp          = up;
    Yaw              = yaw;
    Pitch            = pitch;
    first            = true;
    lastMouseX       = 0.0f;
    lastMouseY       = 0.0f;

    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt( Position, Position + Front, Up );
}

void Camera::ProcessMouseMovement( double mouseRawX, double mouseRawY, GLboolean constrainPitch )
{
    if( first )
    {
        lastMouseX = mouseRawX;
        lastMouseY = mouseRawY;
	    first      = false;
    }

    double xoffset = mouseRawX  - lastMouseX;
    double yoffset = lastMouseY - mouseRawY; // reversed since y-coordinates go from bottom to top

    lastMouseX = mouseRawX;
    lastMouseY = mouseRawY;

    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    if( constrainPitch )
    {
        if( Pitch >  89.9f ) Pitch =  89.9f;
        if( Pitch < -89.9f ) Pitch = -89.9f;
    }

    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;

    front.x = float( cos( glm::radians( Yaw ) ) * cos( glm::radians( Pitch ) ) );
    front.y = float( sin( glm::radians( Pitch ) ) );
    front.z = float( sin( glm::radians( Yaw ) ) * cos( glm::radians( Pitch ) ) );

    Front = glm::normalize( front );

    Right = glm::normalize( glm::cross( Front, WorldUp ) );
    Up    = glm::normalize( glm::cross( Right,   Front ) );
}