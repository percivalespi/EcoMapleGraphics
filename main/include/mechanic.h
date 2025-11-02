#ifndef MECHANIC_H
#define MECHANIC_H

#include "globals.h" // Incluye el cerebro

// Declaraciones de funciones de mecánicas y utilidad
void plantTree();
void startFire();

// Esta es la función de "Raycasting" que usa mouse_button_callback
bool intersectRayAABB(const glm::vec3& ray_origin, const glm::vec3& ray_dir,
    const glm::vec3& aabb_min, const glm::vec3& aabb_max, float& t);

// NOTA: Las declaraciones de Frustum::extractPlanes y Frustum::isBoxInFrustum
// ya están en globals.h (dentro de la struct Frustum), 
// así que no necesitamos repetirlas aquí.

#endif