#ifndef INPUT_H
#define INPUT_H


#include "globals.h" // Incluye el cerebro
// --- NUEVO: Declaración de la función para pantalla completa ---
// --- NUEVO: Declaración de la función para pantalla completa ---
void toggleFullscreen(GLFWwindow* window);
void window_focus_callback(GLFWwindow* window, int focused); // --- NUEVO: Para el foco de la ventana ---

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void CalculoCamara(GLFWwindow* window);
void Transicion(GLFWwindow* window);
void StartAnim1(const glm::vec3& camTargetPos, const glm::vec3& camTargetLookAt, double durationSec);
void UpdateAnim1(GLFWwindow* window);
void StartMenu(const glm::vec3& camTargetLookAt, double durationSec);

#endif