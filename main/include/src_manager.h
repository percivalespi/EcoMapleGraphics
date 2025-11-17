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

/* -------------------------------------------- Recursos Espacio, Menu y Glaciar ------------------------------------------*/
void initializeModelsEspacio(EspacioAssets& ea);
void initilizeMaterialsEspacio(EspacioAssets& ea);
void initilizeLightsEspacio(EspacioAssets& ea);
void loadEspacio(EspacioAssets& ea);

void initializeModelsMenu(MenuAssets& ma);
void loadMenu(MenuAssets& ma);

void initializeModelsGlaciar(GlaciarAssets& ga);
void initilizeMaterialsGlaciar(GlaciarAssets& ga);
void initilizeLightsGlaciar(GlaciarAssets& ga);
void loadGlaciar(GlaciarAssets& ga);

static unsigned int LoadCubemap(const std::vector<std::string>& faces);
void loadFresnelGlassResources();

int addEnvironmentCubemap(const std::vector<std::string>& faces);
// Inicializar varios cubemaps predefinidos
void loadAllEnvironmentCubemaps();






#endif
