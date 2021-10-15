/*
 *  CSCI 441, Computer Graphics, Fall 2021
 *
 *  Project: lab02
 *  File: main.cpp
 *
 *	Author: Jeffrey Paone - Fall 2021
 *
 *  Description:
 *      Contains the base code for a basic flight simulator.
 *
 */

#include "A3Engine.hpp"

//**************************************************************************************
//
// Our main function

int main() {
    auto labEngine = new A3Engine(4, 1, 640, 640, "Lab02: Flight Simulator v0.41");
    labEngine->initialize();
    if (labEngine->getError() == CSCI441::OpenGLEngine::OPENGL_ENGINE_ERROR_NO_ERROR) {
        labEngine->run();
    }
    labEngine->shutdown();
    delete labEngine;

	return EXIT_SUCCESS;				// exit our program successfully!
}
