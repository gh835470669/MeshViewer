#ifndef CAMERA_H
#define CAMERA_H

// Std. Includes
#include <vector>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// An abstract camera class that processes input and
// calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Defines several possible options for camera movement.
    // Used as abstraction to stay away from window-system specific input methods
    enum MoveDirection {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    GLfloat maxFOV = 45;

    // Camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 givenCamUp;
    glm::vec3 worldUp;
    // Eular Angles
    GLfloat yaw;
    GLfloat pitch;
    // Camera options
    GLfloat movementSpeed;
    GLfloat fiewOfView;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           GLfloat yaw = -90.0f,
           GLfloat pitch = 0.0f) :
        front(glm::vec3(0.0f, 0.0f, -1.0f)),
        movementSpeed(1.0f),
        fiewOfView(maxFOV),
        worldUp(glm::vec3(0.0f, 1.0f, 0.0f))
    {
        this->position = position;
        this->givenCamUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        this->updateCameraVectors();
    }

    // Constructor with scalar values
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ,
           GLfloat upX, GLfloat upY, GLfloat upZ,
           GLfloat yaw, GLfloat pitch) :
        front(glm::vec3(0.0f, 0.0f, -1.0f)),
        movementSpeed(1.0f),
        fiewOfView(maxFOV),
        worldUp(glm::vec3(0.0f, 1.0f, 0.0f))
    {
        this->position = glm::vec3(posX, posY, posZ);
        this->givenCamUp = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        this->updateCameraVectors();
    }

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(this->position, this->position + this->front, this->up);
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void move(MoveDirection direction, GLfloat deltaTime)
    {
        GLfloat velocity = this->movementSpeed * deltaTime;
        if (direction == FORWARD)
            this->position += this->front * velocity;
        if (direction == BACKWARD)
            this->position -= this->front * velocity;
        if (direction == LEFT)
            this->position -= this->right * velocity;
        if (direction == RIGHT)
            this->position += this->right * velocity;
        if (direction == UP)
            this->position += this->worldUp * velocity;
        if (direction == DOWN)
            this->position -= this->worldUp * velocity;
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void zoom(GLfloat delta)
    {
        if (this->fiewOfView >= 1.0f && this->fiewOfView <= maxFOV)
            this->fiewOfView -= delta;
        if (this->fiewOfView <= 1.0f)
            this->fiewOfView = 1.0f;
        if (this->fiewOfView >= maxFOV)
            this->fiewOfView = maxFOV;
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void rotate(const float pitch, const float yaw) {
        this->pitch += pitch;
        if (this->pitch > 89.0f)
            this->pitch = 89.0f;
        if (this->pitch < -89.0f)
            this->pitch = -89.0f;
        this->yaw += yaw;
        this->updateCameraVectors();
    }

private:

    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        front.y = sin(glm::radians(this->pitch));
        front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        this->front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        this->right = glm::normalize(glm::cross(this->front, this->givenCamUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        this->up    = glm::normalize(glm::cross(this->right, this->front));
    }
};

#endif // CAMERA_H
