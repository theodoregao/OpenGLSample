#include "Camera.h"
#include <glm\gtx\transform.hpp>


Camera::Camera() : viewDirection(0.0f, 0.0f, -1.0f), UP(0.0f, 1.0f, 0.0f)
{
}

glm::mat4 Camera::getWorldToViewMatrix() const {
	return glm::lookAt(position, position + viewDirection, UP);
}

void Camera::mouseUpdate(const glm::vec2& newMousePosition) {
	glm::vec2 mouseDelta = newMousePosition - oldMousePosition;
	if (glm::length(mouseDelta) < 50.0f) {
		// horizontal rotate
		viewDirection = glm::mat3(glm::rotate(-mouseDelta.x * 0.01f, UP)) * viewDirection;
		// vertical rotate
		viewDirection = glm::mat3(glm::rotate(-mouseDelta.y * 0.01f, glm::cross(viewDirection, UP))) * viewDirection;
	}
	oldMousePosition = newMousePosition;
}

const float MOVEMENT_SPEED = 0.1f;

void Camera::moveForward() {
	position += MOVEMENT_SPEED * viewDirection;
}

void Camera::moveBackward() {
	position -= MOVEMENT_SPEED * viewDirection;
}

void Camera::strafeLeft() {
	position += MOVEMENT_SPEED * glm::cross(viewDirection, UP);
}

void Camera::strafeRight() {
	position -= MOVEMENT_SPEED * glm::cross(viewDirection, UP);
}

void Camera::moveUp() {
	position += MOVEMENT_SPEED * UP;
}

void Camera::moveDown() {
	position -= MOVEMENT_SPEED * UP;
}

