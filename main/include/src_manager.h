#ifndef SRC_MANAGER_H
#define SRC_MANAGER_H

#include "globals.h"

// Funciones para cargar modelos con (PHONG-MULTIPLE)
// NOTA: SE CAMBIO A 20 EL Número de Luces

//Cargar - Inicializar Modelos de la ciudad

/* -------------------------------------------- Recursos TEST ------------------------------------------*/
void initializeModelsTest(TestAssets& fa);
void initilizeMaterialsTest(TestAssets& fa);
void initilizeLightsTest(TestAssets& fa);
void loadTest(TestAssets& fa);


/* -------------------------------------------- Recursos UI ------------------------------------------*/
void loadUI(UIAssets& ui);

/* -------------------------------------------- Recursos Bosque ------------------------------------------*/
void initializeModelsForest(ForestAssets& fa);
void initilizeMaterialsForest(ForestAssets& fa);
void initilizeLightsForest(ForestAssets& fa);
void loadForest(ForestAssets& fa);






#endif
