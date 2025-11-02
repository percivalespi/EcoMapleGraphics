#ifndef MECHANIC_H
#define MECHANIC_H

#include "globals.h" // Incluye el cerebro

// Declaraciones de funciones de mecánicas y utilidad
void plantTree();
void startFire();

// Esta es la función de "Raycasting" que usa mouse_button_callback
bool intersectRayAABB(const glm::vec3& ray_origin, const glm::vec3& ray_dir,
    const glm::vec3& aabb_min, const glm::vec3& aabb_max, float& t);

void updateGameLogic();

//Esta funcion se va a mover a otra clase a futuro para generar de difente manera entornos
void generateForest();

void generateCity();


#endif