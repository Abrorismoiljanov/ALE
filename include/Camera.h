#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <SDL2/SDL.h>

class Camera {
public:
    // Camera attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    // Angles for rotation
    float yaw;
    float pitch;

    // Speed and sensitivity
    float speed;
    float sensitivity;

    // Mouse position tracking
    float lastX, lastY;
    bool firstMouse;

    Camera(glm::vec3 startPos = glm::vec3(0.0f, 0.0f, 5.0f), float startYaw = -90.0f, float startPitch = 0.0f, 
           float moveSpeed = 2.5f, float mouseSensitivity = 0.1f)
        : position(startPos), yaw(startYaw), pitch(startPitch), speed(moveSpeed), sensitivity(mouseSensitivity),
          firstMouse(true) {
        front = glm::vec3(0.0f, 0.0f, -1.0f);  // Looking straight down the negative Z axis
        up = glm::vec3(0.0f, 1.0f, 0.0f);     // Default up is Y axis
        right = glm::normalize(glm::cross(front, up)); // Right vector
        lastX = 1280.0f / 2.0f;
        lastY = 720.0f / 2.0f;
    }

    // Process keyboard input for movement
    void ProcessKeyboard(const Uint8* keystate, float deltaTime, bool relativemouse) {
        float velocity = speed * deltaTime;
        if (relativemouse) {
                if (keystate[SDL_SCANCODE_W]) position += front * velocity;
                if (keystate[SDL_SCANCODE_S]) position -= front * velocity;
                if (keystate[SDL_SCANCODE_A]) position -= right * velocity;
                if (keystate[SDL_SCANCODE_D]) position += right * velocity;
        }
    }

    // Process mouse movement for camera rotation
    void ProcessMouseMovement(int dx, int dy) {
        float xoffset = dx * sensitivity;
        float yoffset = -dy * sensitivity; // Reversed y-axis
        yaw += xoffset;
        pitch += yoffset;


        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        updateCameraVectors();
    }

    // Generate the view matrix
    glm::mat4 GetViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }

    // Update camera direction vectors based on yaw and pitch
    void updateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);

        right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        up = glm::normalize(glm::cross(right, front));
    }

    // Generate the projection matrix
    glm::mat4 GetProjectionMatrix(float width, float height) {
        return glm::perspective(glm::radians(45.0f), width / height, 0.1f, 100.0f);
    }
};
