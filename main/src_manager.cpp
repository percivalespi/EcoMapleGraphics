#include "src_manager.h"
#include "globals.h"

/* -------------------------------------------- - Manejo de Recursos TEST------------------------------------------*/
void initializeModelsTest(TestAssets& ta) {

    ta.respawn = false;

    ta.suelo = new Model("models/city/Prod/suelo.fbx");
    ta.suelo_verde = new Model("models/city/Prod/sueloVerde.fbx");
    ta.metales = new Model("models/city/Prod/metales.fbx");
    ta.objMadera = new Model("models/city/Prod/elementosMadera.fbx");
    //ta.objCristales = new Model("models/city/Prod/objCristales3.fbx");
    ta.objPlasticos = new Model("models/city/Prod/objPlasticos3.fbx");
    ta.objConcreto = new Model("models/city/Prod/objConcreto.fbx");
    ta.objLlantas = new Model("models/city/Prod/objLlantafbx.fbx");
    ta.objLadrillo = new Model("models/city/Prod/objLadrillo.fbx");
    ta.grafitis = new Model("models/city/Prod/objMurofbx.fbx");
    ta.bandera = new Model("models/city/Prod/bandera.fbx");

    ta.luzSemaforo = new Model("models/city/Prod/luzSemaforo.fbx");
    ta.co2 = new Model("models/city/Prod/CO2.fbx");
    ta.arboles = new Model("models/city/Prod/arbolesv2.fbx");

    //BASURAS
    ta.basura1 = new Model("models/city/Prod/basura_norte_azul.fbx");
    ta.basura2 = new Model("models/city/Prod/basura_norte_cafe.fbx");
    ta.basura3 = new Model("models/city/Prod/basura_norte_rojo.fbx");
    ta.basura4 = new Model("models/city/Prod/basura_sur_blanca.fbx"); 
    ta.basura5 = new Model("models/city/Prod/basura_sur_ladrillo.fbx");
    ta.basura6 = new Model("models/city/Prod/basura_sur_rojo.fbx");

    //BOTES
    ta.bote1 = new Model("models/city/Prod/bote_norte_azul.fbx");
    ta.bote2 = new Model("models/city/Prod/bote_norte_cafe.fbx");
    ta.bote3 = new Model("models/city/Prod/bote_norte_gris.fbx");
    ta.bote4 = new Model("models/city/Prod/bote_norte_rojo.fbx");
    ta.bote5 = new Model("models/city/Prod/bote_sur_blanca.fbx");
    ta.bote6 = new Model("models/city/Prod/bote_sur_gris.fbx");
    ta.bote7 = new Model("models/city/Prod/bote_sur_ladrillo.fbx");
    ta.bote8 = new Model("models/city/Prod/bote_sur_rojo.fbx");

    //CONTENEDORES
    ta.contenedor1 = new Model("models/city/Prod/contenedor_norte_azul.fbx");
    ta.contenedor2 = new Model("models/city/Prod/contenedor_norte_cafe.fbx");
    ta.contenedor3 = new Model("models/city/Prod/contenedor_norte_gris.fbx");
    ta.contenedor4 = new Model("models/city/Prod/contenedor_norte_rojo.fbx");
    ta.contenedor5 = new Model("models/city/Prod/contenedor_sur_blanco.fbx");
    ta.contenedor6 = new Model("models/city/Prod/contenedor_sur_gris.fbx");
    ta.contenedor7 = new Model("models/city/Prod/contenedor_sur_ladrillo.fbx");
    ta.contenedor8 = new Model("models/city/Prod/contenedor_sur_rojo.fbx");

    //MUEBLES
    ta.mueble1 = new Model("models/city/Prod/mueble_norte_cafe_1.fbx");
    ta.mueble2 = new Model("models/city/Prod/mueble_norte_cafe_2.fbx");
    ta.mueble3 = new Model("models/city/Prod/mueble_norte_rojo_1.fbx");
    ta.mueble4 = new Model("models/city/Prod/mueble_norte_rojo_2.fbx");
    ta.mueble5 = new Model("models/city/Prod/mueble_norte_rojo_3.fbx");
    ta.mueble6 = new Model("models/city/Prod/mueble_norte_rojo_4.fbx");
    ta.mueble7 = new Model("models/city/Prod/mueble_sur_rojo_1.fbx");
    ta.mueble8 = new Model("models/city/Prod/mueble_sur_rojo_2.fbx");
    ta.mueble9 = new Model("models/city/Prod/mueble_sur_rojo_3.fbx");
    ta.mueble10 = new Model("models/city/Prod/mueble_sur_rojo_4.fbx");

    //Modelos IA

    ta.fabrica = new Model("models/city/Prod/fabrica.fbx");
    ta.rascacielos = new Model("models/city/Prod/rascacielos.fbx");
    ta.edificio2 = new Model("models/city/Prod/edificioIA2.fbx");
    ta.tienda = new Model("models/city/Prod/tienda.fbx");

    ta.hopstial = new Model("models/city/Prod/hospital.fbx");
    ta.banco = new Model("models/city/Prod/banco.fbx");
    ta.policia = new Model("models/city/Prod/policia.fbx");
    ta.camion = new Model("models/city/Prod/Camionv2.fbx");

	ta.persona1 = new Animated("models/city/Prod/persona1.fbx");
    ta.persona2 = new Animated("models/city/Prod/persona2.fbx");
    ta.persona3 = new Animated("models/city/Prod/persona3.fbx");
}

/*                                         PROPIEDADES DE LOS MATERIALES CIUDAD                                    */
void initilizeMaterialsTest(TestAssets& ta) {
    // --- ACERO (Metálico y brillante) ---
    ta.steel.ambient = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
    ta.steel.diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    ta.steel.specular = glm::vec4(0.77f, 0.77f, 0.77f, 1.0f);
    ta.steel.transparency = 1.0f; // ¡Siempre inicializa esto!

    // --- ASFALTO - Carretera - Mate

    ta.asphalt.ambient = glm::vec4(0.4f, 0.40f, 0.40f, 1.0f);
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

/*                                         PROPIEDADES DE LUZ GLOBAL PARA LA CIUDAD                                   */
void initilizeLightsTest(TestAssets& ta) {
    ta.light01.Position = glm::vec3(100.4895f, 1.8836f, -1.1042f);
    ta.light01.Color = glm::vec4(0.2f, 0.2f, 0.2f, 0.5f);
    ta.light01.Power = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // Potencia en Watts
    ta.light01.alphaIndex = 50;
    ta.light01.distance = 1.3f;

    gLights.push_back(ta.light01);
}

/*                                       CARGA DE LUCES, MODELOS Y MATERIALES PARA CIUDAD                           */
void loadTest(TestAssets& ta) {
    initilizeLightsTest(ta);
    initilizeLightsForest(fa);
    initilizeMaterialsTest(ta);
    initializeModelsTest(ta);
}

// Función auxiliar para crear una textura de 1 píxel de color sólido
unsigned int CreateSolidTexture(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    unsigned char data[] = { r, g, b, a }; // Un solo píxel RGBA

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Configuración para que se estire sin borrosidad
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return textureID;
}

/* -------------------------------------------- - Manejo de Recursos UI------------------------------------------*/
void loadUI(UIAssets& ui) {
    // --- MODIFICADO: CARGAR LEYENDAS ---
    // --- Cargar Texturas UI ---

    ui.tex_solid_green = CreateSolidTexture(50, 205, 50, 255);

    // Gris oscuro semitransparente para el fondo (R=50, G=50, B=50, A=150)
    ui.tex_solid_gray = CreateSolidTexture(50, 50, 50, 150);

    // Rojo para peligro (opcional)
    ui.tex_solid_red = CreateSolidTexture(220, 20, 60, 255);

    ui.fireTextureID = TextureFromFile("fire.png", "models/image");
    ui.treeTextureID = TextureFromFile("tree.png", "models/image");
    ui.highlightTextureID = TextureFromFile("highlight.png", "models/image");

    // --- NUEVO: Cargar texturas de leyendas ---
    ui.legendFireTextureID = TextureFromFile("legend_fire.png", "models/image");
    ui.legendTreeTextureID = TextureFromFile("legend_tree.png", "models/image");
    // -------------------------------------------
    ui.tex_intro_wasd = TextureFromFile("image1wasd.png", "models/image");
    ui.tex_intro_mouse = TextureFromFile("image2mouse.png", "models/image");
    ui.tex_intro_descender = TextureFromFile("image3_descender.png", "models/image");
    ui.tex_intro_volar = TextureFromFile("image3_volar.png", "models/image");
    ui.tex_intro_acercamiento = TextureFromFile("image3acercamiento.png", "models/image");


    // --- CARGA TUTORIAL BOSQUE ---
    ui.tex_f_plantar = TextureFromFile("image1_plantar.png", "models/image");
    ui.tex_f_talar = TextureFromFile("image2_talar.png", "models/image");
    ui.tex_f_camara3 = TextureFromFile("image3_camara3persona.png", "models/image");
    ui.tex_f_incendio = TextureFromFile("image4_incendio.png", "models/image");

    // --- CARGA CONDICIONALES ---
    ui.tex_c_cambio_pj = TextureFromFile("image3_cambiopersonaje.png", "models/image");
    ui.tex_c_parar_fuego = TextureFromFile("imagen5_pararincendio.png", "models/image");
    ui.tex_c_polos_derretidos = TextureFromFile("image7_polosdescongelados.png", "models/image");
    ui.tex_c_alerta_temp = TextureFromFile("imagen7_subidatemperatura.png", "models/image");

    if (ui.tex_f_plantar == 0) std::cout << "ERROR: Falta image1_plantar.png" << std::endl;
	if (ui.tex_f_talar == 0) std::cout << "ERROR: Falta image2_talar.png" << std::endl;
	if (ui.tex_f_camara3 == 0) std::cout << "ERROR: Falta image3_camara3persona.png" << std::endl;
	if (ui.tex_f_incendio == 0) std::cout << "ERROR: Falta image4_incendio.png" << std::endl;
	if (ui.tex_c_cambio_pj == 0) std::cout << "ERROR: Falta image3_cambiopersonaje.png" << std::endl;
	if (ui.tex_c_parar_fuego == 0) std::cout << "ERROR: Falta imagen5_pararincendio.png" << std::endl;

	if (ui.tex_c_polos_derretidos == 0) std::cout << "ERROR: Falta image7_polosdescongelados.png" << std::endl;
	if (ui.tex_c_alerta_temp == 0) std::cout << "ERROR: Falta imagen7_subidatemperatura.png" << std::endl;

    // --- CARGA DE AVISO BLOQUEADO ---
    ui.tex_bloqueado = TextureFromFile("image4_bloqueados.png", "models/image");

    // (Opcional) Comprobación de errores rápida
    if (ui.tex_intro_wasd == 0) std::cout << "ERROR: Falta image1wasd.png" << std::endl;
	if (ui.tex_intro_mouse == 0) std::cout << "ERROR: Falta image2mouse.png" << std::endl;
	if (ui.tex_intro_descender == 0) std::cout << "ERROR: Falta image3_descender.png" << std::endl;
	if (ui.tex_intro_volar == 0) std::cout << "ERROR: Falta image3_volar.png" << std::endl;
	if (ui.tex_intro_acercamiento == 0) std::cout << "ERROR: Falta image3acercamiento.png" << std::endl;

    ui.tex_glaciar_bienvenida = TextureFromFile("image1_bienvenidapolos.png", "models/image");
    ui.tex_glaciar_bosque = TextureFromFile("image2_inidicacionbosque.png", "models/image");
    ui.tex_glaciar_ciclo = TextureFromFile("image3_ciclodianoche.png", "models/image");



    if (ui.tex_glaciar_bienvenida == 0) std::cout << "ERROR: Falta image1_bienvenidapolos.png" << std::endl;
	if (ui.tex_glaciar_bosque == 0) std::cout << "ERROR: Falta image2_inidicacionbosque.png" << std::endl;
    if (ui.tex_glaciar_ciclo == 0) std::cout << "ERROR: Falta image3_ciclodianoche.png" << std::endl;

    ui.warningFireID = TextureFromFile("fuegoincendio.png", "models/image");
    if (ui.warningFireID == 0) {
        std::cerr << "ERROR: No se cargo models/image/fuegoincendio.png" << std::endl;
    }
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


/* -------------------------------------------- - Manejo de Recursos Bosque CARGA DE MODELOS------------------------------------------*/
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
    ea.cubeenv_noche = new Model("models/noche_tierra.fbx");
    ea.sol = new Model("models/iceland/Sol.fbx");

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

    ga.Oso1 = new Animated("models/iceland/OsoPS.fbx");
    ga.Oso2 = new Animated("models/iceland/OsoPW.fbx");

}

void initilizeMaterialsGlaciar(GlaciarAssets& ga) {
    ga.defaultMaterial.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    ga.defaultMaterial.diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    ga.defaultMaterial.specular = glm::vec4(0.010f, 0.010f, 0.010f, 1.0f);
    ga.defaultMaterial.transparency = 1.0f;

    ga.nieveMaterial.ambient = glm::vec4(0.19f, 0.2f, 0.21f, 1.0f);
    ga.nieveMaterial.diffuse = glm::vec4(0.48f, 0.5f, 0.51f, 1.0f);
    ga.nieveMaterial.specular = glm::vec4(0.10f, 0.10f, 0.10f, 1.0f);
    ga.nieveMaterial.transparency = 1.0f;

    ga.GlaciaresMaterial.ambient = glm::vec4(0.18f, 0.15f, 0.18f, 1.0f);
    ga.GlaciaresMaterial.diffuse = glm::vec4(0.5f, 0.48f, 0.5f, 1.0f);
    ga.GlaciaresMaterial.specular = glm::vec4(0.010f, 0.010f, 0.010f, 1.0f);
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

// Crea un cubemap desde 6 rutas (orden GL: POSITIVE_X, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y, POSITIVE_Z, NEGATIVE_Z)
static unsigned int LoadCubemap(const std::vector<std::string>& faces)
{
    unsigned int texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    int w, h, ch;
    stbi_set_flip_vertically_on_load(false);
    for (unsigned int i = 0; i < 6; i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &ch, 0);
        if (data) {
            GLenum fmt = (ch == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cerr << "ERROR: Failed to load cubemap face: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return texID;
}

// Carga recursos del cristal: cubemap + modelo
void loadFresnelGlassResources()
{
    if (g_envCubemapTexID == 0) {
        std::vector<std::string> faces = {
            "models/mycube.fbm/posx.jpg",
            "models/mycube.fbm/negx.jpg",
            "models/mycube.fbm/posy.jpg",
            "models/mycube.fbm/negy.jpg",
            "models/mycube.fbm/posz.jpg",
            "models/mycube.fbm/negz.jpg"
        };
        g_envCubemapTexID = LoadCubemap(faces);
        if (g_envCubemapTexID == 0) {
            std::cerr << "ERROR: Could not create environment cubemap." << std::endl;
        }
    }

    if (g_glassModel == nullptr) {
        // Coloca tu FBX/OBJ en models/GlassBuilding.fbx
        g_glassModel = new Model("models/city/Prod/objCristales3.fbx");
        if (!g_glassModel) {
            std::cerr << "ERROR: Could not load models/GlassBuilding.fbx" << std::endl;
        }
    }
}

int addEnvironmentCubemap(const std::vector<std::string>& faces) {
    unsigned int id = LoadCubemap(faces);
    if (id != 0) {
        g_envCubemaps.push_back(id);
        return int(g_envCubemaps.size()) - 1;
    }
    return -1;
}

void loadAllEnvironmentCubemaps() {
    g_envCubemaps.clear();

    // Ejemplo: “Día”
    addEnvironmentCubemap({
            "models/mycube.fbm/posx.jpg",
            "models/mycube.fbm/negx.jpg",
            "models/mycube.fbm/posy.jpg",
            "models/mycube.fbm/negy.jpg",
            "models/mycube.fbm/posz.jpg",
            "models/mycube.fbm/negz.jpg"
        });

    // Ejemplo: “Atardecer”
    addEnvironmentCubemap({
        "models/noche/mycube.fbm/posx.jpg",
        "models/noche/mycube.fbm/negx.jpg",
        "models/noche/mycube.fbm/posy.jpg",
        "models/noche/mycube.fbm/negy.jpg",
        "models/noche/mycube.fbm/posz.jpg",
        "models/noche/mycube.fbm/negz.jpg"
        });

    g_envIndex = 0; // empieza con el primero
}
