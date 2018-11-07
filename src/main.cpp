//
//  main.cpp
//
//  Copyright © 2018 Alun Evans. All rights reserved.
//
#include "includes.h"
#include "Game.h"

Game* GAME = nullptr;

bool glCheckError() {
    GLenum errCode;
    if ((errCode = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << errCode << std::endl;
        return false;
    }
    return true;
}

void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW ERROR: code " << error << "; msg: " << description << std::endl;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //quit
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        std::cout << "Left mouse button down!" << std::endl;
        //TODO: update game mouse position
    }
}

int main(void)
{
    // register the error call-back function before doing anything else
    glfwSetErrorCallback(glfw_error_callback);
    
    //create window pointer
    GLFWwindow* window;
    
    // Initialize the library
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(600, 600, "Hello OpenGL!", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    // Make the window's context current
    glfwMakeContextCurrent(window);
    
    //initialise GLEW
    glewExperimental = GL_TRUE;
    glewInit();
    
    //get info about OpenGL version
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "Renderer: "<< renderer << "; version: " << version << std::endl;
    
    //GLFW input functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
       
	//create game singleton and initialise it
	GAME = new Game();
	GAME->init();

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
		// Poll window events once a frame to get input etc
		glfwPollEvents();

		//update game
		GAME->update();
		glfwSwapBuffers(window);
    }

	//free game memory - not necessary but good practice!
	delete GAME;

    //terminate glfw and exit
    glfwTerminate();
    return 0;
}


