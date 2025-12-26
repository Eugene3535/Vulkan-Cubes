#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <cglm/struct/vec3.h>
#include <cglm/struct/mat4.h>
#include <cglm/struct/cam.h>
#include <cglm/util.h>

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    enum Movement 
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    // camera Attributes
    vec3s Position;
    vec3s Front;
    vec3s Up;
    vec3s Right;
    vec3s WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(float yaw = YAW, float pitch = PITCH): 
        
        MovementSpeed(SPEED), 
        MouseSensitivity(SENSITIVITY), 
        Zoom(ZOOM)
    {
        Position = {0.f, 0.f, 3.f};
        Front = { 0.f, 0.f, -1.f }; 
        WorldUp = {0.f, 1.f, 0.f};;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch):
        MovementSpeed(SPEED), 
        MouseSensitivity(SENSITIVITY), 
        Zoom(ZOOM)
    {
        Position = { posX, posY, posZ };
        Front = { 0.0f, 0.0f, -1.0f }; 
        WorldUp = { upX, upY, upZ };
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    mat4s GetViewMatrix() noexcept
    {
        auto center = glms_vec3_add(Position, Front);

        return glms_lookat(Position, center, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;

        switch (direction)
        {
            case FORWARD:  glm_vec3_muladds(Front.raw, velocity, Position.raw); break;
            case BACKWARD: glm_vec3_mulsubs(Front.raw, velocity, Position.raw); break;
            case LEFT:     glm_vec3_mulsubs(Right.raw, velocity, Position.raw); break;
            case RIGHT:    glm_vec3_muladds(Right.raw, velocity, Position.raw); break;
            
            default:
                break;
        }
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        vec3s front;
        front.x = cos(glm_rad(Yaw)) * cos(glm_rad(Pitch));
        front.y = sin(glm_rad(Pitch));
        front.z = sin(glm_rad(Yaw)) * cos(glm_rad(Pitch));
        Front = glms_vec3_normalize(front);
        
        // also re-calculate the Right and Up vector
        Right = glms_vec3_normalize(glms_cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glms_cross(Right, Front);
    }
};

#endif // !CAMERA_HPP