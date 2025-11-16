#include "src_manager.h"
#include "globals.h"

/* -------------------------------------------- - Manejo de Recursos TEST------------------------------------------*/
void initializeModelsTest(TestAssets& ta) {

    ta.suelo = new Model("models/city/Prod/suelo.fbx");
    ta.suelo_verde = new Model("models/city/Prod/sueloVerde.fbx");
    ta.metales = new Model("models/city/Prod/metales.fbx");
    ta.objMadera = new Model("models/city/Prod/elementosMadera.fbx");
    ta.objCristales = new Model("models/city/Prod/objCristales3.fbx");
    ta.objPlasticos = new Model("models/city/Prod/objPlasticos3.fbx");
    ta.objConcreto = new Model("models/city/Prod/objConcreto.fbx");
    ta.objLlantas = new Model("models/city/Prod/objLlantafbx.fbx");
    ta.objLadrillo = new Model("models/city/Prod/objLadrillo.fbx");
    ta.grafitis = new Model("models/city/Prod/objMurofbx.fbx");
    ta.bandera = new Model("models/city/Prod/bandera.fbx");

    ta.luzSemaforo = new Model("models/city/Prod/luzSemaforo.fbx");
    ta.co2 = new Model("models/city/Prod/CO2.fbx");

    //Modelos IA

    ta.fabrica = new Model("models/city/Prod/fabrica.fbx");
    ta.rascacielos = new Model("models/city/Prod/rascacielos.fbx");
    ta.edificio2 = new Model("models/city/Prod/edificioIA2.fbx");
    ta.tienda = new Model("models/city/Prod/tienda.fbx");

    ta.hopstial = new Model("models/city/Prod/hospital.fbx");
    ta.banco = new Model("models/city/Prod/banco.fbx");
    ta.policia = new Model("models/city/Prod/policia.fbx");











    //ta.character01 = new AnimatedModel("models/IllumModels/KAYA.fbx");

}

void initilizeMaterialsTest(TestAssets& ta) {
    // --- ACERO (Metálico y brillante) ---
    ta.steel.ambient = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
    ta.steel.diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    ta.steel.specular = glm::vec4(0.77f, 0.77f, 0.77f, 1.0f);
    ta.steel.transparency = 1.0f; // ¡Siempre inicializa esto!

    // --- ASFALTO - Carretera - Mate

    ta.asphalt.ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    ta.asphalt.diffuse = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
    ta.asphalt.specular = glm::vec4(0.02f, 0.02f, 0.02f, 1.0f); // Casi sin brillo
    ta.asphalt.transparency = 1.0f;

    //Plastico
    ta.plastic.ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);;
    ta.plastic.diffuse = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
    ta.plastic.specular = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f); // Casi sin brillo
    ta.plastic.transparency = 1.0f;

    //Traslucido
    ta.traslucido.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);;
    ta.traslucido.diffuse = glm::vec4(0.588f, 0.588f, 0.588f, 1.0f);
    ta.traslucido.specular = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f); // Casi sin brillo
    ta.traslucido.transparency = 0.3f;

    //Traslucido - Focos
    ta.focos.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);;
    ta.focos.diffuse = glm::vec4(0.588f, 0.588f, 0.588f, 1.0f);
    ta.focos.specular = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f); // Casi sin brillo
    ta.focos.transparency = 0.5f;


    //Traslucido - Gases
    ta.gases.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);;
    ta.gases.diffuse = glm::vec4(0.588f, 0.588f, 0.588f, 1.0f);
    ta.gases.specular = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f); // Casi sin brillo
    ta.gases.transparency = 0.8f;

    //Madera - Mate
    ta.madera.ambient = glm::vec4(0.3f, 0.3f, 0.2f, 1.0f);
    ta.madera.diffuse = glm::vec4(0.55f, 0.55f, 0.5f, 1.0f);
    ta.madera.specular = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    ta.madera.transparency = 1.0f;

    //Cesped - Mate
    ta.cesped.ambient = glm::vec4(0.05f, 0.2f, 0.05f, 1.0f);
    ta.cesped.diffuse = glm::vec4(0.1f, 0.6f, 0.15f, 1.0f);
    ta.cesped.specular = glm::vec4(0.02f, 0.02f, 0.02f, 1.0f);
    ta.cesped.transparency = 1.0f;

    //Concreto -Mate
    ta.concreto.ambient = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
    ta.concreto.diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    ta.concreto.specular = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    ta.concreto.transparency = 1.0f;

    //Gome -mate
    ta.goma.ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    ta.goma.diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    ta.goma.specular = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    ta.goma.transparency = 1.0f;

    //Ladrillo
    ta.ladrillo.ambient = glm::vec4(0.3f, 0.15f, 0.1f, 1.0f);
    ta.ladrillo.diffuse = glm::vec4(0.7f, 0.35f, 0.25f, 1.0f);
    ta.ladrillo.specular = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    ta.ladrillo.transparency = 1.0f;

    //Tela -Mate
    ta.tela.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    ta.tela.diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    ta.tela.specular = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    ta.tela.transparency = 1.0f;
}

void initilizeLightsTest(TestAssets& ta) {
    ta.light01.Position = glm::vec3(1.4895f, 1.8836f, -1.1042f);
    ta.light01.Color = glm::vec4(0.2f, 0.2f, 0.2f, 0.5f);
    ta.light01.Power = glm::vec4(60.0f, 60.0f, 60.0f, 1.0f); // Potencia en Watts
    ta.light01.alphaIndex = 50;
    ta.light01.distance = 2.0f;

    gLights.push_back(ta.light01);
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

    fa.sphereDay = new Model("models/esfera_cielo.fbx");
    fa.sphereNight = new Model("models/esfera_noche.fbx");

    // Validación simple
    if (!fa.sphereDay) std::cout << "ERROR: No se cargo esfera_cielo.fbx" << std::endl;
    if (!fa.sphereNight) std::cout << "ERROR: No se cargo esfera_noche.fbx" << std::endl;


    fa.character01 = new AnimatedModel("models/LoboAnimation.fbx"); // <-- MODELO DEL LOBO CORRIENDO
    fa.character02 = new AnimatedModel("models/LoboAnimation.fbx"); // <-- MODELO DEL LOBO IDLE
    fa.character03 = new AnimatedModel("models/castorW.fbx"); // <-- MODELO DEL CASTOR CORRIENDO
    fa.character04 = new AnimatedModel("models/castorI.fbx"); // <-- MODELO DEL CASTOR IDLE
    fa.character05 = new AnimatedModel("models/OsoNW.fbx"); // <-- MODELO DEL OSO NEGRO CORRIENDO
    fa.character06 = new AnimatedModel("models/OsoNW.fbx"); // <-- MODELO DEL OSO NEGRO IDLE (sentado)
    fa.character07 = new AnimatedModel("models/AlceAnimation.fbx"); // <-- MODELO DEL ALCE CORRIENDO
    fa.character08 = new AnimatedModel("models/AlceAnimation.fbx"); // <-- MODELO DEL ALCE IDLE

    fa.skull_model = new Model("models/Craneo.fbx"); // <-- NUEVO

    // Ver si se cargaron los modelos (Se va?)
    if (!fa.chopped_once_model) {
        std::cerr << "ERROR: Could not load troncomuerto.fbx" << std::endl;
    }
    if (!fa.burning_tree_model) {
        std::cerr << "ERROR: Could not load troncoquemado.fbx" << std::endl;
    }
    if (!fa.chopped_twice_model) {
        std::cerr << "ERROR: Could not load basecortada.fbx" << std::endl;
    }
    if (!fa.skull_model) { // <-- NUEVO
        std::cerr << "ERROR: Could not load Craneo.fbx" << std::endl;
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

/* -------------------------------------------- - Manejo de Recursos Glaciar------------------------------------------*/
void initializeModelsEspacio(EspacioAssets& ea) {
    ea.Tierra = new Model("models/iceland/Tierra.fbx");
    ea.Canada = new Model("models/iceland/Canada.fbx");
    ea.Luna = new Model("models/IllumModels/moon.fbx");
    ea.cubeenv = new Model("models/mycube.fbx");
    ea.cubeenv_noche = new Model("models/noche/mycube.fbx");

    // CORREGIDO: Usar -> en punteros
    if (!fa.cubeenv || fa.cubeenv->meshes.empty() || fa.cubeenv->meshes[0].textures.empty()) {
        std::cout << "ERROR: Skybox DIA" << std::endl;
    }
    if (!fa.cubeenv_noche || fa.cubeenv_noche->meshes.empty() || fa.cubeenv_noche->meshes[0].textures.empty()) {
        std::cout << "ERROR: Skybox NIGHT" << std::endl;
    }
}

void initilizeMaterialsEspacio(EspacioAssets& ea) {
    ea.defaultMaterial.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    ea.defaultMaterial.specular = glm::vec4(1.0f, 0.01f, 0.01f, 1.0f);
    ea.defaultMaterial.diffuse = glm::vec4(1.0f, 1.0f, 0.01f, 1.0f);
    ea.defaultMaterial.transparency = 1.0f;
}

void initilizeLightsEspacio(EspacioAssets& ea) {
    ea.theLight.Position = glm::vec3(-20.0f, 0.0f, 3.0f);
    ea.theLight.Color = glm::vec4(1.0, 0.97, 0.92, 1.0);
    ea.theLight.Power = glm::vec4(60.0f, 60.0f, 60.0f, 1.0f);
    ea.theLight.alphaIndex = 120;
    ea.theLight.distance = 5.0f;
}

void loadEspacio(EspacioAssets& ea) {
    initilizeLightsEspacio(ea);
    initilizeMaterialsEspacio(ea);
    initializeModelsEspacio(ea);
}

void initializeModelsMenu(MenuAssets& ma) {
    ma.Texto1 = new Model("models/iceland/Texto1.fbx");
    ma.Texto2 = new Model("models/iceland/Texto2.fbx");
    ma.Texto3 = new Model("models/iceland/Texto3.fbx");
    ma.Texto4 = new Model("models/iceland/Texto4.fbx");
    ma.Texto5 = new Model("models/iceland/Texto5.fbx");
    ma.Texto6 = new Model("models/iceland/Texto6.fbx");
    ma.Fondo_Menu = new Model("models/iceland/Fondo_Menu.fbx");
    ma.HojaArce_Menu = new Model("models/iceland/HojaArce_Menu.fbx");
}

void loadMenu(MenuAssets& ma) {
    initializeModelsMenu(ma);
}

void initializeModelsGlaciar(GlaciarAssets& ga) {
    ga.Agua = new Model("models/iceland/Agua.fbx");
    ga.Iceberg = new Model("models/iceland/Iceberg.fbx");
    ga.TrozoH1 = new Model("models/iceland/TrozoH1.fbx");
    ga.TrozoH2 = new Model("models/iceland/TrozoH2.fbx");
    ga.Glaciares = new Model("models/iceland/Glaciares.fbx");
    ga.PlacaHielo = new Model("models/iceland/PlacaHielo.fbx");
    ga.BarraAzul = new Model("models/iceland/BarraAzul.fbx");
    ga.BarraRoja = new Model("models/iceland/BarraRoja.fbx");
    ga.TerAzulBase = new Model("models/iceland/TerAzulBase.fbx");
    ga.TerRojoBase = new Model("models/iceland/TerRojoBase.fbx");

    ga.Oso1 = new Animated("models/iceland/Oso1.fbx");
    ga.Oso2 = new Animated("models/iceland/Oso2.fbx");
    ga.Oso3 = new Animated("models/iceland/Oso3.fbx");
    ga.Oso4 = new Animated("models/iceland/Oso4.fbx");
    ga.Oso5 = new Animated("models/iceland/Oso5.fbx");

    // CORREGIDO: Usar -> en punteros
    if (!fa.cubeenv || fa.cubeenv->meshes.empty() || fa.cubeenv->meshes[0].textures.empty()) {
        std::cout << "ERROR: Skybox DIA" << std::endl;
    }
    if (!fa.cubeenv_noche || fa.cubeenv_noche->meshes.empty() || fa.cubeenv_noche->meshes[0].textures.empty()) {
        std::cout << "ERROR: Skybox NIGHT" << std::endl;
    }
}

void initilizeMaterialsGlaciar(GlaciarAssets& ga) {
    ga.defaultMaterial.ambient = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    ga.defaultMaterial.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ga.defaultMaterial.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ga.defaultMaterial.transparency = 1.0f;

    ga.nieveMaterial.ambient = glm::vec4(0.2f, 0.2f, 0.25f, 1.0f);
    ga.nieveMaterial.diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    ga.nieveMaterial.specular = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);


    ga.nieveMaterial.transparency = 0.9f;

    ga.GlaciaresMaterial.ambient = glm::vec4(0.18f, 0.15f, 0.18f, 0.8f);
    ga.GlaciaresMaterial.diffuse = glm::vec4(0.3f, 0.28f, 0.3f, 1.0f);
    ga.GlaciaresMaterial.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ga.GlaciaresMaterial.transparency = 1.0f;
}

void initilizeLightsGlaciar(GlaciarAssets& ga) {
    ga.theLight.Position = glm::vec3(0.0f, 20.0f, 0.0f);
    ga.theLight.Color = glm::vec4(1.0, 1.0, 1.0, 1.0);
    ga.theLight.Power = glm::vec4(60.0f, 60.0f, 60.0f, 1.0f);
    ga.theLight.alphaIndex = 120;
    ga.theLight.distance = 10.0f;
}

void loadGlaciar(GlaciarAssets& ga) {
    initilizeLightsGlaciar(ga);
    initilizeMaterialsGlaciar(ga);
    initializeModelsGlaciar(ga);
}
