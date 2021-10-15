#include "Ship.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

#ifndef M_PI
#define M_PI 3.14159265
#endif

Ship::Ship( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {
    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    _rotateShipAngle = 0.0f;
    _shipTrans = glm::vec3(30.0f, 5.0f, 30.0f);
    _turnRate = 0.05f;
    _moveSpeed = 0.05f;
    _bobRate = 0.005;

    _colorBody = glm::vec3( 0.8f, 0.64f, 0.24f );

    _colorWing = _colorBody;
    _rotateWingAngle = 3 * M_PI / 4.0f;

    _colorNose = glm::vec3( 0.2f, 0.2f, 0.2f );
    _rotateNoseAngle = 2 * M_PI / 3.0f;
}

void Ship::drawShip(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {
    glUseProgram( _shaderProgramHandle );

    _shipTrans.y += _bobRate;
    if (_shipTrans.y > 5.5 || _shipTrans.y < 4.5) {
        _bobRate *= -1;
    }

    modelMtx = glm::translate(modelMtx, _shipTrans);
    modelMtx = glm::rotate(modelMtx, _rotateShipAngle, CSCI441::Y_AXIS );
    _drawShipBody(modelMtx, viewMtx, projMtx);        // the body of our ship
    _drawShipWing(true, modelMtx, viewMtx, projMtx);  // the left wing
    _drawShipWing(false, modelMtx, viewMtx, projMtx); // the right wing
    _drawShipNose(modelMtx, viewMtx, projMtx);        // the nose
}

void Ship::flyForward() {
    _shipTrans.x += _moveSpeed * sinf(_rotateShipAngle);
    _shipTrans.z += _moveSpeed * cosf(_rotateShipAngle);
    if (_shipTrans.x > 55) {
        _shipTrans.x -= _moveSpeed;
    }
    if (_shipTrans.x < -55) {
        _shipTrans.x += _moveSpeed;
    }
    if (_shipTrans.z > 55) {
        _shipTrans.z -= _moveSpeed;
    }
    if (_shipTrans.z < -55) {
        _shipTrans.z += _moveSpeed;
    }
}

void Ship::flyBackward() {
    _shipTrans.x -= _moveSpeed * sinf(_rotateShipAngle);
    _shipTrans.z -= _moveSpeed * cosf(_rotateShipAngle);
    if (_shipTrans.x > 55) {
        _shipTrans.x -= _moveSpeed;
    }
    if (_shipTrans.x < -55) {
        _shipTrans.x += _moveSpeed;
    }
    if (_shipTrans.z > 55) {
        _shipTrans.z -= _moveSpeed;
    }
    if (_shipTrans.z < -55) {
        _shipTrans.z += _moveSpeed;
    }
}

void Ship::turnLeft() {
    _rotateShipAngle += _turnRate;
}

void Ship::turnRight() {
    _rotateShipAngle -= _turnRate;
}

glm::vec3 Ship::getPosition() {
    return _shipTrans;
}

void Ship::_drawShipBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorBody[0]);

    CSCI441::drawSolidSphere(2, 10, 20);
}

void Ship::_drawShipWing(bool leftWing, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    if( leftWing )
        modelMtx = glm::rotate( modelMtx, -_rotateWingAngle, CSCI441::Z_AXIS );
    else
        modelMtx = glm::rotate( modelMtx, _rotateWingAngle, CSCI441::Z_AXIS );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorWing[0]);

    CSCI441::drawSolidCone( 1.6, 3, 16, 16 );
}

void Ship::_drawShipNose(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::rotate( modelMtx, _rotateNoseAngle, CSCI441::X_AXIS );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorNose[0]);

    CSCI441::drawSolidCone( 1.4, 3.5, 16, 16 );
}

void Ship::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}




