#include "Hero2.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

#ifndef M_PI
#define M_PI 3.14159265
#endif

Hero2::Hero2( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {
    _propAngle = 0.0f;
    _propAngleRotationSpeed = M_PI / 16.0f;

    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    _rotatePlaneAngle = M_PI / 2.0f;

    _colorBody = glm::vec3( 0, 0, 1);
    _colorLeg = glm::vec3(0, 0.8, 0);
    _colorArm = glm::vec3(0.9, 0.8, 0.1);
    _colorBlack = glm::vec3(0.0, 0.0, 0.0);
    _scaleArm = glm::vec3( 1.5f, 0.5f, 0.5f );
    _scaleLeg = glm::vec3( 0.75f, 0.5f, 0.5f );
    _scaleBody = glm::vec3( 1.5f, 0.5f, 1.0f );
    _scaleHead = glm::vec3( 1.0f, 1.0f, 1.0f );
    _moveSpeed = 1;
    _rotateHeroAngle = 0;
    _heroTrans = glm::vec3(0, 0 ,0);


}
void Hero2::flyForward() {
    _heroTrans.x += _moveSpeed * sinf(_rotateHeroAngle);
    _heroTrans.z += _moveSpeed * cosf(_rotateHeroAngle);
    if (_heroTrans.x > 55) {
        _heroTrans.x -= _moveSpeed;
    }
    if (_heroTrans.x < -55) {
        _heroTrans.x += _moveSpeed;
    }
    if (_heroTrans.z > 55) {
        _heroTrans.z -= _moveSpeed;
    }
    if (_heroTrans.z < -55) {
        _heroTrans.z += _moveSpeed;
    }
}

void Hero2::flyBackward() {
    _heroTrans.x -= _moveSpeed * sinf(_rotateHeroAngle);
    _heroTrans.z -= _moveSpeed * cosf(_rotateHeroAngle);
    if (_heroTrans.x > 55) {
        _heroTrans.x -= _moveSpeed;
    }
    if (_heroTrans.x < -55) {
        _heroTrans.x += _moveSpeed;
    }
    if (_heroTrans.z > 55) {
        _heroTrans.z -= _moveSpeed;
    }
    if (_heroTrans.z < -55) {
        _heroTrans.z += _moveSpeed;
    }
}

void Hero2::turnLeft() {
    _rotateHeroAngle += 0.1;
}

void Hero2::turnRight() {
    _rotateHeroAngle -= 0.1;
}

glm::vec3 Hero2::getPosition() {
    return _heroTrans;
}
void Hero2::drawHero(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {
    glUseProgram( _shaderProgramHandle );
    if(_up){
        if(_rotation>= 45){
            _up = false;
        }else{
            _rotation ++;
        }
    }else{
        if(_rotation<= -45){
            _up = true;
        }else{
            _rotation --;
        }
    }
    modelMtx = glm::translate(modelMtx, _heroTrans);
    modelMtx = glm::rotate(modelMtx, _rotateHeroAngle, CSCI441::Y_AXIS );
    DrawBody(modelMtx, viewMtx, projMtx);
    _drawLeftArm(modelMtx, viewMtx, projMtx);
    _drawRightArm(modelMtx, viewMtx, projMtx);
    _drawLegs(modelMtx, viewMtx, projMtx);
    _drawHead(modelMtx, viewMtx, projMtx);
}


void Hero2::DrawBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale( modelMtx, _scaleBody );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorBody[0]);

    CSCI441::drawSolidCube( 0.1 );
}void Hero2::_drawLegs(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, glm::vec3( -0.11f, 0.0f, 0.027f ) );

    modelMtx1 = glm::scale( modelMtx1, _scaleLeg );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorLeg[0]);
    CSCI441::drawSolidCube( 0.1 );
    modelMtx1 = glm::translate( modelMtx, glm::vec3( -0.11f, 0.0f, -0.027f ) );

    modelMtx1 = glm::scale( modelMtx1, _scaleLeg );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorLeg[0]);

    CSCI441::drawSolidCube( 0.1 );
}
void Hero2::_drawLeftArm(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, glm::vec3( 0.0f, 0.0f, 0.075f ) );
    modelMtx1 = glm::scale( modelMtx1, _scaleArm );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorArm[0]);

    CSCI441::drawSolidCube( 0.1 );
}
void Hero2::_drawRightArm(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, glm::vec3( 0.0f, 0.0f, -0.075f ) );
    modelMtx1 = glm::scale( modelMtx1, _scaleArm );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorArm[0]);

    CSCI441::drawSolidCube( 0.1 );
}
void Hero2::_drawHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {

    glm::mat4 modelMtx1 = glm::translate( modelMtx, glm::vec3( 0.12f, 0.0f, 0.0 ) );
    modelMtx1 = glm::rotate( modelMtx1, glm::radians(_rotation), CSCI441::X_AXIS );
    modelMtx1 = glm::scale( modelMtx1, _scaleHead );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorArm[0]);

    CSCI441::drawSolidCube( 0.1 );
    modelMtx1 = glm::rotate( modelMtx, glm::radians(_rotation), CSCI441::X_AXIS );
    modelMtx1 = glm::translate( modelMtx1, glm::vec3( 0.12f, -0.05f, 0.0 ) );

    modelMtx1 = glm::scale( modelMtx1, glm::vec3( 0.9f, 0.1f, 0.9f ));

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorBlack[0]);

    CSCI441::drawSolidCube( 0.1 );
}

void Hero2::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}
