#ifndef MP_HERO_HPP
#define MP_HERO_HPP

#include <GL/glew.h>

#include <glm/glm.hpp>

class Hero {
public:
    /// \desc creates a simple plane that gives the appearance of flight
    /// \param shaderProgramHandle shader program handle that the plane should be drawn using
    /// \param mvpMtxUniformLocation uniform location for the full precomputed MVP matrix
    /// \param normalMtxUniformLocation uniform location for the precomputed Normal matrix
    /// \param materialColorUniformLocation uniform location for the material diffuse color
    Hero( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation );

    /// \desc draws the model plane for a given MVP matrix
    /// \param modelMtx existing model matrix to apply to plane
    /// \param viewMtx camera view matrix to apply to plane
    /// \param projMtx camera projection matrix to apply to plane
    /// \note internally uses the provided shader program and sets the necessary uniforms
    /// for the MVP and Normal Matrices as well as the material diffuse color
    void drawHero(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx );

    /// \desc simulates the plane flying forward by rotating the propeller clockwise
    void flyForward();
    /// \desc simulates the plane flying backward by rotating the propeller counter-clockwise
    void flyBackward();

    glm::vec3 getPosition();

    GLfloat getAngle();
    void turnLeft();
    void turnRight();

private:
    /// \desc current angle of rotation for the propeller
    GLfloat _propAngle;
    /// \desc one rotation step
    GLfloat _propAngleRotationSpeed;

    /// \desc handle of the shader program to use when drawing the plane
    GLuint _shaderProgramHandle;
    /// \desc stores the uniform locations needed for the plan information
    struct ShaderProgramUniformLocations {
        /// \desc location of the precomputed ModelViewProjection matrix
        GLint mvpMtx;
        /// \desc location of the precomputed Normal matrix
        GLint normalMtx;
        /// \desc location of the material diffuse color
        GLint materialColor;
    } _shaderProgramUniformLocations;

    /// \desc angle to rotate our plane at
    GLfloat _rotatePlaneAngle;

    /// \desc color the plane's body
    glm::vec3 _colorBody;
    /// \desc amount to scale the plane's body by
    glm::vec3 _scaleBody;
    glm::vec3 _scaleArm;
    /// \desc color the plane's wing
    glm::vec3 _colorWing;
    glm::vec3 _colorLeg;
    glm::vec3 _colorArm;

    /// \desc draws just the plane's body
    /// \param modelMtx existing model matrix to apply to plane
    /// \param viewMtx camera view matrix to apply to plane
    /// \param projMtx camera projection matrix to apply to plane
    void DrawBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;

    void _drawLeftArm(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;

    void _drawRightArm(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;

    void _drawLegs(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;

    glm::vec3 _scaleLeg;

    void _drawHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;

    glm::vec3 _scaleHead;
    glm::vec3 _colorBlack;
    float _rotation;
    bool _up;
    float _moveSpeed;
    float _rotateHeroAngle;
    glm::vec3  _heroTrans;



};


#endif //LAB05_PLANE_HPP
