/*
*
* ProyectoFinal 0.0.1-alpha

Funcionalidades: (Con Chunks, In<stancias, Montaña, Luz de Phong, Sol/Luna Esféricos, Ciclo Día/Noche, Nubes, Hojas y Talar/Plantar Árboles + Incendio Secuencial)

Ultimas Implementaciones: (Incendio Secuencial + Scope Global + Estilo Súper Estricto)
*/

/* ---------------------------------------- Encabezados del Proyecto -------------------------------------------------*/

// Archivo de Encabezado con las Estrucuras y Bibliotecas del Proyecto
#include <globals.h>

// Declaraciones de funciones (Se va a mover)
bool Start();
bool Update();
void init_values();

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


// --> Variables Globales Para la [Carga de Modelos]
Model* terrain_model = nullptr;
Model* grass_model = nullptr;
Model* rock_model = nullptr;
Model* mountain_model = nullptr;
Model* tree_model = nullptr;
Model* chopped_once_model = nullptr;
Model* burning_tree_model = nullptr;
Model* chopped_twice_model = nullptr;
Model* cubeenv = nullptr;
Model* cubeenv_noche = nullptr;
Model* sun_model = nullptr;
Model* moon_model = nullptr;
Model* cloud_model = nullptr;
Model* leaf_model = nullptr;

// --> Variables Globales Para la [Iluminacion y Materiales]
Light theLight;
Material defaultMaterial;
Material mountainMaterial;
Material treeMaterial;
Material cloudMaterial;
Material leafMaterial;
Material sunMaterial;

// --> Variables Globales Para el estado del mundo [Iluminacion y Materiales]
std::vector<Chunk> terrain_chunks;
std::vector<Leaf> falling_leaves;
std::vector<glm::mat4> cloud_matrices;
std::vector<glm::vec3> cloud_positions;
std::vector<glm::mat4> leaf_matrices;
int next_tree_id = 0;
bool isDay = true;

// --> Variable Globale Para el Tamano del Mundo
const int WORLD_SIZE = 10;

// --> Variable Globale Para lo que la camara puede ver [Pendiente por separar]
Frustum cameraFrustum;

// --> Variables GLOBALES VAO (Vertex Array Object) Y VBO ((Vertex Buffer Object))
unsigned int grassInstanceVBO = 0;
unsigned int rockInstanceVBO = 0;
unsigned int treeInstanceVBO = 0;
unsigned int choppedOnceTreeInstanceVBO = 0;
unsigned int burningTreeInstanceVBO = 0;
unsigned int choppedTwiceTreeInstanceVBO = 0;
unsigned int cloudInstanceVBO = 0;
unsigned int leafInstanceVBO = 0;
unsigned int crosshairVAO = 0, crosshairVBO = 0;

// --> Variables Globales Para la Interfaz Gráfica
unsigned int uiVAO = 0, uiVBO = 0; // Buffers UI

// Shader Interfaz Grafica (UI)
Shader* uiShader = nullptr;

// Texturas ID UI: Iconos
unsigned int fireTextureID = 0;
unsigned int treeTextureID = 0;
unsigned int highlightTextureID = 0;
// Texturas ID UI: Leyendas Texto
unsigned int legendFireTextureID = 0;
unsigned int legendTreeTextureID = 0;

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
bool isFireActive = false;
float fireStartTime = 0.0f;

// -> Variable Global Para el Manejo del [Audio]
ISoundEngine* SoundEngine = createIrrKlangDevice();

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
    if (crosshairVAO != 0) { glDeleteVertexArrays(1, &crosshairVAO); crosshairVAO = 0; }
    if (crosshairVBO != 0) { glDeleteBuffers(1, &crosshairVBO); crosshairVBO = 0; }
    if (uiVAO != 0) { glDeleteVertexArrays(1, &uiVAO); uiVAO = 0; }
    if (uiVBO != 0) { glDeleteBuffers(1, &uiVBO); uiVBO = 0; }
    if (grassInstanceVBO != 0) { glDeleteBuffers(1, &grassInstanceVBO); }
    if (rockInstanceVBO != 0) { glDeleteBuffers(1, &rockInstanceVBO); }
    if (treeInstanceVBO != 0) { glDeleteBuffers(1, &treeInstanceVBO); }
    if (choppedOnceTreeInstanceVBO != 0) { glDeleteBuffers(1, &choppedOnceTreeInstanceVBO); }
    if (burningTreeInstanceVBO != 0) { glDeleteBuffers(1, &burningTreeInstanceVBO); }
    if (choppedTwiceTreeInstanceVBO != 0) { glDeleteBuffers(1, &choppedTwiceTreeInstanceVBO); }
    if (cloudInstanceVBO != 0) { glDeleteBuffers(1, &cloudInstanceVBO); }
    if (leafInstanceVBO != 0) { glDeleteBuffers(1, &leafInstanceVBO); }

    if (fireTextureID != 0) { glDeleteTextures(1, &fireTextureID); }
    if (treeTextureID != 0) { glDeleteTextures(1, &treeTextureID); }
    if (highlightTextureID != 0) { glDeleteTextures(1, &highlightTextureID); }

    // --- NUEVO: Limpieza de leyendas ---
    if (legendFireTextureID != 0) { glDeleteTextures(1, &legendFireTextureID); }
    if (legendTreeTextureID != 0) { glDeleteTextures(1, &legendTreeTextureID); }
    // -----------------------------------

    delete phongShader;
    delete instancePhongShader;
    delete instanceAlphaTestPhongShader;
    delete skyboxShader;
    delete sunShader;
    delete crosshairShader;
    delete uiShader;
    delete terrain_model;
    delete grass_model;
    delete rock_model;
    delete mountain_model;
    delete tree_model;
    delete chopped_once_model;
    delete burning_tree_model;
    delete chopped_twice_model;
    delete cubeenv;
    delete cubeenv_noche;
    delete sun_model;
    delete moon_model;
    delete cloud_model;
    delete leaf_model;

    glfwTerminate();
    return 0;
}

void init_values() {
    theLight.Power = glm::vec4(1.5f, 1.5f, 1.5f, 1.0f);
    theLight.Color = glm::vec4(1.0f, 1.0f, 0.95f, 1.0f);
    theLight.alphaIndex = 32;
    defaultMaterial.ambient = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    defaultMaterial.specular = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    defaultMaterial.transparency = 1.0f;
    mountainMaterial.ambient = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    mountainMaterial.specular = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    mountainMaterial.transparency = 1.0f;
    treeMaterial.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    treeMaterial.specular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    treeMaterial.transparency = 1.0f;
    cloudMaterial.ambient = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    cloudMaterial.specular = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    cloudMaterial.transparency = 1.0f;
    leafMaterial.ambient = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    leafMaterial.specular = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    leafMaterial.transparency = 1.0f;
    sunMaterial.ambient = glm::vec4(1.0f, 1.0f, 0.8f, 1.0f);
    sunMaterial.specular = glm::vec4(0.0f);
    sunMaterial.transparency = 1.0f;
}

bool Start() {
    glfwInit();
    init_values();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mundo Instanciado", NULL, NULL);
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
    //phongShader = new Shader("shaders/11_PhongShaderMultLights.vs", "shaders/11_PhongShaderMultLights.fs");
    instancePhongShader = new Shader("shaders/instancing_phong.vs", "shaders/11_BasicPhongShader.fs");
    instanceAlphaTestPhongShader = new Shader("shaders/instancing_phong.vs", "shaders/instancing_phong_alpha_test.fs");
    skyboxShader = new Shader("shaders/10_vertex_simple.vs", "shaders/10_fragment_simple.fs");
    sunShader = new Shader("shaders/sun.vs", "shaders/sun.fs");
    crosshairShader = new Shader("shaders/crosshair.vs", "shaders/crosshair.fs");
    uiShader = new Shader("shaders/ui_shader.vs", "shaders/ui_shader.fs");

    if (!phongShader || phongShader->ID == 0 ||
        !instancePhongShader || instancePhongShader->ID == 0 ||
        !instanceAlphaTestPhongShader || instanceAlphaTestPhongShader->ID == 0 ||
        !skyboxShader || skyboxShader->ID == 0 ||
        !sunShader || sunShader->ID == 0 ||
        !crosshairShader || crosshairShader->ID == 0 ||
        !uiShader || uiShader->ID == 0)
    {
        std::cerr << "ERROR: Shaders failed to load." << std::endl;
        delete phongShader;
        delete instancePhongShader;
        delete instanceAlphaTestPhongShader;
        delete skyboxShader;
        delete sunShader;
        delete crosshairShader;
        delete uiShader;
        return false;
    }

    terrain_model = new Model("models/tierra_superficie.fbx");
    grass_model = new Model("models/pastoooyya.fbx");
    rock_model = new Model("models/piedrotanew.fbx");
    mountain_model = new Model("models/montana.fbx");
    tree_model = new Model("models/arce.fbx");
    chopped_once_model = new Model("models/troncomuerto.fbx");
    burning_tree_model = new Model("models/troncoquemado.fbx");
    chopped_twice_model = new Model("models/basecortada.fbx");
    sun_model = new Model("models/sphere.fbx");
    moon_model = new Model("models/moon.fbx");
    cloud_model = new Model("models/cloud.fbx");
    leaf_model = new Model("models/hojaarce.fbx");
    cubeenv = new Model("models/mycube.fbx");
    cubeenv_noche = new Model("models/noche/mycube.fbx");

    if (!chopped_once_model) {
        std::cerr << "ERROR: Could not load troncomuerto.fbx" << std::endl;
    }
    if (!burning_tree_model) {
        std::cerr << "ERROR: Could not load troncoquemado.fbx" << std::endl;
    }
    if (!chopped_twice_model) {
        std::cerr << "ERROR: Could not load basecortada.fbx" << std::endl;
    }
    // CORREGIDO: Usar -> en punteros
    if (!cubeenv || cubeenv->meshes.empty() || cubeenv->meshes[0].textures.empty()) {
        std::cout << "ERROR: Skybox DIA" << std::endl;
    }
    if (!cubeenv_noche || cubeenv_noche->meshes.empty() || cubeenv_noche->meshes[0].textures.empty()) {
        std::cout << "ERROR: Skybox NIGHT" << std::endl;
    }

    // --- MODIFICADO: CARGAR LEYENDAS ---
    // --- Cargar Texturas UI ---
    fireTextureID = TextureFromFile("fire.png", "models/image");
    treeTextureID = TextureFromFile("tree.png", "models/image");
    highlightTextureID = TextureFromFile("highlight.png", "models/image");

    // --- NUEVO: Cargar texturas de leyendas ---
    legendFireTextureID = TextureFromFile("legend_fire.png", "models/image");
    legendTreeTextureID = TextureFromFile("legend_tree.png", "models/image");
    // -------------------------------------------

    if (fireTextureID == 0) {
        std::cerr << "ERROR: Failed to load 'models/image/fire.png'" << std::endl;
    }
    if (treeTextureID == 0) {
        std::cerr << "ERROR: Failed to load 'models/image/tree.png'" << std::endl;
    }
    if (highlightTextureID == 0) {
        std::cerr << "ERROR: Failed to load 'models/image/highlight.png'" << std::endl;
    }

    // --- NUEVO: Comprobaciones de error para leyendas ---
    if (legendFireTextureID == 0) {
        std::cerr << "ERROR: Failed to load 'models/image/legend_fire.png'" << std::endl;
    }
    if (legendTreeTextureID == 0) {
        std::cerr << "ERROR: Failed to load 'models/image/legend_tree.png'" << std::endl;
    }
    // ----------------------------------------------------

    // --- Configuración Cruz ---
    float crosshairSize = 0.03f;
    float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    float crosshairVertices[] = {
        -crosshairSize / aspectRatio, 0.0f,
         crosshairSize / aspectRatio, 0.0f,
         0.0f, -crosshairSize,
         0.0f,  crosshairSize
    };
    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);
    glBindVertexArray(crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // --- Configuración UI VAO/VBO ---
    float uiQuadVertices[] = {
        // pos      // tex
        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f,
        0.0f, 0.0f,  0.0f, 0.0f,

        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 1.0f,  1.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f
    };
    glGenVertexArrays(1, &uiVAO);
    glGenBuffers(1, &uiVBO);
    glBindVertexArray(uiVAO);
    glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uiQuadVertices), &uiQuadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    std::cout << "Using Manual Tree Trunk AABB Min: (" << tree_trunk_aabb_min.x << ", " << tree_trunk_aabb_min.y << ", " << tree_trunk_aabb_min.z << ")" << std::endl;
    std::cout << "Using Manual Tree Trunk AABB Max: (" << tree_trunk_aabb_max.x << ", " << tree_trunk_aabb_max.y << ", " << tree_trunk_aabb_max.z << ")" << std::endl;

    //generateForest();

    // Configurar VBOs Instancias
    size_t max_initial_trees = WORLD_SIZE * WORLD_SIZE * TREES_PER_CHUNK;
    size_t max_plantable_trees = 200;
    size_t max_total_trees = max_initial_trees + max_plantable_trees;
    size_t initial_leaves_count = falling_leaves.size();
    size_t max_explosion_leaves = max_total_trees * EXPLOSION_LEAVES_PER_HIT;
    size_t max_total_leaves = initial_leaves_count + max_explosion_leaves;

    setupInstanceVBO(grassInstanceVBO, WORLD_SIZE * WORLD_SIZE * GRASS_PER_CHUNK, grass_model);
    setupInstanceVBO(rockInstanceVBO, WORLD_SIZE * WORLD_SIZE * ROCKS_PER_CHUNK, rock_model);
    setupInstanceVBO(treeInstanceVBO, max_total_trees, tree_model);
    if (chopped_once_model != nullptr) {
        setupInstanceVBO(choppedOnceTreeInstanceVBO, max_total_trees, chopped_once_model);
    }
    if (burning_tree_model != nullptr) {
        setupInstanceVBO(burningTreeInstanceVBO, max_total_trees, burning_tree_model);
    }
    if (chopped_twice_model != nullptr) {
        setupInstanceVBO(choppedTwiceTreeInstanceVBO, max_total_trees, chopped_twice_model);
    }
    setupInstanceVBO(cloudInstanceVBO, TOTAL_CLOUDS, cloud_model);

    std::cout << "Initial leaves: " << initial_leaves_count << ", Max total leaves (VBO): " << max_total_leaves << std::endl;
    leaf_matrices.reserve(max_total_leaves);
    setupInstanceVBO(leafInstanceVBO, max_total_leaves, leaf_model);

    return true;
}

bool Update() {
    //Delta Time
    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Procesar Entradas por Hardware
    processInput(window);

    //Actualizar la Logica de Mundo
    updateGameLogic();

    // Dibujar Escena 3D
    renderScene();

    // Dibujar UI 2D 
    renderUI();

    glfwSwapBuffers(window);
    glfwPollEvents();
    return true;
}
