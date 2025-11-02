#ifndef INPUT_H
#define INPUT_H


#include "globals.h" // Incluye el cerebro

// Declaraciones de funciones de input
// Estas son las 5 funciones que vamos a mover
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

#endif