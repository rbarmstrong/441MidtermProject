#include "ArcCam.hpp"

#include <cmath>

void ArcCam::recomputeOrientation() {
    // TODO #2
    _direction = glm::vec3(_radius*sin(_theta)*sin(_phi), -_radius*cos(_phi),-_radius*cos(_theta)*sin(_phi));
    // TODO #3
    _position = _lookAtPoint+_direction;
    CSCI441::Camera::computeViewMatrix();
}

void ArcCam::moveForward(GLfloat movementFactor) {
    _position += _direction*movementFactor;
    recomputeOrientation();
}

void ArcCam::moveBackward(GLfloat movementFactor) {
    _position -= _direction*movementFactor;
    recomputeOrientation();
}