#include "MPEngine.hpp"
#include <ctime>
#include <CSCI441/objects.hpp>

//*************************************************************************************
//
// Helper Functions

#ifndef M_PI
#define M_PI 3.14159265
#endif

/// \desc Simple helper function to return a random number between 0.0f and 1.0f.
GLfloat getRand() {
    return (GLfloat)rand() / (GLfloat)RAND_MAX;
}

//*************************************************************************************
//
// Public Interface

MPEngine::MPEngine(int OPENGL_MAJOR_VERSION, int OPENGL_MINOR_VERSION,
                   int WINDOW_WIDTH, int WINDOW_HEIGHT, const char* WINDOW_TITLE)
         : CSCI441::OpenGLEngine(OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE) {

    for(auto& _key : _keys) _key = GL_FALSE;

    _mousePosition = glm::vec2(MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED );
    _leftMouseButtonState = GLFW_RELEASE;
    _camToggle = false;
}

MPEngine::~MPEngine() {
    delete _arcBall;
}

void MPEngine::handleKeyEvent(GLint key, GLint action) {
    if(key != GLFW_KEY_UNKNOWN)
        _keys[key] = ((action == GLFW_PRESS) || (action == GLFW_REPEAT));

    if(action == GLFW_PRESS) {
        switch( key ) {
            // quit!
            case GLFW_KEY_C:
                _camToggle = !_camToggle;
                break;
            case GLFW_KEY_Q:
            case GLFW_KEY_ESCAPE:
                setWindowShouldClose();
                break;

            default: break; // suppress CLion warning
        }
    }
}

void MPEngine::handleMouseButtonEvent(GLint button, GLint action) {
    // if the event is for the left mouse button
    if( button == GLFW_MOUSE_BUTTON_LEFT ) {
        // update the left mouse button's state
        _leftMouseButtonState = action;
    }
}

void MPEngine::handleCursorPositionEvent(glm::vec2 currMousePosition) {
    // if mouse hasn't moved in the window, prevent camera from flipping out
    if(_mousePosition.x == MOUSE_UNINITIALIZED) {
        _mousePosition = currMousePosition;
    }

    // if the left mouse button is being held down while the mouse is moving
    if(_leftMouseButtonState == GLFW_PRESS) {
        if (!_camToggle) {
            _freeCam->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                             (_mousePosition.y - currMousePosition.y) * 0.005f );
        }
        else {
            if (_keys[GLFW_KEY_LEFT_SHIFT] || _keys[GLFW_KEY_RIGHT_SHIFT]) {
                if (currMousePosition.y > _mousePosition.y) {
                    _arcBall->moveBackward((currMousePosition.y - _mousePosition.y) * 0.02f);
                }
                else if (currMousePosition.y < _mousePosition.y) {
                    _arcBall->moveForward((_mousePosition.y - currMousePosition.y) * 0.02f);
                }
            }
            else {
                // rotate the camera by the distance the mouse moved
                _arcBall->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                                 (currMousePosition.y - _mousePosition.y) * 0.005f );
            }
        }

    }

    // update the mouse position
    _mousePosition = currMousePosition;
}

//*************************************************************************************
//
// Engine Setup

void MPEngine::_setupGLFW() {
    CSCI441::OpenGLEngine::_setupGLFW();

    // set our callbacks
    glfwSetKeyCallback(_window, MP_keyboard_callback);
    glfwSetMouseButtonCallback(_window, MP_mouse_button_callback);
    glfwSetCursorPosCallback(_window, MP_cursor_callback);
}

void MPEngine::_setupOpenGL() {
    glEnable( GL_DEPTH_TEST );					                    // enable depth testing
    glDepthFunc( GL_LESS );							                // use less than depth test

    glEnable(GL_BLEND);									            // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	            // use one minus blending equation

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	// clear the frame buffer to black
}

void MPEngine::_setupShaders() {
    _lightingShaderProgram = new CSCI441::ShaderProgram("shaders/A3.v.glsl", "shaders/A3.f.glsl" );

    _lightingShaderUniformLocations.mvpMatrix      = _lightingShaderProgram->getUniformLocation("mvpMatrix");
    _lightingShaderUniformLocations.normMatrix     = _lightingShaderProgram->getUniformLocation("normMatrix");
    _lightingShaderUniformLocations.lightDirec     = _lightingShaderProgram->getUniformLocation("lightDirec");
    _lightingShaderUniformLocations.lightColor     = _lightingShaderProgram->getUniformLocation("lightColor");
    _lightingShaderUniformLocations.materialColor  = _lightingShaderProgram->getUniformLocation("materialColor");

    _lightingShaderUniformLocations.camPos      = _lightingShaderProgram->getUniformLocation("camPos");

    _lightingShaderAttributeLocations.vPos         = _lightingShaderProgram->getAttributeLocation("vPos");
    _lightingShaderAttributeLocations.vNorm        = _lightingShaderProgram->getAttributeLocation("vNorm");

}

void MPEngine::_setupBuffers() {
    CSCI441::setVertexAttributeLocations( _lightingShaderAttributeLocations.vPos, _lightingShaderAttributeLocations.vNorm );

    _ship = new Ship(_lightingShaderProgram->getShaderProgramHandle(),
                     _lightingShaderUniformLocations.mvpMatrix,
                     _lightingShaderUniformLocations.normMatrix,
                     _lightingShaderUniformLocations.materialColor);

    _createGroundBuffers();
    _generateEnvironment();
}

void MPEngine::_createGroundBuffers() {
    struct Vertex {
        GLfloat x, y, z;
        GLfloat xNorm, yNorm, zNorm;
    };

    Vertex groundQuad[4] = {
            {-1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f},
            { 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f},
            {-1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f},
            { 1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f}
    };

    GLushort indices[4] = {0,1,2,3};

    _numGroundPoints = 4;

    glGenVertexArrays(1, &_groundVAO);
    glBindVertexArray(_groundVAO);

    GLuint vbods[2];       // 0 - VBO, 1 - IBO
    glGenBuffers(2, vbods);
    glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundQuad), groundQuad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vPos);
    glVertexAttribPointer(_lightingShaderAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vNorm);
    glVertexAttribPointer(_lightingShaderAttributeLocations.vNorm, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 3));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void MPEngine::_generateEnvironment() {
    //******************************************************************
    // parameters to make up our grid size and spacing, feel free to
    // play around with this
    const GLfloat GRID_WIDTH = WORLD_SIZE * 1.8f;
    const GLfloat GRID_LENGTH = WORLD_SIZE * 1.8f;
    const GLfloat GRID_SPACING_WIDTH = 1.0f;
    const GLfloat GRID_SPACING_LENGTH = 1.0f;
    // precomputed parameters based on above
    const GLfloat LEFT_END_POINT = -GRID_WIDTH / 2.0f - 5.0f;
    const GLfloat RIGHT_END_POINT = GRID_WIDTH / 2.0f + 5.0f;
    const GLfloat BOTTOM_END_POINT = -GRID_LENGTH / 2.0f - 5.0f;
    const GLfloat TOP_END_POINT = GRID_LENGTH / 2.0f + 5.0f;
    //******************************************************************

    srand( time(0) );                                                   // seed our RNG

    // psych! everything's on a grid.
    for(int i = LEFT_END_POINT; i < RIGHT_END_POINT; i += GRID_SPACING_WIDTH) {
        for(int j = BOTTOM_END_POINT; j < TOP_END_POINT; j += GRID_SPACING_LENGTH) {
            // don't just draw a building ANYWHERE.
            if( i % 2 && j % 2 && getRand() < 0.2f ) {
                // translate to spot
                glm::mat4 transToSpotMtx = glm::translate( glm::mat4(1.0), glm::vec3(i, 0.0f, j) );

                // compute random height
                GLdouble height = powf(getRand(), 2.5)*10 + 1;
                // scale to building size
                glm::mat4 scaleToHeightMtx = glm::scale( glm::mat4(1.0), glm::vec3(1, height, 1) );

                // translate up to grid
                glm::mat4 transToHeight = glm::translate( glm::mat4(1.0), glm::vec3(0, height/2.0f, 0) );

                // compute full model matrix
                glm::mat4 modelMatrix = transToHeight * scaleToHeightMtx * transToSpotMtx;

                // compute random color
                glm::vec3 color( 0.7, 0.7, 0.7 );
                // store building properties
                BuildingData currentBuilding = {modelMatrix, color};
                _buildings.emplace_back( currentBuilding );
            }
        }
    }
}

void MPEngine::_setupScene() {
    _arcBall = new ArcBall();
    _arcBall->setRadius(10.0f);
    _arcBall->setLookAtPoint(_ship->getPosition());
    _arcBall->setTheta( M_PI / 3.0f );
    _arcBall->setPhi( M_PI / 1.8f );
    _arcBall->recomputeOrientation();

    _freeCam = new CSCI441::FreeCam();
    _freeCam->setPosition( glm::vec3(60.0f, 40.0f, 30.0f) );
    _freeCam->setTheta( -M_PI / 3.0f );
    _freeCam->setPhi( M_PI / 2.8f );
    _freeCam->recomputeOrientation();
    _cameraSpeed = glm::vec2(0.25f, 0.02f);

    glm::vec3 lightDirec = glm::vec3(-1, -1, -1);
    glm::vec3 lightColor = glm::vec3(1, 1, 1);
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(),
                        _lightingShaderUniformLocations.lightDirec,
                        1,
                        &lightDirec[0]);
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(),
                        _lightingShaderUniformLocations.lightColor,
                        1,
                        &lightColor[0]);
}

//*************************************************************************************
//
// Engine Cleanup

void MPEngine::_cleanupShaders() {
    fprintf( stdout, "[INFO]: ...deleting Shaders.\n" );
    delete _lightingShaderProgram;
}

void MPEngine::_cleanupBuffers() {
    fprintf( stdout, "[INFO]: ...deleting VAOs....\n" );
    CSCI441::deleteObjectVAOs();
    glDeleteVertexArrays( 1, &_groundVAO );

    fprintf( stdout, "[INFO]: ...deleting VBOs....\n" );
    CSCI441::deleteObjectVBOs();

    fprintf( stdout, "[INFO]: ...deleting models..\n" );
    delete _ship;
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

void MPEngine::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) {
    // use our lighting shader program
    _lightingShaderProgram->useProgram();

    // camera position can change so send it to shader every frame
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.camPos, 1, &_freeCam->getPosition()[0]);

    //// BEGIN DRAWING THE GROUND PLANE ////
    // draw the ground plane
    glm::mat4 groundModelMtx = glm::scale( glm::mat4(1.0f), glm::vec3(WORLD_SIZE, 1.0f, WORLD_SIZE));
    _computeAndSendMatrixUniforms(groundModelMtx, viewMtx, projMtx);

    glm::vec3 groundColor(0.5f, 0.5f, 0.5f);
    glUniform3fv(_lightingShaderUniformLocations.materialColor, 1, &groundColor[0]);

    glBindVertexArray(_groundVAO);
    glDrawElements(GL_TRIANGLE_STRIP, _numGroundPoints, GL_UNSIGNED_SHORT, (void*)0);
    //// END DRAWING THE GROUND PLANE ////

    //// BEGIN DRAWING THE BUILDINGS ////
    for( const BuildingData& currentBuilding : _buildings ) {
        _computeAndSendMatrixUniforms(currentBuilding.modelMatrix, viewMtx, projMtx);

        glUniform3fv(_lightingShaderUniformLocations.materialColor, 1, &currentBuilding.color[0]);

        CSCI441::drawSolidCube(1.0);
    }
    //// END DRAWING THE BUILDINGS ////

    //// BEGIN DRAWING THE SHIP ////
    glm::mat4 modelMtx(1.0f);
    // draw our ship now
    _ship->drawShip(modelMtx, viewMtx, projMtx);
    //// END DRAWING THE SHIP ////
}

void MPEngine::_updateScene() {
    if (_camToggle) {
        // fly
        if( _keys[GLFW_KEY_W] ) {
            // go forward
            _ship->flyForward();
        }
        if( _keys[GLFW_KEY_S] ) {
            // go backward
            _ship->flyBackward();
        }
        // turn right
        if( _keys[GLFW_KEY_D] ) {
            _ship->turnRight();
        }
        // turn left
        if( _keys[GLFW_KEY_A] ) {
            _ship->turnLeft();
        }
        _arcBall->setLookAtPoint(_ship->getPosition());
        _arcBall->recomputeOrientation();
    }
    else {
        // fly
        if( _keys[GLFW_KEY_SPACE] ) {
            // go backward if shift held down
            if( _keys[GLFW_KEY_LEFT_SHIFT] || _keys[GLFW_KEY_RIGHT_SHIFT] ) {
                _freeCam->moveBackward(_cameraSpeed.x);
            }
                // go forward
            else {
                _freeCam->moveForward(_cameraSpeed.x);
            }
        }
        // turn right
        if( _keys[GLFW_KEY_D] ) {
            _freeCam->rotate(_cameraSpeed.y, 0.0f);
        }
        // turn left
        if( _keys[GLFW_KEY_A] ) {
            _freeCam->rotate(-_cameraSpeed.y, 0.0f);
        }
        // pitch up
        if( _keys[GLFW_KEY_W] ) {
            _freeCam->rotate(0.0f, _cameraSpeed.y);
        }
        // pitch down
        if( _keys[GLFW_KEY_S] ) {
            _freeCam->rotate(0.0f, -_cameraSpeed.y);
        }
    }
}

void MPEngine::run() {
    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while( !glfwWindowShouldClose(_window) ) {	        // check if the window was instructed to be closed
        glDrawBuffer( GL_BACK );				        // work with our back frame buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window

        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore,
        // query what the actual size of the window we are rendering to is.
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize( _window, &framebufferWidth, &framebufferHeight );

        // update the viewport - tell OpenGL we want to render to the whole window
        glViewport( 0, 0, framebufferWidth, framebufferHeight );

        // set the projection matrix based on the window size
        // use a perspective projection that ranges
        // with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
        glm::mat4 projectionMatrix = glm::perspective( 45.0f, (GLfloat) framebufferWidth / (GLfloat) framebufferHeight, 0.001f, 1000.0f );

        // set up our look at matrix to position our camera
        glm::mat4 viewMatrix;
        if (_camToggle) {
            viewMatrix = _arcBall->getViewMatrix();
        }
        else {
            viewMatrix = _freeCam->getViewMatrix();
        }

        // draw everything to the window
        _renderScene(viewMatrix, projectionMatrix);

        _updateScene();

        glfwSwapBuffers(_window);                       // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();				                // check for any events and signal to redraw screen
    }
}

//*************************************************************************************
//
// Private Helper FUnctions

void MPEngine::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.mvpMatrix, mvpMtx);

    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.normMatrix, normalMtx);
}

//*************************************************************************************
//
// Callbacks

void MP_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods ) {
    auto engine = (MPEngine*) glfwGetWindowUserPointer(window);

    // pass the key and action through to the engine
    engine->handleKeyEvent(key, action);
}

void MP_cursor_callback(GLFWwindow *window, double x, double y ) {
    auto engine = (MPEngine*) glfwGetWindowUserPointer(window);

    // pass the cursor position through to the engine
    engine->handleCursorPositionEvent(glm::vec2(x, y));
}

void MP_mouse_button_callback(GLFWwindow *window, int button, int action, int mods ) {
    auto engine = (MPEngine*) glfwGetWindowUserPointer(window);

    // pass the mouse button and action through to the engine
    engine->handleMouseButtonEvent(button, action);
}
