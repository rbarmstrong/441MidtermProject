//
// Created by rbarmstrong on 10/10/2021.
//

#include "ArcBall.hpp"

#include <cmath>

void ArcBall::recomputeOrientation() {
    GLfloat x = _radius * sinf(_theta) * sinf(_phi);
    GLfloat y = -1 * _radius * cosf(_phi);
    GLfloat z = -1 * _radius * cosf(_theta) * sinf(_phi);
    _position = glm::vec3(x + _lookAtPoint.x, y + _lookAtPoint.y, z + _lookAtPoint.z);

    computeViewMatrix();
}

void ArcBall::moveForward(GLfloat movementFactor) {
    if (_radius > 1) {
        _radius -= movementFactor;
    }
    recomputeOrientation();
}

void ArcBall::moveBackward(GLfloat movementFactor) {
    if (_radius < 30) {
        _radius += movementFactor;
    }
    recomputeOrientation();
}
