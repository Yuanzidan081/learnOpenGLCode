#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
enum CameraDir
{
    Forward,
    Backward,
    Right,
    Left
};
const float Yaw = -90.0f;
const float Pitch = 0.0f;
const float Fov = 45.0f;
const float Sensitivity = 0.05f;
const float Speed = 2.5f;
class Camera
{
public:
    // 相机位置  世界坐标系向上
    Camera(glm::vec3 pos = glm::vec3(0.0, 0.0, 0.0), glm::vec3 up = glm::vec3(0.0, 1.0, 0.0),
        float yaw = Yaw, float pitch = Pitch) :
        m_Pos(pos), m_Front(glm::vec3(0.0, 0.0, -1.0)), m_WorldUp(up),
        m_Yaw(yaw), m_Pitch(pitch),
        m_Fov(Fov), m_Sensitivity(Sensitivity), m_Speed(Speed)
    {
        UdpateCameraFrame();
    }
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
        float yaw = Yaw, float pitch = Pitch) :
        m_Pos(glm::vec3(posX, posY, posZ)), m_Front(glm::vec3(0.0, 0.0, -1.0)), m_WorldUp(glm::vec3(upX, upY, upZ)),
        m_Yaw(yaw), m_Pitch(pitch),
        m_Fov(Fov), m_Sensitivity(Sensitivity), m_Speed(Speed)
    {
        UdpateCameraFrame();
    }
    glm::mat4 GetCameraView()
    {
        return glm::lookAt(m_Pos, m_Pos + m_Front, m_Up);
    }
    void KeyBoardPressEvent(CameraDir& dir, float deltaTime)
    {
        m_Speed = 2.5f * deltaTime;
        if (dir == CameraDir::Forward)
            m_Pos += m_Speed * m_Front;
        if (dir == CameraDir::Backward)
            m_Pos -= m_Speed * m_Front;
        if (dir == CameraDir::Left)
            m_Pos -= m_Speed * m_Right;
        if (dir == CameraDir::Right)
            m_Pos += m_Speed * m_Right;
    }
    void MouseScroolEvent(double yoffset)
    {
        m_Fov -= yoffset;
        if (m_Fov <= 1.0f)
            m_Fov = 1.0f;
        if (m_Fov >= 45.0f)
            m_Fov = 45.0f;
    }
    void MouseMovementEvent(double xoffset, double yoffset)
    {
        xoffset *= m_Sensitivity;
        yoffset *= m_Sensitivity;
        m_Yaw += xoffset;
        m_Pitch += yoffset;
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;
        UdpateCameraFrame();
    }
    // 相机属性
    glm::vec3 m_Pos;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;
    // 欧拉角
    float m_Yaw, m_Pitch;

    // 相机选项
    float m_Fov;
    float m_Sensitivity;
    float m_Speed;
private:
    void UdpateCameraFrame()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(m_Pitch)) * cos(glm::radians(m_Yaw));
        front.y = sin(glm::radians(m_Pitch));
        front.z = cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw));
        m_Front = glm::normalize(front);
        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }


};