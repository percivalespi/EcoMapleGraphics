#ifndef RENDER_H
#define RENDER_H

#include "globals.h" 

// Manejo de Renderizado
void renderForestScene(const glm::mat4& projection, const glm::mat4& view);
void renderTestEnvironment(const glm::mat4& projection, const glm::mat4& view);
void renderEspacioScene(const glm::mat4& projection, const glm::mat4& view);
void renderMenuScene(const glm::mat4& projection, const glm::mat4& view);
void renderGlaciarScene(const glm::mat4& projection, const glm::mat4& view);

void setupInstanceVBO(unsigned int& vbo, size_t max_items, Model* model_to_setup);
void renderScene(); 
void renderUI();    
void initializeRenderBuffers(UIAssets& ui);
void initializeInstanceBuffers(ForestAssets& fa);

void SetLightUniformInt(Shader* shader, const char* propertyName, size_t lightIndex, int value);
void SetLightUniformFloat(Shader * shader, const char* propertyName, size_t lightIndex, float value);
void SetLightUniformVec4(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec4 value);
void SetLightUniformVec3(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec3 value);


#endif