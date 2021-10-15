/*
 *  CSCI 441, Computer Graphics, Fall 2021
 *
 *  Project: lab04
 *  File: main.cpp
 *
 *  Description:
 *      This file contains the basic setup to work with GLSL shaders.
 *
 *  Author: Dr. Paone, Colorado School of Mines, 2021
 *
 */

#include "MPEngine.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

///*****************************************************************************
//
// Our main function
int main() {

    auto mpEngine = new MPEngine(4, 1, 640, 480, "Midterm Project: The Main Attraction");
    mpEngine->initialize();
    if (mpEngine->getError() == CSCI441::OpenGLEngine::OPENGL_ENGINE_ERROR_NO_ERROR) {
        mpEngine->run();
    }
    mpEngine->shutdown();
    delete mpEngine;

	return EXIT_SUCCESS;
}
