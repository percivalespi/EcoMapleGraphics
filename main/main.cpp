/*
* ProyectoFinal 0.0.1-alpha - Main.cpp
* Integración: Cámara 3ra Persona + Cambio de Personaje (Demi/Miku)
*/

#include "globals.h"
#include "input.h"
#include "mechanic.h"
#include "render.h"
#include "src_manager.h"
#include "audio.h"

// Bandera Para activar entorno de prueba
bool g_runTestEnvironment = false;

// Firmas de Funciones
bool Start();
bool Update();
void processInput2(GLFWwindow* window);

/* ------------------------------------------ Variables Globales ------------------------------------------------------ */
int rrr=0;
int bandera = 0;

// --> Ventana
GLFWwindow* window;
unsigned int SCR_WIDTH = 1024;
unsigned int SCR_HEIGHT = 768;

bool is_fullscreen = false;
int last_window_width = SCR_WIDTH;
int last_window_height = SCR_HEIGHT;
int last_window_x = 0;
int last_window_y = 0;

Camera camera(glm::vec3(0.0f, 5.0f, 25.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool g_isWindowFocused = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float sunAngle = 0.0f;
float sunElevationAngle = 45.0f;

// --> Reglas del Mundo
const float CHUNK_SIZE = 20.0f;
const float fireDuration = 80.0f;
const float minBurnDuration = 15.0f;
const float maxBurnDuration = 40.0f;
const float minCloudDistanceSq = 60.0f * 60.0f;
const int maxPlacementTries = 20;

// --> Generación Aleatoria
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis_pos(-CHUNK_SIZE / 2.0f, CHUNK_SIZE / 2.0f);
std::uniform_real_distribution<float> dis_scale_grass(0.8f, 1.2f);
std::uniform_real_distribution<float> dis_scale_rock(0.2f, 0.7f);
std::uniform_real_distribution<float> dis_scale_tree(1.0f, 1.5f);
std::uniform_real_distribution<float> dis_rot(0.0f, 360.0f);
std::uniform_real_distribution<float> dis_axis(0.0f, 1.0f);
std::uniform_real_distribution<float> dis_leaf_offset(-2.0f, 2.0f);
std::uniform_real_distribution<float> dis_leaf_height(8.0f, 15.0f);
std::uniform_real_distribution<float> dis_initial_fall(1.0f, 2.5f);
std::uniform_real_distribution<float> dis_initial_spin(20.0f, 70.0f);
std::uniform_real_distribution<float> dis_explode_y(1.0f, 5.0f);
std::uniform_real_distribution<float> dis_explode_angle(0.0f, 360.0f);
std::uniform_real_distribution<float> dis_explode_radius(0.0f, 1.5f);
std::uniform_real_distribution<float> dis_explode_fall(1.5f, 3.0f);
std::uniform_real_distribution<float> dis_explode_spin(40.0f, 90.0f);
std::uniform_real_distribution<float> dis_fire_time(0.0f, fireDuration - maxBurnDuration);
std::uniform_real_distribution<float> dis_burn_duration(minBurnDuration, maxBurnDuration);
std::uniform_real_distribution<float> dis_cloud_distant_x(-800.0f, 800.0f);
std::uniform_real_distribution<float> dis_cloud_distant_z(-800.0f, 800.0f);
std::uniform_real_distribution<float> dis_cloud_y(70.0f, 100.0f);
std::uniform_real_distribution<float> dis_cloud_scale(1.0f, 2.5f);

// --> Shaders
Shader* phongShader = nullptr;
Shader* instancePhongShader = nullptr;
Shader* instanceAlphaTestPhongShader = nullptr;
Shader* skyboxShader = nullptr;
Shader* sunShader = nullptr;
Shader* crosshairShader = nullptr;
Shader* mLightsShader = nullptr;
Shader* uiShader = nullptr;
Shader* dynamicShader = nullptr;
Shader* phongShader2 = nullptr;
Shader* fresnelShader = nullptr;
Shader* moonShader;
Shader* basicShader;
Shader* wavesShader;
Shader* wavesShader2;
Shader* lambertShader;
Shader* lineShader = nullptr;
Shader* singleTreeShader;
Shader* smoke;

float timeSmoke = 0.0f;

//Variables para Fresnel
unsigned int g_envCubemapTexID = 0;
Model* g_glassModel = nullptr;

// --> Estado del Mundo
std::vector<Chunk> terrain_chunks;
std::vector<Leaf> falling_leaves;
std::vector<glm::mat4> cloud_matrices;
std::vector<glm::vec3> cloud_positions;
std::vector<glm::mat4> leaf_matrices;
int next_tree_id = 0;
bool isDay = true;

// --> IA y Mundo
float WORLD_MIN_X, WORLD_MAX_X, WORLD_MIN_Z, WORLD_MAX_Z;
std::vector<AnimalInstance> g_animals;
std::uniform_real_distribution<float> dis_ai_time(3.0f, 10.0f);
std::uniform_real_distribution<float> dis_ai_target_dist(5.0f, 15.0f);
std::uniform_real_distribution<float> dis_spawn_chance(0.0f, 1.0f);

// --> Simulación Salud
float g_forestHealth = 1.0f;
float g_previousForestHealth = 1.0f;
int g_totalInitialTrees = 0;
int g_currentLivingTrees = 0;
int g_maxAnimalsInWorld = 0;
float g_animalDeathTimer = ANIMAL_DEATH_RATE;
float g_animalRespawnTimer = ANIMAL_RESPAWN_RATE;

// --> Tamaño Mundo
const int WORLD_SIZE = 10;

// --> Frustum
Frustum cameraFrustum;

// --> Constantes Mecánicas
const unsigned int GRASS_PER_CHUNK = 80;
const unsigned int ROCKS_PER_CHUNK = 15;
const unsigned int TREES_PER_CHUNK = 5;
const unsigned int LEAVES_PER_TREE = 2;
const unsigned int NUM_DISTANT_CLOUDS = 40;
const unsigned int NUM_LOCAL_CLOUDS = 10;
const unsigned int TOTAL_CLOUDS = NUM_DISTANT_CLOUDS + NUM_LOCAL_CLOUDS;
const unsigned int EXPLOSION_LEAVES_PER_HIT = 20;

const glm::vec3 tree_trunk_aabb_min(-0.5f, 0.0f, -0.5f);
const glm::vec3 tree_trunk_aabb_max(0.5f, 2.0f, 5.5f);

const float max_plant_distance = 15.0f;
bool p_key_pressed = false;
bool f_key_pressed = false;
bool g_key_pressed = false;
bool z_key_pressed = false;
bool plus_key_pressed = false;
bool minus_key_pressed = false;
bool isFireActive = false;
bool r_key_pressed = false;
float fireStartTime = 0.0f;

// Vector de Luces
std::vector<Light> gLights;

// Assets
ForestAssets fa;
UIAssets ui;
TestAssets ta;
EspacioAssets ea;
MenuAssets ma;
GlaciarAssets ga;

// --> Audio
ISoundEngine* SoundEngine = createIrrKlangDevice();

// --> Variables Tierra/Glaciar
float Time = 0.0f;
float TimeA1 = 0.0f;
float transparenciaC = 0.0f;
float wavesTime = 0.0f;
float avanceA1 = 0.0005f;
float DT1 = 0.0f;
float DT2 = 0.0f;
float DT3 = 0.0f;
float DT4 = 0.0f;
float DT5 = 0.0f;
float DT6 = 0.0f;
float DHoja = 0.0f;
const float velocidadCarga = 0.01f;
float temperatura = -25.0f;
float barraTF = 0.0f;
float barraTC = 0.0f;
int escena = 0;
bool menu = false;
bool animacion1 = false;
bool calor = false;
CamAni g_anim1;
MenuAnim g_menu;

// --> Glaciar variables
float glacierScaleY = 1.0f;
float meltSpeedBase = 0.01f;
float osoDropFactor = 2.0f;
const float OSO_SEGMENTO = 10.0f;
const float OSO_SPEED = 1.0f;
glm::vec3 osoPos[3] = { {0,0,0}, {0,0,0}, {0,0,0} };
glm::vec3 osoUltimoGiro[3] = { {0,0,0}, {0,0,0}, {0,0,0} };
float      osoYawDeg[3] = { 0.0f, 0.0f, 0.0f };

glm::vec3 posicionActual(0.0f, 2.0f, 10.0f);
glm::vec3 posicionA1(-0.6f, 6.0f, 5.0f);
glm::vec3 posicionCarga(0.0f, 47.0f, 4.0f);
glm::vec3 posicionInicioG(0.0f, 2.0f, 10.0f);
glm::vec3 posicionOrigen(0.0f, 0.0f, 0.0f);
glm::vec3 posicionEscenario1(0.0f, 40.0f, 340.0f);

// =============================================================================================
// === VARIABLES GLOBALES PARA PERSONAJE Y CÁMARA 3RA PERSONA ===
// =============================================================================================

// Estado
bool g_isThirdPerson = false;
bool g_pressM = false;

// Variables de los Modelos
Animated* g_demiModel = nullptr;
Animated* g_mikuModel = nullptr;
//Model* camionBasura = nullptr;

// Estado del Jugador
int g_activeCharacter = 1; // 1=Demi, 2=Miku
glm::vec3 g_demiPos(0.0f, 0.0f, 0.0f); // Posición compartida
float g_demiRotY = 0.0f;
bool g_demiMoving = false;

// Constantes (Definición)
const float DEMI_SPEED = 10.0f;
const float DEMI_SCALE = 0.04f;
const float MIKU_SCALE = 0.04f;
const float DEMI_CAM_DIST = 15.0f;
const float DEMI_OFFSET_Y = 0.0f;



// =============================================================================================

int main() {
    if (!Start()) {
        glfwTerminate();
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        if (!Update()) {
            break;
        }
    }



    // Limpieza existente
    if (ui.crosshairVAO != 0) { glDeleteVertexArrays(1, &ui.crosshairVAO); ui.crosshairVAO = 0; }
    if (ui.crosshairVBO != 0) { glDeleteBuffers(1, &ui.crosshairVBO); ui.crosshairVBO = 0; }
    if (ui.uiVAO != 0) { glDeleteVertexArrays(1, &ui.uiVAO); ui.uiVAO = 0; }
    if (ui.uiVBO != 0) { glDeleteBuffers(1, &ui.uiVBO); ui.uiVBO = 0; }

    if (fa.grassInstanceVBO != 0) { glDeleteBuffers(1, &fa.grassInstanceVBO); }
    if (fa.rockInstanceVBO != 0) { glDeleteBuffers(1, &fa.rockInstanceVBO); }
    if (fa.treeInstanceVBO != 0) { glDeleteBuffers(1, &fa.treeInstanceVBO); }
    if (fa.choppedOnceTreeInstanceVBO != 0) { glDeleteBuffers(1, &fa.choppedOnceTreeInstanceVBO); }
    if (fa.burningTreeInstanceVBO != 0) { glDeleteBuffers(1, &fa.burningTreeInstanceVBO); }
    if (fa.choppedTwiceTreeInstanceVBO != 0) { glDeleteBuffers(1, &fa.choppedTwiceTreeInstanceVBO); }
    if (fa.cloudInstanceVBO != 0) { glDeleteBuffers(1, &fa.cloudInstanceVBO); }
    if (fa.leafInstanceVBO != 0) { glDeleteBuffers(1, &fa.leafInstanceVBO); }

    if (ui.fireTextureID != 0) { glDeleteTextures(1, &ui.fireTextureID); }
    if (ui.treeTextureID != 0) { glDeleteTextures(1, &ui.treeTextureID); }
    if (ui.highlightTextureID != 0) { glDeleteTextures(1, &ui.highlightTextureID); }
    if (ui.legendFireTextureID != 0) { glDeleteTextures(1, &ui.legendFireTextureID); }
    if (ui.legendTreeTextureID != 0) { glDeleteTextures(1, &ui.legendTreeTextureID); }

    delete phongShader;
    delete instancePhongShader;
    delete instanceAlphaTestPhongShader;
    delete skyboxShader;
    delete sunShader;
    delete crosshairShader;
    delete uiShader;
    delete dynamicShader;

    delete fa.character01;
    delete fa.character02;
    delete fa.skull_model;

    delete fa.terrain_model;
    delete fa.grass_model;
    delete fa.rock_model;
    delete fa.mountain_model;
    delete fa.tree_model;
    delete fa.chopped_once_model;
    delete fa.burning_tree_model;
    delete fa.chopped_twice_model;
    delete fa.cubeenv;
    delete fa.cubeenv_noche;
    delete fa.sun_model;
    delete fa.moon_model;
    delete fa.cloud_model;
    delete fa.leaf_model;

    delete fa.sphereDay;
    delete fa.sphereNight;

    // Limpiar Personajes
    if (g_demiModel) delete g_demiModel;
    if (g_mikuModel) delete g_mikuModel;

    glfwTerminate();
    return 0;
}

bool Start() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "EcoMapleGrahics v1.0", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetWindowFocusCallback(window, window_focus_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    glEnable(GL_DEPTH_TEST);

    // Shaders
    phongShader = new Shader("shaders/11_BasicPhongShader.vs", "shaders/11_BasicPhongShader.fs");
    instancePhongShader = new Shader("shaders/instancing_phong.vs", "shaders/11_BasicPhongShader.fs");
    instanceAlphaTestPhongShader = new Shader("shaders/instancing_phong.vs", "shaders/instancing_phong_alpha_test.fs");
    skyboxShader = new Shader("shaders/10_vertex_simple.vs", "shaders/10_fragment_simple.fs");
    sunShader = new Shader("shaders/sun.vs", "shaders/sun.fs");
    crosshairShader = new Shader("shaders/crosshair.vs", "shaders/crosshair.fs");
    uiShader = new Shader("shaders/ui_shader.vs", "shaders/ui_shader.fs");
    moonShader = new Shader("shaders/16_moonAnimation.vs", "shaders/16_moonAnimation.fs");
    basicShader = new Shader("shaders/10_vertex_simple.vs", "shaders/10_fragment_simple.fs");
    fresnelShader = new Shader("shaders/17_Fresnel.vs", "shaders/17_Fresnel.fs");
    wavesShader = new Shader("shaders/13_wavesAnimation.vs", "shaders/13_wavesAnimation.fs");
    wavesShader2 = new Shader("shaders/13_wavesAnimation2.vs", "shaders/13_wavesAnimation2.fs");
    phongShader2 = new Shader("shaders/11_BasicPhongShader2.vs", "shaders/11_BasicPhongShader2.fs");
    mLightsShader = new Shader("shaders/11_PhongShaderMultLights.vs", "shaders/11_PhongShaderMultLights.fs");
    dynamicShader = new Shader("shaders/10_vertex_skinning-IT.vs", "shaders/10_fragment_skinning-IT.fs");
    lineShader = new Shader("shaders/line.vs", "shaders/line.fs");
    singleTreeShader = new Shader("shaders/12_PhongShaderMultLights.vs", "shaders/12_PhongShaderMultLights.fs");
    smoke = new Shader("shaders/smoke.vs", "shaders/smoke.fs");



    if (!phongShader || phongShader->ID == 0 ||
        !instancePhongShader || instancePhongShader->ID == 0 ||
        !instanceAlphaTestPhongShader || instanceAlphaTestPhongShader->ID == 0 ||
        !skyboxShader || skyboxShader->ID == 0 ||
        !sunShader || sunShader->ID == 0 ||
        !crosshairShader || crosshairShader->ID == 0 ||
        !uiShader || uiShader->ID == 0 ||
        !mLightsShader || mLightsShader->ID == 0 ||
        !dynamicShader || dynamicShader->ID == 0)
    {
        std::cerr << "ERROR: Shaders failed to load." << std::endl;
        return false;
    }
    audio_init();
    loadUI(ui);
    loadTest(ta);
    loadForest(fa);
    loadMenu(ma);
    loadEspacio(ea);
    loadGlaciar(ga);
    loadFresnelGlassResources();

    initializeRenderBuffers(ui);

    float world_half_width = (WORLD_SIZE / 2.0f) * CHUNK_SIZE;
    WORLD_MIN_X = -world_half_width;
    WORLD_MAX_X = world_half_width;
    WORLD_MIN_Z = -world_half_width;
    WORLD_MAX_Z = world_half_width;

    generateForest();

    g_totalInitialTrees = 0;
    for (const auto& chunk : terrain_chunks) {
        for (const auto& tree : chunk.tree_instances) {
            if (tree.state == TreeState::ALIVE) {
                g_totalInitialTrees++;
            }
        }
    }
    g_currentLivingTrees = g_totalInitialTrees;
    g_maxAnimalsInWorld = (int)g_animals.size();

    initializeInstanceBuffers(fa);

    setupInstanceVBO(ta.cityTreesVBO, 1, ta.arboles);

    // =============================================================================================
    // === CARGA DE MODELOS JUGABLES ===
    // =============================================================================================

    // Cargar Demi
    g_demiModel = new Animated("models/demi.fbx");
    if (g_demiModel) {
        g_demiModel->currentAnimation = 0;
        g_demiModel->fps = 30.0f;
        g_demiPos = glm::vec3(0.0f, 0.0f, 0.0f); // Posición inicial
    }
    else {
        std::cout << "ERROR CRITICO: No se cargo models/demi.fbx" << std::endl;
    }

    // Cargar Miku
    g_mikuModel = new Animated("models/miku.fbx");
    if (g_mikuModel) {
        g_mikuModel->currentAnimation = 0;
        g_mikuModel->fps = 30.0f;
    }
    else {
        std::cout << "ERROR: No se cargo models/miku.fbx" << std::endl;
    }



    return true;
}

bool Update() {
    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (!animacion1 && escena == 0 && !g_isThirdPerson) {
        CalculoCamara(window);
    }
    else if (!menu && animacion1) UpdateAnim1(window);
    else if (menu) Transicion(window);

    // ... código anterior (Transicion, etc) ...

        // --- CORRECCIÓN: LIMITAR LA TEMPERATURA (CLAMPING) ---
        // Esto asegura que la temperatura se mantenga dentro de los límites visuales del termómetro.
        // Si baja de -40, se queda en -40. Si sube de 40, se queda en 40.
    float tempVisual = glm::clamp(temperatura, -40.0f, 40.0f);

    // Usamos 'tempVisual' para calcular el tamaño de las barras
    // (Nota: Si quieres que la mecánica del juego siga subiendo más allá de 40 
    // aunque no se vea, usa 'tempVisual' aquí. Si quieres limitar el juego también,
    // asigna el clamp a 'temperatura' directamente: temperatura = glm::clamp(...))

    // Opción recomendada: Limitar todo el juego para evitar errores lógicos:
    temperatura = glm::clamp(temperatura, -40.0f, 40.0f);

    barraTF = (temperatura + 25.0) * 0.04088f;
    barraTC = temperatura * 0.0236f;

    // Corrección extra: Evitar que las barras sean negativas si la fórmula falla
    if (barraTF < 0.0f) barraTF = 0.0f;
    if (barraTC < 0.0f) barraTC = 0.0f;
    // -----------------------------------------------------



    processInput(window);

    Animated* currentModel = nullptr;
    if (g_activeCharacter == 1) currentModel = g_demiModel;
    else if (g_activeCharacter == 2) currentModel = g_mikuModel;

    if (currentModel) {
        currentModel->currentAnimation = 0;

        if (g_demiMoving) {
            currentModel->UpdateAnimation(deltaTime);
        }
        else {
            currentModel->animationCount = 0;
            currentModel->elapsedTime = 0.0f;
            currentModel->UpdateAnimation(0.0f);
        }
    }

    updateGameLogic();
    updateAudioLogic(camera.Position);

    


    

    renderScene();

    if (escena == 1 && !menu) renderUI();


    glfwSwapBuffers(window);
    glfwPollEvents();


    return true;
}

// (processInput2 eliminado, no se usa)
void processInput2(GLFWwindow* window) {}