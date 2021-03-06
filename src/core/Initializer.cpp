#include "Initializer.h"
#include <iostream>

Initializer::Initializer()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Initialize the library */
    if (!glfwInit())
        std::cout << "Not inited";

    /* Create a windowed mode window and its OpenGL context */
    m_Window = glfwCreateWindow(800, 600, "OpenGL Boilerplate", NULL, NULL);
    if (!m_Window)
    {
        glfwTerminate();
        std::cout << "Create window failed";
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
    }

    glViewport(0, 0, 800, 600);
    glClearColor(0.1f, 0.3f, 0.7f, 1.0f);
}
