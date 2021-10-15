#include "A3Engine.hpp"

#include <CSCI441/objects.hpp>
#include <CSCI441/SimpleShader.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <cstdio>
#include <cstdlib>
#include <ctime>


#ifndef M_PI
#define M_PI 3.14159265
#endif

//*************************************************************************************
//
// Helper Functions

/// \desc Simple helper function to return a random number between 0.0 and 1.0
/// \note granularity and rand normalization is dependent on the value of RAND_MAX
GLdouble getRand() { return rand() / (GLdouble)RAND_MAX; }

//*************************************************************************************
//
// Engine Setup

A3Engine::A3Engine(int OPENGL_MAJOR_VERSION, int OPENGL_MINOR_VERSION,
                         int WINDOW_WIDTH, int WINDOW_HEIGHT, const char*WINDOW_TITLE)
      : CSCI441::OpenGLEngine(OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE) {
    for(auto& _key : _keys) _key = GL_FALSE;
    _gridVAO = 0;
    _numGridPoints = 0;
    _gridColor = glm::vec3(1.0f, 1.0f, 1.0f);
    _freeCam = new ArcCam();
    //_currentCam = new Camera();
    _mousePosition = glm::vec2(MOUSE_UNITIALIZED, MOUSE_UNITIALIZED );
    _leftMouseButtonState = GLFW_RELEASE;
    _X = 0;
    _Y =0;
    _R = 0.5;
    _degrees =0;
}

A3Engine::~A3Engine() {
    delete _freeCam;
}

void A3Engine::_setupGLFW() {
    CSCI441::OpenGLEngine::_setupGLFW();                                // set up our OpenGL context

    glfwSetKeyCallback( _window, lab02_keyboard_callback );             // set our keyboard callback function
    glfwSetCursorPosCallback( _window, lab02_cursor_callback );         // set our cursor position callback function
    glfwSetMouseButtonCallback( _window, lab02_mouse_button_callback ); // set our mouse button callback function
}

void A3Engine::_setupOpenGL() {
    glEnable( GL_DEPTH_TEST );                                          // tell OpenGL to perform depth testing with
                                                                        // the Z-Buffer to perform hidden surface removal
                                                                        // We will discuss this more very soon.
    glDepthFunc( GL_LESS );							                // use less than depth test

    glEnable(GL_BLEND);									            // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	    // set the clear color to black
}

void A3Engine::_setupShaders() {
    _lightingShaderProgram = new CSCI441::ShaderProgram("shaders/lab05.v.glsl", "shaders/lab05.f.glsl" );
    _lightingShaderUniformLocations.mvpMatrix      = _lightingShaderProgram->getUniformLocation("mvpMatrix");
    _lightingShaderUniformLocations.lightDir      = _lightingShaderProgram->getUniformLocation("lightDir");
    _lightingShaderUniformLocations.lightCol      = _lightingShaderProgram->getUniformLocation("lightCol");
    _lightingShaderUniformLocations.normMat      = _lightingShaderProgram->getUniformLocation("normMat");
    _lightingShaderUniformLocations.materialColor  = _lightingShaderProgram->getUniformLocation("materialColor");
    _lightingShaderAttributeLocations.vPos         = _lightingShaderProgram->getAttributeLocation("vPos");
    _lightingShaderAttributeLocations.vertex         = _lightingShaderProgram->getAttributeLocation("vertex");
}

void A3Engine::_setupBuffers() {
    CSCI441::setVertexAttributeLocations(_lightingShaderAttributeLocations.vPos, _lightingShaderAttributeLocations.vertex);

    hero = new Hero(_lightingShaderProgram->getShaderProgramHandle(),
                    _lightingShaderUniformLocations.mvpMatrix,
                    _lightingShaderUniformLocations.normMat,
                    _lightingShaderUniformLocations.materialColor);

    _createGroundBuffers();
    _generateEnvironment();
}

void A3Engine::_createGroundBuffers() {

    struct Vertex {
        GLfloat x, y, z;
        GLfloat verx, very, verz;

    };


    Vertex groundQuad[4] = {
            {-1.0f, 0.0f, -1.0f, -1.0f, 1.0f, -1.0f},
            { 1.0f, 0.0f, -1.0f, 1.0f, 1.0f, -1.0f},
            {-1.0f, 0.0f,  1.0f, -1.0f, 1.0f,  1.0f},
            { 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f}
    };

    GLushort indices[4] = {0,1,2,3};

    _numGroundPoints = 4;

    glGenVertexArrays(1, &_groundVAO);
    glBindVertexArray(_groundVAO);

    GLuint vbods[2];
    glGenBuffers(2, vbods);
    glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundQuad), groundQuad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vPos);
    glVertexAttribPointer(_lightingShaderAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);


    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vertex);
    glVertexAttribPointer(_lightingShaderAttributeLocations.vertex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*3));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void A3Engine::_setupScene() {
    _freeCam->setLookAtPoint(glm::vec3(_X, 0.15, _Y));
    _freeCam->setRadius(_R);

    _freeCam->setTheta( -M_PI / 3.0f );
    _freeCam->setPhi( M_PI / 2.8f );
    _freeCam->recomputeOrientation();



    glm::vec3 dir;
    dir = glm::vec3(-1, -1, -1);
    glm::vec3 col = glm::vec3(1, 1, 1);
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightDir, 1, &dir[0]);
    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightCol, 1, &col[0]);

}
void A3Engine::_cleanupShaders() {
    fprintf( stdout, "[INFO]: ...deleting Shaders.\n" );
    delete _lightingShaderProgram;
}
void A3Engine::_cleanupBuffers() {
    fprintf( stdout, "[INFO]: ...deleting VAOs....\n" );
    CSCI441::deleteObjectVAOs();
    glDeleteVertexArrays( 1, &_groundVAO );

    fprintf( stdout, "[INFO]: ...deleting VBOs....\n" );
    CSCI441::deleteObjectVBOs();

    fprintf( stdout, "[INFO]: ...deleting models..\n" );
    delete hero;
}
void A3Engine::_generateEnvironment() {
    //******************************************************************
    // parameters to make up our grid size and spacing, feel free to
    // play around with this
    const GLfloat GRID_WIDTH = 100;
    const GLfloat GRID_LENGTH = 100;
    const GLfloat GRID_SPACING_WIDTH = 1.0f;
    const GLfloat GRID_SPACING_LENGTH = 1.0f;
    // precomputed parameters based on above
    const GLfloat LEFT_END_POINT = -GRID_WIDTH / 2.0f - 5.0f;
    const GLfloat RIGHT_END_POINT = GRID_WIDTH / 2.0f + 5.0f;
    const GLfloat BOTTOM_END_POINT = -GRID_LENGTH / 2.0f - 5.0f;
    const GLfloat TOP_END_POINT = GRID_LENGTH / 2.0f + 5.0f;
    //******************************************************************

    srand( time(nullptr) );                                             // seed our RNG
    for(int i =LEFT_END_POINT; i < RIGHT_END_POINT; i++){
        for(int j =BOTTOM_END_POINT; j < TOP_END_POINT; j++){
            double rand = getRand();
            if(i!=0 && j !=0) {
                if (i % 2 == 0 && j % 2 == 0 && rand < 0.4) {
                    BuildingData currentBuilding;
                    double y = getRand() * 10;
                    currentBuilding.modelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(i, y / 2, j)) *
                                                  glm::scale(glm::mat4(1.0), glm::vec3(1, y, 1));
                    currentBuilding.color = glm::vec3(getRand(), getRand(), getRand());
                    _buildings.push_back(currentBuilding);
                }
            }

        }


    }

    //******************************************************************
    // draws a grid as our ground plane
    // do not edit this next section
    std::vector< glm::vec3 > points;
    // draw horizontal lines
    for(GLfloat i = LEFT_END_POINT; i <= RIGHT_END_POINT; i += GRID_SPACING_WIDTH) {
        points.emplace_back( glm::vec3(i, 0, BOTTOM_END_POINT) );
        points.emplace_back( glm::vec3(i, 0, TOP_END_POINT) );
    }
    // draw vertical lines
    for(GLfloat j = BOTTOM_END_POINT; j <= TOP_END_POINT; j += GRID_SPACING_LENGTH) {
        points.emplace_back( glm::vec3(LEFT_END_POINT, 0, j) );
        points.emplace_back( glm::vec3(RIGHT_END_POINT, 0, j) );
    }
    _gridVAO = CSCI441::SimpleShader3::registerVertexArray(points, std::vector<glm::vec3>(points.size()));
    _numGridPoints = points.size();
    _gridColor = glm::vec3(1.0f, 1.0f, 1.0f);
    //******************************************************************
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

void A3Engine::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) {
    // use our lighting shader program
    _lightingShaderProgram->useProgram();

    //// BEGIN DRAWING THE GROUND PLANE ////
    // draw the ground plane
    glm::mat4 groundModelMtx = glm::scale( glm::mat4(1.0f), glm::vec3(WORLD_SIZE, 1.0f, WORLD_SIZE));
    _computeAndSendMatrixUniforms(groundModelMtx, viewMtx, projMtx);

    glm::vec3 groundColor(1.0f, 1.0f, 1.0f);
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

    //// BEGIN DRAWING THE PLANE ////
    glm::mat4 modelMtx(glm::translate( glm::mat4(1.0), glm::vec3(_X, 0.15, _Y) ));

    modelMtx = glm::rotate( modelMtx, glm::radians(90.0f-_degrees), CSCI441::Y_AXIS );
    // rotate the plane with our camera phi direction
    modelMtx = glm::rotate( modelMtx,  0.0f, CSCI441::X_AXIS );
    // draw our plane now
    hero->drawHero(modelMtx, viewMtx, projMtx);
    //// END DRAWING THE PLANE ////
}

void A3Engine::run() {
    printf("\nControls:\n");
    printf("\tW / S - forwards / backwards:\n");
    printf("\tMouse Drag - Pan camera:\n");
    printf("\tQ / ESC - quit\n");

    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while( !glfwWindowShouldClose(_window) ) {	// check if the window was instructed to be closed
        glDrawBuffer( GL_BACK );				// work with our back frame buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window

        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore,
        // query what the actual size of the window we are rendering to is.
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize( _window, &framebufferWidth, &framebufferHeight );

        // update the viewport - tell OpenGL we want to render to the whole window
        glViewport( 0, 0, framebufferWidth, framebufferHeight );

        // update the projection matrix based on the window size
        // the GL_PROJECTION matrix governs properties of the view coordinates;
        // i.e. what gets seen - use a perspective projection that ranges
        // with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
        glm::mat4 projectionMatrix = glm::perspective( 45.0f, (GLfloat) framebufferWidth / (GLfloat) framebufferHeight, 0.001f, 1000.0f );
        glm::mat4 viewMatrix = _freeCam->getViewMatrix();
        _renderScene(viewMatrix, projectionMatrix);
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
}
void A3Engine::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {

    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;

    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.mvpMatrix, mvpMtx);


    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.normMat, normalMtx);


}
void A3Engine::moveForward(double d) {
    _X += d*cos(glm::radians(_degrees));
    _Y += d*sin(glm::radians(_degrees));
    if(_X>=10){
        _X= 10;
    }
    if(_Y>= 10){
        _Y = 10;
    }
    if(_X<=-10){
        _X= -10;
    }
    if(_Y<= -10){
        _Y = -10;
    }
    _freeCam->setLookAtPoint(glm::vec3(_X, 0.15, _Y));
    _freeCam->setRadius(_R);
    _freeCam->recomputeOrientation();
}

void A3Engine::moveBackward(double d) {
    _X -= d*cos(glm::radians(_degrees));
    _Y -= d*sin(glm::radians(_degrees));
    if(_X>=10){
        _X= 10;
    }
    if(_Y>= 10){
        _Y = 10;
    }
    if(_X<=-10){
        _X= -10;
    }
    if(_Y<= -10){
        _Y = -10;
    }
    _freeCam->setLookAtPoint(glm::vec3(_X, 0.15, _Y));
    _freeCam->setRadius(_R);
    _freeCam->recomputeOrientation();
}

//*************************************************************************************
//
// Callbacks

void lab02_keyboard_callback( GLFWwindow *window, int key, int scancode, int action, int mods ) {
    auto engine = (A3Engine*) glfwGetWindowUserPointer(window);
    engine-> _radiusMove =false;
    if( action == GLFW_PRESS ) {
        switch( key ) {

            case GLFW_KEY_W:
                engine->moveForward(0.01);
                break;
            case GLFW_KEY_S:
                engine->moveBackward(0.01);
                break;
            case GLFW_KEY_D:
                engine->_degrees+=1;
                break;
            case GLFW_KEY_A:
                engine->_degrees-=1;
                break;
            case GLFW_KEY_LEFT_SHIFT:
                engine-> _radiusMove =true;
                break;
            case GLFW_KEY_ESCAPE:
            case GLFW_KEY_Q:
                engine->setWindowShouldClose();
                break;

            default: break; // to remove CLion warning
        }
    }
    if( action == GLFW_REPEAT ) {
        switch( key ) {

            case GLFW_KEY_W:
                engine->moveForward(0.01);
                break;
            case GLFW_KEY_S:
                engine->moveBackward(0.01);
                break;
            case GLFW_KEY_D:
                engine->_degrees+=1;
                break;
            case GLFW_KEY_A:
                engine->_degrees-=1;
                break;
            case GLFW_KEY_LEFT_SHIFT:
                engine-> _radiusMove =true;
                break;
            default: break; // to remove CLion warning
        }
    }
}

void lab02_cursor_callback( GLFWwindow *window, double x, double y ) {
    auto engine = (A3Engine*) glfwGetWindowUserPointer(window);
    glm::vec2 lastMousePosition = engine->getMousePosition();
    glm::vec2 currMousePosition = glm::vec2(x, y);

    // if mouse hasn't moved in the window, prevent camera from flipping out
    if(lastMousePosition.x == A3Engine::MOUSE_UNITIALIZED) {
        lastMousePosition = currMousePosition;
    }

    // if the left mouse button is being held down while the mouse is moving
    if(engine->getLeftMouseButtonState() == GLFW_PRESS) {
        if(engine->_radiusMove!= true) {
            GLfloat dTheta = (0.005) * (lastMousePosition.x - currMousePosition.x);  // TODO #5
            GLfloat dPhi = (0.005) * (lastMousePosition.y - currMousePosition.y);    // TODO $6
            engine->getFreeCam()->rotate(dTheta, dPhi);
        }else{
            GLfloat R = (0.005) * (lastMousePosition.y - currMousePosition.y);
            engine->_R = engine->_R + R;
            engine->getFreeCam()->setRadius(engine->_R);
            engine->getFreeCam()->recomputeOrientation();
        }
    }

    // update the last mouse position
    engine->setMousePosition(currMousePosition);
}

void lab02_mouse_button_callback( GLFWwindow *window, int button, int action, int mods ) {
    auto engine = (A3Engine*) glfwGetWindowUserPointer(window);

    // if the event is for the left mouse button
    if( button == GLFW_MOUSE_BUTTON_LEFT ) {
        // update the left mouse button's state within the engine
        engine->setLeftMouseButtonState( action );
    }
}