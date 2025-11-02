#include "src_manager.h"
#include "globals.h"

/* -------------------------------------------- - Manejo de Recursos TEST------------------------------------------*/
void initializeModelsTest(TestAssets& ta) {
    ta.car = new Model("models/car.fbx");
    ta.luminaire = new Model("models/luminaire.fbx");
    ta.stop = new Model("models/stop.fbx");
    ta.floor = new Model("models/floor02.fbx");
}

void initilizeMaterialsTest(TestAssets& ta) {
    ta.steel.ambient = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
    ta.steel.diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    ta.steel.specular = glm::vec4(0.7745f, 0.774f, 0.774f, 1.0f);

    ta.asphalt.ambient = glm::vec4(0.02f, 0.02f, 0.02f, 1.0f);
    ta.asphalt.diffuse = glm::vec4(0.12f, 0.12f, 0.12f, 1.0f);
    ta.asphalt.specular = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);

}

void initilizeLightsTest(TestAssets& ta) {
    ta.light01.Position = glm::vec3(1.4895f, 1.8836f, -1.1042f);
    ta.light01.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ta.light01.Power = glm::vec4(90.0f, 90.0f, 90.0f, 1.0f); // Potencia en Watts
    ta.light01.alphaIndex = 50; 
    ta.light01.distance = 7.0f;
    gLights.push_back(ta.light01);

    ta.light02.Position = glm::vec3(-1.8224f, 0.1272f, 1.1042f);
    ta.light02.Color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    ta.light01.alphaIndex = 16;
    gLights.push_back(ta.light02);
}

void loadTest(TestAssets& ta) {
    initilizeLightsTest(ta);
    initilizeMaterialsTest(ta);
    initializeModelsTest(ta);
}




/* -------------------------------------------- - Manejo de Recursos UI------------------------------------------*/
void loadUI(UIAssets& ui) {
    // --- MODIFICADO: CARGAR LEYENDAS ---
    // --- Cargar Texturas UI ---
    ui.fireTextureID = TextureFromFile("fire.png", "models/image");
    ui.treeTextureID = TextureFromFile("tree.png", "models/image");
    ui.highlightTextureID = TextureFromFile("highlight.png", "models/image");

    // --- NUEVO: Cargar texturas de leyendas ---
    ui.legendFireTextureID = TextureFromFile("legend_fire.png", "models/image");
    ui.legendTreeTextureID = TextureFromFile("legend_tree.png", "models/image");
    // -------------------------------------------

    if (ui.fireTextureID == 0) {
        std::cerr << "ERROR: Failed to load 'models/image/fire.png'" << std::endl;
    }
    if (ui.treeTextureID == 0) {
        std::cerr << "ERROR: Failed to load 'models/image/tree.png'" << std::endl;
    }
    if (ui.highlightTextureID == 0) {
        std::cerr << "ERROR: Failed to load 'models/image/highlight.png'" << std::endl;
    }

    // --- NUEVO: Comprobaciones de error para leyendas ---
    if (ui.legendFireTextureID == 0) {
        std::cerr << "ERROR: Failed to load 'models/image/legend_fire.png'" << std::endl;
    }
    if (ui.legendTreeTextureID == 0) {
        std::cerr << "ERROR: Failed to load 'models/image/legend_tree.png'" << std::endl;
    }
}


/* -------------------------------------------- - Manejo de Recursos Bosque------------------------------------------*/
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
    
    // Ver si se  cargaron los modelos (Se va?)
    if (!fa.chopped_once_model) {
        std::cerr << "ERROR: Could not load troncomuerto.fbx" << std::endl;
    }
    if (!fa.burning_tree_model) {
        std::cerr << "ERROR: Could not load troncoquemado.fbx" << std::endl;
    }
    if (!fa.chopped_twice_model) {
        std::cerr << "ERROR: Could not load basecortada.fbx" << std::endl;
    }
    // CORREGIDO: Usar -> en punteros
    if (!fa.cubeenv || fa.cubeenv->meshes.empty() || fa.cubeenv->meshes[0].textures.empty()) {
        std::cout << "ERROR: Skybox DIA" << std::endl;
    }
    if (!fa.cubeenv_noche || fa.cubeenv_noche->meshes.empty() || fa.cubeenv_noche->meshes[0].textures.empty()) {
        std::cout << "ERROR: Skybox NIGHT" << std::endl;
    }
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

