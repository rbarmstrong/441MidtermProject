#include "Car.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

#ifndef M_PI
#define M_PI 3.14159265
#endif

Car::Car(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {
    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    _rotate90deg = M_PI / 2.0f;

    _colorBody = glm::vec3( 0.3f, 0.3f, 0.5f );
    _scaleBody = glm::vec3( 2.0f, 0.6f, 1.0f );

    _colorWheel = glm::vec3( 0.0f, 1.0f, 0.0f );
    _colorNose = glm::vec3( 0.0f, 0.0f, 1.0f );

    _wheelTrans = glm::vec3( 0.0f, 0.0f, 0.035f );
    _wheelTrans2 = glm::vec3( 0.06f, 0.0f, 0.0f );
    _wheelTransY = glm::vec3( 0.0f, -0.04f, 0.0f );

    _idleMove = 0.0f;
    _idleIsUp = true;
    _moveSpeed = 1;
    _rotateCarAngle = 0;
    _carTrans = glm::vec3(31.0f, .15f, 31.0f);

}
void Car::flyForward() {
    _carTrans.x += _moveSpeed * sinf(_rotateCarAngle);
    _carTrans.z += _moveSpeed * cosf(_rotateCarAngle);
    if (_carTrans.x > 55) {
        _carTrans.x -= _moveSpeed;
    }
    if (_carTrans.x < -55) {
        _carTrans.x += _moveSpeed;
    }
    if (_carTrans.z > 55) {
        _carTrans.z -= _moveSpeed;
    }
    if (_carTrans.z < -55) {
        _carTrans.z += _moveSpeed;
    }
}

void Car::flyBackward() {
    _carTrans.x -= _moveSpeed * sinf(_rotateCarAngle);
    _carTrans.z -= _moveSpeed * cosf(_rotateCarAngle);
    if (_carTrans.x > 55) {
        _carTrans.x -= _moveSpeed;
    }
    if (_carTrans.x < -55) {
        _carTrans.x += _moveSpeed;
    }
    if (_carTrans.z > 55) {
        _carTrans.z -= _moveSpeed;
    }
    if (_carTrans.z < -55) {
        _carTrans.z += _moveSpeed;
    }
}

void Car::turnLeft() {
    _rotateCarAngle += 0.1;
}

void Car::turnRight() {
    _rotateCarAngle -= 0.1;
}
GLfloat Car::getAngle() {
    return _rotateCarAngle;
}

glm::vec3 Car::getPosition() {
    return _carTrans;
}
void Car::drawCar(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {
    glUseProgram( _shaderProgramHandle );
    modelMtx = glm::translate(modelMtx, _carTrans);
    modelMtx = glm::rotate(modelMtx, _rotateCarAngle, CSCI441::Y_AXIS );
    _drawCarBody(modelMtx, viewMtx, projMtx);        // the body of car
    _drawWheel(true, true, modelMtx, viewMtx, projMtx);  // the left front wheel
    _drawWheel(true, false, modelMtx, viewMtx, projMtx);  // the left back wheel
    _drawWheel(false, true, modelMtx, viewMtx, projMtx);  // the right front wheel
    _drawWheel(false, false, modelMtx, viewMtx, projMtx);  // the right back wheel
    _drawCarTopper(modelMtx, viewMtx, projMtx);        // the nose
    _drawCarNose(modelMtx, viewMtx, projMtx);
}

void Car::_drawCarBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale( modelMtx, _scaleBody );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorBody[0]);

    CSCI441::drawSolidCube( 0.1 );
}

void Car::_drawWheel(bool leftWheel, bool frontWheel, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    if( leftWheel )
        modelMtx = glm::translate( modelMtx, -_wheelTrans);
    else
        modelMtx = glm::translate( modelMtx, _wheelTrans);

    if( frontWheel )
        modelMtx = glm::translate( modelMtx, -_wheelTrans2);
    else
        modelMtx = glm::translate( modelMtx, _wheelTrans2);

    modelMtx = glm::translate( modelMtx, _wheelTransY);

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorWheel[0]);

    CSCI441::drawSolidTorus( 0.01, 0.02, 16, 16 );
}

void Car::_drawCarTopper(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {

    modelMtx = glm::rotate( modelMtx, _rotate90deg, CSCI441::Y_AXIS );

    if (_idleIsUp) {
        _idleMove -= 0.0005;
    } else {
        _idleMove -= -0.0005;
    }

    if (_idleMove > 0.01 || _idleMove < -0.01) {
        _idleIsUp = !_idleIsUp;
    }
    modelMtx = glm::translate( modelMtx, glm::vec3(0.0f, _idleMove, 0.0f));


    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorNose[0]);

    CSCI441::drawSolidCylinder( 0.03, 0.03, 0.05, 16, 16 );
}

void Car::_drawCarNose(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::rotate( modelMtx, _rotate90deg, CSCI441::Z_AXIS );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorNose[0]);

    CSCI441::drawSolidCone( 0.025, 0.18, 16, 16 );
}

void Car::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}
