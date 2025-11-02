#ifndef RENDER_H
#define RENDER_H

#include "globals.h" 

// Declaraciones de funciones de Renderizado
void setupInstanceVBO(unsigned int& vbo, size_t max_items, Model* model_to_setup);
void renderScene(); // <-- Nueva! Para toda la escena 3D
void renderUI();    // <-- Nueva! Para toda la interfaz 2D


#endif