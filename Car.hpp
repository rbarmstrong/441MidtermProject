#ifndef A3_CAR_HPP
#define A3_CAR_HPP

#include <GL/glew.h>

#include <glm/glm.hpp>

class Car {
public:
    /// \desc creates a simple plane that gives the appearance of flight
    /// \param shaderProgramHandle shader program handle that the plane should be drawn using
    /// \param mvpMtxUniformLocation uniform location for the full precomputed MVP matrix
    /// \param normalMtxUniformLocation uniform location for the precomputed Normal matrix
    /// \param materialColorUniformLocation uniform location for the material diffuse color
    Car(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation );

    void drawCar( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx );

    glm::vec3 getPosition();

    GLfloat getAngle();

private:
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

    GLfloat _rotate90deg;

    glm::vec3 _colorBody;
    glm::vec3 _scaleBody;

    glm::vec3 _colorWheel;

    glm::vec3 _colorNose;

    glm::vec3 _wheelTrans;
    glm::vec3 _wheelTrans2;
    glm::vec3 _wheelTransY;

    bool _idleIsUp;
    GLfloat _idleMove;

    void _drawCarBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    void _drawWheel(bool leftWing, bool frontWheel, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    void _drawCarTopper(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx );

    void _drawCarNose(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    /// \desc precomputes the matrix uniforms CPU-side and then sends them
    /// to the GPU to be used in the shader for each vertex.  It is more efficient
    /// to calculate these once and then use the resultant product in the shader.
    /// \param modelMtx model transformation matrix
    /// \param viewMtx camera view matrix
    /// \param projMtx camera projection matrix
    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;

    void flyBackward();

    void flyForward();

    void turnLeft();

    void turnRight();

    glm::vec3 _carTrans;
    float _moveSpeed;
    float _rotateCarAngle;

};


#endif // A3_CAR_HPP
