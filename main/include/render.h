#ifndef RENDER_H
#define RENDER_H

#include "globals.h" 

// Declaraciones de funciones de Renderizado
void setupInstanceVBO(unsigned int& vbo, size_t max_items, Model* model_to_setup);
void renderScene(); 
void renderUI();    
void initializeRenderBuffers(UIAssets& ui);
void initializeInstanceBuffers(ForestAssets& fa);

#endif