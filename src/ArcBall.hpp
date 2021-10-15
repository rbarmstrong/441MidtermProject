//
// Created by rbarmstrong on 10/10/2021.
//
#include <CSCI441/Camera.hpp>

#ifndef A3_ARCBALL_H
#define A3_ARCBALL_H


class ArcBall : public CSCI441::Camera {
public:
    void recomputeOrientation() final;

    void moveForward(GLfloat movementFactor) final;
    void moveBackward(GLfloat movementFactor) final;
};


#endif //A3_ARCBALL_H
