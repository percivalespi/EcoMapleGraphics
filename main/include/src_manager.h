#ifndef SRC_MANAGER_H
#define SRC_MANAGER_H

#include "globals.h"

// Funciones para cargar modelos con (PHONG-MULTIPLE)
// NOTA: SE CAMBIO A 20 EL Número de Luces

//Cargar - Inicializar Modelos de la ciudad

void initializeModelsForest(ForestAssets& fa);
void initilizeMaterialsForest(ForestAssets& fa);
void initilizeLightsForest(ForestAssets& fa);
void loadForest(ForestAssets& fa);

#endif
