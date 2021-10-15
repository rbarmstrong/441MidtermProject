#ifndef LAB02_ENGINE_HPP
#define LAB02_ENGINE_HPP

#include <CSCI441/OpenGLEngine.hpp>
#include <CSCI441/ShaderProgram.hpp>
#include "ArcCam.hpp"

#include <glm/glm.hpp>
#include "Hero.hpp"
#include <vector>

class A3Engine : public CSCI441::OpenGLEngine {
public:
    A3Engine(int OPENGL_MAJOR_VERSION, int OPENGL_MINOR_VERSION,
                int WINDOW_WIDTH, int WINDOW_HEIGHT,
                const char* WINDOW_TITLE);
    ~A3Engine();

    void run() final;

    /// \desc returns a pointer to our free cam object
    [[nodiscard]] ArcCam* getFreeCam() const { return _freeCam; }

    /// \desc returns the last location of the mouse in window coordinates
    [[nodiscard]] glm::vec2 getMousePosition() const { return _mousePosition; }
    /// \desc updates the last location of the mouse
    /// \param mousePos new mouse location in window coordinates
    void setMousePosition(glm::vec2 mousePos) { _mousePosition = mousePos; }

    /// \desc returns the current state of the left mouse button
    [[nodiscard]] GLint getLeftMouseButtonState() const { return _leftMouseButtonState; }
    /// \desc updates the current state of the left mouse button
    /// \param state current left mouse button state
    void setLeftMouseButtonState(GLint state) { _leftMouseButtonState = state; }

    /// \desc value off-screen to represent mouse has not begun interacting with window yet
    static constexpr GLfloat MOUSE_UNITIALIZED = -9999.0f;

    void moveForward(double d);

    void moveBackward(double d);

    bool _radiusMove;
    GLfloat _R;
    float _degrees;
private:
    void _setupGLFW() final;
    void _setupOpenGL() final;
    void _setupShaders() final;
    void _setupBuffers() final;
    void _setupScene() final;
    float _X;
    float _Y;
    void _cleanupBuffers() final;
    void _cleanupShaders() final;

    /// \desc Creates a VAO to draw a simple grid environment with buildings
    /// for the user to navigate through.
    void _generateEnvironment();

    void _renderScene(glm::mat4 viewMtx, glm::mat4 projMtx);



    /// \desc VAO representing our ground plane grid
    GLuint _gridVAO;
    /// \desc tracks the number of vertices comprising the ground plane
    GLuint _numGridPoints;
    /// \desc color to draw the ground plane grid
    glm::vec3 _gridColor;

    /// \desc the camera in our world set up as a ArcCam
    ArcCam* _freeCam;
    static constexpr GLuint NUM_KEYS = GLFW_KEY_LAST;
    GLboolean _keys[NUM_KEYS];
    /// \desc last location of the mouse in window coordinates
    glm::vec2 _mousePosition;
    /// \desc current state of the left mouse button
    GLint _leftMouseButtonState;
    glm::vec2 _cameraSpeed;

    /// \desc our plane model

    Hero* hero;

    /// \desc the size of the world (controls the ground size and locations of buildings)
    static constexpr GLfloat WORLD_SIZE = 55.0f;
    /// \desc VAO for our ground
    GLuint _groundVAO;
    /// \desc the number of points that make up our ground object
    GLsizei _numGroundPoints;

    /// \desc creates the ground VAO
    void _createGroundBuffers();

    /// \desc smart container to store information specific to each building we wish to draw
    struct BuildingData {
        /// \desc transformations to position and size the building
        glm::mat4 modelMatrix;
        /// \desc color to draw the building
        glm::vec3 color;
    };
    /// \desc information list of all the buildings to draw
    std::vector<BuildingData> _buildings;

    /// \desc shader program that performs lighting
    CSCI441::ShaderProgram* _lightingShaderProgram = nullptr;   // the wrapper for our shader program
    /// \desc stores the locations of all of our shader uniforms
    struct LightingShaderUniformLocations {
        /// \desc precomputed MVP matrix location
        GLint mvpMatrix;
        /// \desc material diffuse color location
        GLint materialColor;

        GLint lightDir;
        GLint lightCol;
        GLint normMat;

    } _lightingShaderUniformLocations;
    /// \desc stores the locations of all of our shader attributes
    struct LightingShaderAttributeLocations {
        /// \desc vertex position location
        GLint vPos;

        GLint vertex;

    } _lightingShaderAttributeLocations;

    /// \desc precomputes the matrix uniforms CPU-side and then sends them
    /// to the GPU to be used in the shader for each vertex.  It is more efficient
    /// to calculate these once and then use the resultant product in the shader.
    /// \param modelMtx model transformation matrix
    /// \param viewMtx camera view matrix
    /// \param projMtx camera projection matrix
    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;

    void _renderScene();

    //Camera *_currentCam;
};

void lab02_keyboard_callback( GLFWwindow *window, int key, int scancode, int action, int mods );
void lab02_cursor_callback( GLFWwindow *window, double x, double y );
void lab02_mouse_button_callback( GLFWwindow *window, int button, int action, int mods );

#endif // LAB02_ENGINE_HPP