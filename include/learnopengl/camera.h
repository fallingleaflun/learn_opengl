#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#include <glad/glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include <vector>
#include <iostream>

enum class Camera_Movement {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    FORWARD,
    BACKWARD
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 1.0f;
const float FOV = 45.0f;

class Camera{
    public:
        // camera Attributes
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        glm::quat FrontQuaternion;
        
        // euler Angles
        float Yaw;
        float Pitch;

        // camera options
        float MovementSpeed;
        float MouseSensitivity;
        float Fov;
        
        // constructor with vectors
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
        
        // constructor with scalar values
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
        
        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 GetViewMatrix();
        
        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void ProcessKeyboard(Camera_Movement direction, float deltaTime);
        
        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
        
        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(float yoffset);
        
        void updateCameraVectorsByQuat1(float xoffset, float yoffset);
        void updateCameraVectorsByQuat2(float xoffset, float yoffset);
        glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);
        void ResetYawAndPitch();
    private:
        // calculates the front vector from the Camera's (updated) Euler Angles
        void updateCameraVectors();
};

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(FOV)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(FOV)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}


void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == Camera_Movement::UP)
        Position += Up * velocity;

    if (direction == Camera_Movement::DOWN)
        Position -= Up * velocity;

    if (direction == Camera_Movement::LEFT)
        Position -= Right * velocity;

    if (direction == Camera_Movement::RIGHT)
        Position += Right * velocity;

    if (direction == Camera_Movement::FORWARD)
        Position += Front * velocity;
    
    if (direction == Camera_Movement::BACKWARD)
        Position -= Front * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;
    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch){
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }
    std::cout<<"pitch:"<< Pitch <<", yaw:"<< Yaw << std::endl;

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    Fov -= (float)yoffset;
    if (Fov < 1.0f)
        Fov = 1.0f;
    if (Fov > 45.0f)
        Fov = 45.0f;
}

/// upadate CameraVector by Euler
void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;

    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

    Front = glm::normalize(front);

    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
    FrontQuaternion = glm::quat(Front); // 转换为四元数
}

/// update CameraVector by Quat(Maybe have some question)有问题的版本，不要用这个接口
void Camera::updateCameraVectorsByQuat1(float xoffset, float yoffset)
{

    Yaw += 90;

    xoffset *= 0.008f * MouseSensitivity;

    yoffset *= 0.008f * MouseSensitivity;

    Yaw = xoffset;

    Pitch = yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped

    if (Pitch > 90.0f)

        Pitch -= 90.0f;

    if (Pitch < -90.0f)

        Pitch += 90.0f;

    if (Yaw > 90.0f)

        Yaw -= 90.0f;

    if (Yaw < -90.0f)

        Yaw += 90.0f;

    glm::vec3 axis = glm::cross(Front, Up);

    glm::quat pitchQuat = glm::angleAxis(Pitch, axis);

    // determine heading quaternion from the camera up vector and the heading angle

    axis = glm::cross(Front, axis);

    glm::quat yawQuat = glm::angleAxis(Yaw, Up);

    // add the two quaternions

    glm::quat combinedRotation = pitchQuat * yawQuat;

    Front = glm::rotate(combinedRotation, Front);

    Front = glm::normalize(Front);

    Right = glm::normalize(glm::cross(Front, WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.

    Up = glm::normalize(glm::cross(Right, Front));

    Yaw -= 90;
}

/// update CameraVector by Quat (right)
void Camera::updateCameraVectorsByQuat2(float xoffset, float yoffset)
{

    //这个还是有问题的，因为求出来的四元数旋转轴是在世界空间的，会导致旋转一定角度后上下旋转会反过来，并且在Front接近WorldUp的时候会有天旋地转的感觉,所以这里我用/**/注释掉，更换新的标蓝色的
    /*
    // Find the rotation between the front of the object (that we assume towards +Z,
    // but this depends on your model) and the desired direction
    glm::quat rot1 = RotationBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f),normalize(glm::vec3(0.008f * MouseSensitivity*xoffset, 0.008f * MouseSensitivity * yoffset, 1.0f)) );
    Front = glm::normalize(glm::rotate(rot1, Front));
    Right = glm::normalize(glm::cross(Front, WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
    */
    glm::vec3 ViewDest = Front + Right * xoffset * 0.008f * MouseSensitivity + Up * yoffset * 0.008f * MouseSensitivity;
    glm::quat rot1 = RotationBetweenVectors(Front, ViewDest);
    Front = glm::normalize(glm::rotate(rot1, Front));
    Right = glm::normalize(glm::cross(Front, Up));
    Up = glm::normalize(glm::cross(Right, Front));
}


/// get rotation between two vectors
glm::quat Camera::RotationBetweenVectors(glm::vec3 start, glm::vec3 dest)
{
    start = normalize(start);
    dest = normalize(dest);
    float cosTheta = dot(start, dest);
    glm::vec3 rotationAxis;
    if (cosTheta < -1 + 0.001f)
    {
        // special case when vectors in opposite directions:
        // there is no "ideal" rotation axis
        // So guess one; any will do as long as it's perpendicular to start
        rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
        if (glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
            rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);
        rotationAxis = normalize(rotationAxis);
        return glm::angleAxis(180.0f, rotationAxis);
    }
    rotationAxis = cross(start, dest);
    float s = sqrt((1 + cosTheta) * 2);
    float invs = 1 / s;
    return glm::quat(
        s * 0.5f,
        rotationAxis.x * invs,
        rotationAxis.y * invs,
        rotationAxis.z * invs
    );
}

void Camera::ResetYawAndPitch()
{
    Yaw = YAW;
    Pitch = PITCH;
}

#endif