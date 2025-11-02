#include "src_manager.h"
#include "globals.h"

void initializeModelsForest(ForestAssets& fa) {
    fa.terrain_model = new Model("models/tierra_superficie.fbx");
    fa.grass_model = new Model("models/pastoooyya.fbx");
    fa.rock_model = new Model("models/piedrotanew.fbx");
    fa.mountain_model = new Model("models/montana.fbx");
    fa.tree_model = new Model("models/arce.fbx");
    fa.chopped_once_model = new Model("models/troncomuerto.fbx");
    fa.burning_tree_model = new Model("models/troncoquemado.fbx");
    fa.chopped_twice_model = new Model("models/basecortada.fbx");
    fa.sun_model = new Model("models/sphere.fbx");
    fa.moon_model = new Model("models/moon.fbx");
    fa.cloud_model = new Model("models/cloud.fbx");
    fa.leaf_model = new Model("models/hojaarce.fbx");
    fa.cubeenv = new Model("models/mycube.fbx");
    fa.cubeenv_noche = new Model("models/noche/mycube.fbx");
}

void initilizeMaterialsForest(ForestAssets& fa) {
    fa.defaultMaterial.ambient = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    fa.defaultMaterial.specular = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    fa.defaultMaterial.transparency = 1.0f;
    fa.mountainMaterial.ambient = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    fa.mountainMaterial.specular = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    fa.mountainMaterial.transparency = 1.0f;
    fa.treeMaterial.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    fa.treeMaterial.specular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    fa.treeMaterial.transparency = 1.0f;
    fa.cloudMaterial.ambient = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    fa.cloudMaterial.specular = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    fa.cloudMaterial.transparency = 1.0f;
    fa.leafMaterial.ambient = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    fa.leafMaterial.specular = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    fa.leafMaterial.transparency = 1.0f;
    fa.sunMaterial.ambient = glm::vec4(1.0f, 1.0f, 0.8f, 1.0f);
    fa.sunMaterial.specular = glm::vec4(0.0f);
    fa.sunMaterial.transparency = 1.0f;
}

void initilizeLightsForest(ForestAssets& fa) {
    fa.theLight.Power = glm::vec4(1.5f, 1.5f, 1.5f, 1.0f);
    fa.theLight.Color = glm::vec4(1.0f, 1.0f, 0.95f, 1.0f);
    fa.theLight.alphaIndex = 32;
}

void loadForest(ForestAssets& fa) {
    initilizeLightsForest(fa);
    initilizeMaterialsForest(fa);
    initializeModelsForest(fa);
}
