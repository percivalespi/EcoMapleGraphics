/*
*
* ProyectoFinal 0.0.1-alpha

Funcionalidades: (Con Chunks, In<stancias, Montaña, Luz de Phong, Sol/Luna Esféricos, Ciclo Día/Noche, Nubes, Hojas y Talar/Plantar Árboles + Incendio Secuencial)

Ultimas Implementaciones: (Incendio Secuencial + Scope Global + Estilo Súper Estricto)
*/

/*Si lo aprendiste bien dale like y sucribete*/

/* ---------------------------------------- Encabezados del Proyecto -------------------------------------------------*/

// Archivo de Encabezado con las Estrucuras y Bibliotecas del Proyecto
#include "globals.h"
#include "input.h"
#include "mechanic.h"
#include "render.h"
#include "src_manager.h"

// Bandera Para activar entorno de prueba
bool g_runTestEnvironment = false; // <-- CAMBIADO A FALSE PARA VER EL BOSQUE

// Firmas de Funciones - Estructura Básica OpenGL
bool Start();
bool Update();

void processInput2(GLFWwindow* window);

/* ------------------------------------------ Variables Globales ------------------------------------------------------ */

// --> Variables Globales Para la <Ventana> 
GLFWwindow* window;
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

Camera camera(glm::vec3(0.0f, 5.0f, 25.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float sunAngle = 0.0f;
float sunElevationAngle = 45.0f;

// --> Variables Globales Para Definir las [Reglas del Mundo]
const float CHUNK_SIZE = 20.0f;
const float fireDuration = 80.0f; // 1 Minuto
const float minBurnDuration = 15.0f; // [Mínimo que un árbol arde]
const float maxBurnDuration = 40.0f; // Máximo que un árbol arde]
const float minCloudDistanceSq = 60.0f * 60.0f;
const int maxPlacementTries = 20;

// --> Variables Globales Para la Generacion Aleatoria del Bosque

// Generando Semilla Aleatora
std::random_device rd;
std::mt19937 gen(rd());

// Generación de elemntos aleatorios dentro del chunk [BOSQUE]
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
std::uniform_real_distribution<float> dis_fire_time(0.0f, fireDuration - maxBurnDuration); // Tiempo de inicio de fuego
std::uniform_real_distribution<float> dis_burn_duration(minBurnDuration, maxBurnDuration); // Duración de quema

// Generación de elemntos aleatorios dentro del chunk [NUBES]
std::uniform_real_distribution<float> dis_cloud_distant_x(-800.0f, 800.0f);
std::uniform_real_distribution<float> dis_cloud_distant_z(-800.0f, 800.0f);
std::uniform_real_distribution<float> dis_cloud_y(70.0f, 100.0f);
std::uniform_real_distribution<float> dis_cloud_scale(1.0f, 2.5f);

// --> Variables Globales Para el uso de [SHADERS]

// Shaders de Phong
Shader* phongShader = nullptr;
Shader* instancePhongShader = nullptr;
Shader* instanceAlphaTestPhongShader = nullptr;
// Shaders personalizados
Shader* skyboxShader = nullptr;
Shader* sunShader = nullptr;
Shader* crosshairShader = nullptr;
Shader* mLightsShader = nullptr;
// Shader Interfaz Grafica (UI)
Shader* uiShader = nullptr;
// Shader de Skinning
Shader* dynamicShader = nullptr;

Shader* phongShader2 = nullptr;
Shader* moonShader;
Shader* basicShader;
Shader* wavesShader;
Shader* wavesShader2;


// --> Variables Globales Para el estado del mundo [Iluminacion y Materiales]
std::vector<Chunk> terrain_chunks;
std::vector<Leaf> falling_leaves;
std::vector<glm::mat4> cloud_matrices;
std::vector<glm::vec3> cloud_positions;
std::vector<glm::mat4> leaf_matrices;
int next_tree_id = 0;
bool isDay = true;

// --- NUEVO: Definición de globales de IA y Mundo ---
float WORLD_MIN_X, WORLD_MAX_X, WORLD_MIN_Z, WORLD_MAX_Z;
std::vector<AnimalInstance> g_animals;
std::uniform_real_distribution<float> dis_ai_time(3.0f, 10.0f); // 3-10 segundos por estado
std::uniform_real_distribution<float> dis_ai_target_dist(5.0f, 15.0f); // Caminar 5-15 metros
std::uniform_real_distribution<float> dis_spawn_chance(0.0f, 1.0f); // Rango de 0.0 a 1.0

// --- NUEVO: Definición de Globales de Simulación ---
float g_forestHealth = 1.0f;        // Empezar al 100%
float g_previousForestHealth = 1.0f; // <-- AÑADE ESTA LÍNEA (inicializar igual)
int g_totalInitialTrees = 0;
int g_currentLivingTrees = 0;
int g_maxAnimalsInWorld = 0;
float g_animalDeathTimer = ANIMAL_DEATH_RATE;
float g_animalRespawnTimer = ANIMAL_RESPAWN_RATE;
// --- FIN NUEVO ---

// --> Variable Globale Para el Tamano del Mundo
const int WORLD_SIZE = 10;

// --> Variable Globale Para lo que la camara puede ver
Frustum cameraFrustum;

// -> Variables Globales Mecanica (Talado de Árboles)
const unsigned int GRASS_PER_CHUNK = 80;
const unsigned int ROCKS_PER_CHUNK = 15;
const unsigned int TREES_PER_CHUNK = 5;
const unsigned int LEAVES_PER_TREE = 2;
const unsigned int NUM_DISTANT_CLOUDS = 40;
const unsigned int NUM_LOCAL_CLOUDS = 10;
const unsigned int TOTAL_CLOUDS = NUM_DISTANT_CLOUDS + NUM_LOCAL_CLOUDS;
const unsigned int EXPLOSION_LEAVES_PER_HIT = 20;

// Hitboxes Arboles
const glm::vec3 tree_trunk_aabb_min(-0.5f, 0.0f, -0.5f);
const glm::vec3 tree_trunk_aabb_max(0.5f, 2.0f, 5.5f);

// Banderas de Estado Mecácnica Fuego/Plantar 
const float max_plant_distance = 15.0f;
bool p_key_pressed = false;
bool f_key_pressed = false;
bool g_key_pressed = false;
bool z_key_pressed = false;
bool isFireActive = false;
float fireStartTime = 0.0f;

// Vector de Luces
std::vector<Light> gLights;

// Assets del Bosque
ForestAssets fa;
UIAssets ui;
TestAssets ta;
EspacioAssets ea;
MenuAssets ma;
GlaciarAssets ga;

// Shaders para materiales
Shader* lambertShader;







// -> Variable Global Para el Manejo del [Audio]
ISoundEngine* SoundEngine = createIrrKlangDevice();

//Variables tierra, glaciares
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

// === Variables globales para derretimiento en escena 1 ===
float glacierScaleY = 1.0f;          // escala vertical de Glaciares (1 → 0)
float meltSpeedBase = 0.01f;         // factor base de derretimiento por °C
float osoDropFactor = 2.0f;          // descenso de osos por unidad derretida

// === Movimiento aleatorio segmentado para Oso3, Oso4 y Oso5 ===
// Recorren tramos de 10 unidades y giran ±90° al azar al finalizar cada tramo.
const float OSO_SEGMENTO = 10.0f;
const float OSO_SPEED = 1.0f;            // unidades/segundo
glm::vec3 osoPos[3] = { {0,0,0}, {0,0,0}, {0,0,0} };

glm::vec3 posicionActual(0.0f, 2.0f, 10.0f);
glm::vec3 posicionA1(-0.6f, 6.0f, 5.0f);
glm::vec3 posicionCarga(0.0f, 47.0f, 4.0f);
glm::vec3 posicionInicioG(0.0f, 2.0f, 10.0f);
glm::vec3 posicionOrigen(0.0f, 0.0f, 0.0f);
glm::vec3 posicionEscenario1(0.0f, 40.0f, 340.0f);

glm::vec3 osoUltimoGiro[3] = { {0,0,0}, {0,0,0}, {0,0,0} };
float      osoYawDeg[3] = { 0.0f, 0.0f, 0.0f };  // 0° mirando +X

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

    // Limpieza
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

    // --- NUEVO: Limpieza de leyendas ---
    if (ui.legendFireTextureID != 0) { glDeleteTextures(1, &ui.legendFireTextureID); }
    if (ui.legendTreeTextureID != 0) { glDeleteTextures(1, &ui.legendTreeTextureID); }
    // -----------------------------------

    delete phongShader;
    delete instancePhongShader;
    delete instanceAlphaTestPhongShader;
    delete skyboxShader;
    delete sunShader;
    delete crosshairShader;
    delete uiShader;
    delete dynamicShader; // <-- NUEVO

    delete fa.character01; // <-- NUEVO
    delete fa.character02; // <__ NUEVO MODELO DE CASTOR
    delete fa.skull_model; // <-- NUEVO

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

    glfwTerminate();
    return 0;
}



bool Start() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "EcoMapleGrahics  v1.0.0-alpha", NULL, NULL);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    glEnable(GL_DEPTH_TEST);

    // ----------------------------- DEFINICIÓN DE SHADERS ----------------------------------------------------

    //NOTA: EL BOSQUE AL TENER UNA SOLA FUENTE DE ILUMINACIÓN (SOL) SE USA LE BASICO
    phongShader = new Shader("shaders/11_BasicPhongShader.vs", "shaders/11_BasicPhongShader.fs");
    instancePhongShader = new Shader("shaders/instancing_phong.vs", "shaders/11_BasicPhongShader.fs");
    instanceAlphaTestPhongShader = new Shader("shaders/instancing_phong.vs", "shaders/instancing_phong_alpha_test.fs");
    skyboxShader = new Shader("shaders/10_vertex_simple.vs", "shaders/10_fragment_simple.fs");
    sunShader = new Shader("shaders/sun.vs", "shaders/sun.fs");
    crosshairShader = new Shader("shaders/crosshair.vs", "shaders/crosshair.fs");
    uiShader = new Shader("shaders/ui_shader.vs", "shaders/ui_shader.fs");

    //Shaders de Tierra y Glaciares
    moonShader = new Shader("shaders/16_moonAnimation.vs", "shaders/16_moonAnimation.fs");
    basicShader = new Shader("shaders/10_vertex_simple.vs", "shaders/10_fragment_simple.fs");
    wavesShader = new Shader("shaders/13_wavesAnimation.vs", "shaders/13_wavesAnimation.fs");
    wavesShader2 = new Shader("shaders/13_wavesAnimation2.vs", "shaders/13_wavesAnimation2.fs");

    //Shader de Luces Multiples de Phong
    phongShader2 = new Shader("shaders/11_BasicPhongShader2.vs", "shaders/11_BasicPhongShader2.fs");
    mLightsShader = new Shader("shaders/11_PhongShaderMultLights.vs", "shaders/11_PhongShaderMultLights.fs");

    // --- NUEVO: Cargar Shader de Skinning ---
    dynamicShader = new Shader("shaders/10_vertex_skinning-IT.vs", "shaders/10_fragment_skinning-IT.fs");
    //dynamicShader2 = new Shader("shaders/10_vertex_skinning-IT2.vs", "shaders/10_fragment_skinning-IT2.fs");


    if (!phongShader || phongShader->ID == 0 ||
        !instancePhongShader || instancePhongShader->ID == 0 ||
        !instanceAlphaTestPhongShader || instanceAlphaTestPhongShader->ID == 0 ||
        !skyboxShader || skyboxShader->ID == 0 ||
        !sunShader || sunShader->ID == 0 ||
        !crosshairShader || crosshairShader->ID == 0 ||
        !uiShader || uiShader->ID == 0 ||
        !mLightsShader || mLightsShader->ID == 0 ||
        !dynamicShader || dynamicShader->ID == 0 // <-- NUEVA COMPROBACIÓN
        )
    {
        std::cerr << "ERROR: Shaders failed to load." << std::endl;
        delete phongShader;
        delete instancePhongShader;
        delete instanceAlphaTestPhongShader;
        delete skyboxShader;
        delete sunShader;
        delete crosshairShader;
        delete uiShader;
        delete mLightsShader;
        delete dynamicShader; // <-- NUEVO DELETE
        return false;
    }

    //ta.light02.Position = glm::vec3(camera.Position);
    //ta.light02.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //ta.light02.Power = glm::vec4(10.0f, 10.0f, 10.0f, 1.0f); // Potencia en Watts
    //ta.light02.alphaIndex = 16;
    //gLights.push_back(ta.light02);

    // Carga de Modelos y Recursos
    loadUI(ui);
    loadTest(ta);
    loadForest(fa);

    loadMenu(ma);
    loadEspacio(ea);
    loadGlaciar(ga);
    // Inicialiacion de los Buffer de Renderizado
    initializeRenderBuffers(ui);

    // Depuración (Se debe de Borrar)
    std::cout << "Using Manual Tree Trunk AABB Min: (" << tree_trunk_aabb_min.x << ", " << tree_trunk_aabb_min.y << ", " << tree_trunk_aabb_min.z << ")" << std::endl;
    std::cout << "Using Manual Tree Trunk AABB Max: (" << tree_trunk_aabb_max.x << ", " << tree_trunk_aabb_max.y << ", " << tree_trunk_aabb_max.z << ")" << std::endl;

    // --- NUEVO: Calcular límites del mundo ---
    // !!! MOVIDO AQUÍ ARRIBA !!!
    float world_half_width = (WORLD_SIZE / 2.0f) * CHUNK_SIZE;
    WORLD_MIN_X = -world_half_width;
    WORLD_MAX_X = world_half_width;
    WORLD_MIN_Z = -world_half_width;
    WORLD_MAX_Z = world_half_width;
    // --- FIN NUEVO ---

    generateForest();

    // --- NUEVO: Contar árboles iniciales y animales ---
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
    std::cout << "Mundo generado con " << g_totalInitialTrees << " arboles vivos." << std::endl;
    std::cout << "Maximo de animales en el mundo: " << g_maxAnimalsInWorld << std::endl;
    // --- FIN NUEVO ---

    initializeInstanceBuffers(fa);

    return true;
}

bool Update() {
    //Delta Time
    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (!animacion1 && escena == 0)CalculoCamara(window);
    //else if (!menu && animacion1)Animacion1(window);
    else if (!menu && animacion1)UpdateAnim1(window);
    else if (menu)Transicion(window);

    //Cálculo del desplazamiento de la barra del termómetro
    barraTF = (temperatura + 25.0) * 0.04088f;
    barraTC = temperatura * 0.0236f;

    // Procesa la entrada del teclado o mouse
    processInput(window);

    //Actualizar la Logica de Mundo
    updateGameLogic();

    // Dibujar Escena 3D
    renderScene();

    // Dibujar UI 2D 
    if(escena==1 && !menu)renderUI();
    glfwSwapBuffers(window);
    glfwPollEvents();
      

    
    return true;
}
