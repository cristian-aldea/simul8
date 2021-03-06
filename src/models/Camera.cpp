#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "../common/utils.h"
#include "../common/globals.h"

#include <algorithm>
#include <cmath>

using std::sin;
using std::cos;

using glm::mat4;
using glm::perspective;
using glm::radians;

using s8::setUniform;

Camera::Camera(GLuint shader, vec3 position) :
        position{position},
        look{vec3(0, 0, 0)},
        up{vec3(0, 1, 0)},
        shader{shader},
        fov{75},
        horizontalAngle{270}, // 0 angle corresponds to +x
        verticalAngle{0},
        mouseX{0},
        mouseY{0} {
    updateUniforms();
}

void Camera::update(GLFWwindow *window, float dt) {
    double newMouseX, newMouseY;
    glfwGetCursorPos(window, &newMouseX, &newMouseY);

    double dx = newMouseX - mouseX;
    double dy = mouseY - newMouseY; // inverted since -dy is up

    mouseX = newMouseX;
    mouseY = newMouseY;

    horizontalAngle += dx * dt * cameraViewSpeed;
    verticalAngle += dy * dt * cameraViewSpeed;

    if (horizontalAngle > 360) {
        horizontalAngle -= 360;
    } else if (horizontalAngle < -360) {
        horizontalAngle += 360;
    }

    verticalAngle = std::max(-89.0, std::min(89.0, verticalAngle));

    double theta = radians(horizontalAngle);
    double phi = radians(verticalAngle);

    look = vec3(cos(theta) * cos(phi), sin(phi), sin(theta) * cos(phi));

    vec3 cameraSideVector = glm::normalize(glm::cross(look, up));

    vec3 lookNoY = look;
    lookNoY.y = 0;
    vec3 cameraForward = glm::normalize(lookNoY);

    // Control camera position with WASD
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= cameraSideVector * cameraSpeed * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += cameraSideVector * cameraSpeed * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= cameraForward * cameraSpeed * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += cameraForward * cameraSpeed * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        position.y += cameraSpeed * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        position.y -= cameraSpeed * dt;
    }

    updateUniforms();

}

void Camera::updateUniforms() const {
    mat4 projectionMatrix = glm::perspective(glm::radians(fov), s8::WINDOW_RATIO, 0.1f, 100.0f);
    mat4 viewMatrix = glm::lookAt(position, position + look, up);

    setUniform(shader, projectionMatrix, s8::UNIFORM_PROJECTION_MATRIX_NAME);
    setUniform(shader, viewMatrix, s8::UNIFORM_VIEW_MATRIX_NAME);
    setUniform(shader, position, s8::UNIFORM_CAMERA_POSITION_NAME);
}


