/*
*
* ProyectoFinal 0.0.1-alpha

Funcionalidades: (Con Chunks, In<stancias, Montaña, Luz de Phong, Sol/Luna Esféricos, Ciclo Día/Noche, Nubes, Hojas y Talar/Plantar Árboles + Incendio Secuencial)

Ultimas Implementaciones: (Incendio Secuencial + Scope Global + Estilo Súper Estricto)
*/

/* ---------------------------------------- Encabezados del Proyecto -------------------------------------------------*/

// Archivo de Encabezado con las Estrucuras y Bibliotecas del Proyecto
#include <globals.h>

#include <input.h>

// Declaraciones de funciones (Se va a mover)
bool Start();
bool Update();
void setupInstanceVBO(unsigned int& vbo, size_t max_items, Model* model_to_setup);
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

    const float TERRAIN_AABB_HEIGHT = 35.0f;
    // Las distribuciones ya están globales

    for (int chunk_idx_x = 0; chunk_idx_x < WORLD_SIZE; ++chunk_idx_x) {
        for (int chunk_idx_z = 0; chunk_idx_z < WORLD_SIZE; ++chunk_idx_z) {
            int x_coord = chunk_idx_x - WORLD_SIZE / 2;
            int z_coord = chunk_idx_z - WORLD_SIZE / 2;
            Chunk chunk;
            chunk.position = glm::vec3(x_coord * CHUNK_SIZE, 0.0f, z_coord * CHUNK_SIZE);
            float half_size = CHUNK_SIZE / 2.0f;
            chunk.aabb_min = glm::vec3(chunk.position.x - half_size, -2.0f, chunk.position.z - half_size);
            chunk.aabb_max = glm::vec3(chunk.position.x + half_size, TERRAIN_AABB_HEIGHT, chunk.position.z + half_size);
            std::vector<glm::vec3> large_object_positions;

            for (unsigned int i = 0; i < ROCKS_PER_CHUNK; i++) {
                glm::mat4 model = glm::mat4(1.0f);
                glm::vec3 pos = chunk.position + glm::vec3(dis_pos(gen), 0.5f, dis_pos(gen));
                large_object_positions.push_back(pos);
                model = glm::translate(model, pos);
                model = glm::rotate(model, glm::radians(dis_rot(gen)), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(dis_scale_rock(gen)));
                chunk.rock_matrices.push_back(model);
            }
            for (unsigned int i = 0; i < TREES_PER_CHUNK; i++) {
                glm::vec3 pos;
                bool position_ok;
                int tries = 0;
                do {
                    position_ok = true;
                    pos = chunk.position + glm::vec3(dis_pos(gen), 0.0f, dis_pos(gen));
                    for (const auto& occupied_pos : large_object_positions) {
                        if (glm::distance(pos, occupied_pos) < 6.0f) {
                            position_ok = false;
                            break;
                        }
                    }
                    tries++;
                } while (!position_ok && tries < 20);

                if (position_ok) {
                    large_object_positions.push_back(pos);
                    TreeInstance instance;
                    instance.id = next_tree_id++;
                    instance.state = TreeState::ALIVE;
                    instance.fireTriggerTime = -1.0f;
                    instance.burnOutTime = -1.0f;
                    float scale_factor = dis_scale_tree(gen);
                    instance.matrix = glm::mat4(1.0f);
                    instance.matrix = glm::translate(instance.matrix, pos);
                    instance.matrix = glm::rotate(instance.matrix, glm::radians(dis_rot(gen)), glm::vec3(0.0f, 1.0f, 0.0f));
                    instance.matrix = glm::rotate(instance.matrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    instance.matrix = glm::scale(instance.matrix, glm::vec3(scale_factor));
                    chunk.tree_instances.push_back(instance);

                    glm::vec3 treeBasePos = pos;
                    for (int j = 0; j < LEAVES_PER_TREE; j++) {
                        Leaf leaf;
                        float startHeight = dis_leaf_height(gen);
                        leaf.initialTreePos = treeBasePos + glm::vec3(dis_leaf_offset(gen), 0.0f, dis_leaf_offset(gen));
                        leaf.initialHeight = startHeight;
                        leaf.position = leaf.initialTreePos + glm::vec3(0.0f, startHeight, 0.0f);
                        leaf.rotationAxis = glm::normalize(glm::vec3(dis_axis(gen), dis_axis(gen), dis_axis(gen)));
                        if (glm::length(leaf.rotationAxis) < 0.01f) {
                            leaf.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
                        }
                        leaf.rotationAngle = dis_rot(gen);
                        leaf.fallSpeed = dis_initial_fall(gen);
                        leaf.spinSpeed = dis_initial_spin(gen);
                        leaf.parentTreeId = instance.id;
                        leaf.is_active = true;
                        leaf.is_explosion_leaf = false;
                        falling_leaves.push_back(leaf);
                    }
                }
            }
            for (unsigned int i = 0; i < GRASS_PER_CHUNK; i++) {
                glm::mat4 model = glm::mat4(1.0f);
                glm::vec3 pos = chunk.position + glm::vec3(dis_pos(gen), 0.5f, dis_pos(gen));
                model = glm::translate(model, pos);
                model = glm::rotate(model, glm::radians(dis_rot(gen)), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(dis_scale_grass(gen)));
                chunk.grass_matrices.push_back(model);
            }
            terrain_chunks.push_back(chunk);
        }
    }

    // Generación Nubes
    for (unsigned int i = 0; i < NUM_DISTANT_CLOUDS; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 pos;
        bool position_ok;
        int tries = 0;
        do {
            position_ok = true;
            pos = glm::vec3(dis_cloud_distant_x(gen), dis_cloud_y(gen), dis_cloud_distant_z(gen));
            for (const auto& existing_pos : cloud_positions) {
                if (glm::distance2(glm::vec2(pos.x, pos.z), glm::vec2(existing_pos.x, existing_pos.z)) < minCloudDistanceSq) {
                    position_ok = false;
                    break;
                }
            }
            tries++;
        } while (!position_ok && tries < maxPlacementTries);
        if (position_ok) {
            cloud_positions.push_back(pos);
            model = glm::translate(model, pos);
            model = glm::rotate(model, glm::radians(dis_rot(gen)), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(dis_cloud_scale(gen)));
            cloud_matrices.push_back(model);
        }
    }
    float world_half_width = (WORLD_SIZE / 2.0f) * CHUNK_SIZE;
    std::uniform_real_distribution<float> dis_cloud_local_x(-world_half_width, world_half_width);
    std::uniform_real_distribution<float> dis_cloud_local_z(-world_half_width, world_half_width);
    for (unsigned int i = 0; i < NUM_LOCAL_CLOUDS; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 pos;
        bool position_ok;
        int tries = 0;
        do {
            position_ok = true;
            pos = glm::vec3(dis_cloud_local_x(gen), dis_cloud_y(gen), dis_cloud_local_z(gen));
            for (const auto& existing_pos : cloud_positions) {
                if (glm::distance2(glm::vec2(pos.x, pos.z), glm::vec2(existing_pos.x, existing_pos.z)) < minCloudDistanceSq) {
                    position_ok = false;
                    break;
                }
            }
            tries++;
        } while (!position_ok && tries < maxPlacementTries);
        if (position_ok) {
            cloud_positions.push_back(pos);
            model = glm::translate(model, pos);
            model = glm::rotate(model, glm::radians(dis_rot(gen)), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(dis_cloud_scale(gen)));
            cloud_matrices.push_back(model);
        }
    }

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
    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    if (isFireActive) {
        float fire_elapsed = (float)glfwGetTime() - fireStartTime;
        if (fire_elapsed >= fireDuration) {
            isFireActive = false;
            std::cout << "El incendio ha terminado." << std::endl;
            for (Chunk& chunk : terrain_chunks) {
                for (TreeInstance& tree : chunk.tree_instances) {
                    if (tree.state == TreeState::BURNING) {
                        tree.state = TreeState::CHOPPED_TWICE;
                    }
                }
            }
        }
        else {
            for (Chunk& chunk : terrain_chunks) {
                for (TreeInstance& tree : chunk.tree_instances) {
                    // Transición 1: ALIVE/CHOPPED_ONCE -> BURNING
                    if (tree.fireTriggerTime >= 0.0f && fire_elapsed >= tree.fireTriggerTime) {
                        if (tree.state == TreeState::ALIVE || tree.state == TreeState::CHOPPED_ONCE) {
                            if (tree.state == TreeState::ALIVE) {
                                for (Leaf& leaf : falling_leaves) {
                                    if (leaf.parentTreeId == tree.id) {
                                        leaf.is_active = false;
                                    }
                                }
                            }
                            tree.state = TreeState::BURNING;
                            tree.fireTriggerTime = -1.0f;
                        }
                    }
                    // Transición 2: BURNING -> CHOPPED_TWICE
                    if (tree.burnOutTime >= 0.0f && fire_elapsed >= tree.burnOutTime) {
                        if (tree.state == TreeState::BURNING) {
                            tree.state = TreeState::CHOPPED_TWICE;
                            tree.burnOutTime = -1.0f;
                        }
                    }
                }
            }
        }
    }

    glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 2000.0f);
    glm::mat4 view = camera.GetViewMatrix();
    cameraFrustum.extractPlanes(projection * view);

    float lightDistance = 200.0f;
    float sunElevRad = glm::radians(sunElevationAngle);
    float sunAngleRad = glm::radians(sunAngle);
    theLight.Position = glm::vec3(lightDistance * cos(sunElevRad) * sin(sunAngleRad), lightDistance * sin(sunElevRad), lightDistance * cos(sunElevRad) * cos(sunAngleRad));
    float minDimFactor = 0.15f;
    float normalizedSunY = glm::clamp(theLight.Position.y / lightDistance, -1.0f, 1.0f);
    float dayNightTransition = glm::smoothstep(-0.1f, 0.1f, normalizedSunY);
    float nightDimFactor = glm::mix(minDimFactor, 1.0f, dayNightTransition);
    glm::vec4 dimmedLightPower = theLight.Power * nightDimFactor;
    isDay = (normalizedSunY > -0.1f);

    leaf_matrices.clear();
    for (size_t i = 0; i < falling_leaves.size(); ++i) {
        Leaf& leaf = falling_leaves[i];
        if (!leaf.is_active) {
            continue;
        }

        leaf.position.y -= leaf.fallSpeed * deltaTime;
        leaf.rotationAngle += leaf.spinSpeed * deltaTime;

        if (leaf.position.y < 0.0f) {
            if (leaf.is_explosion_leaf) {
                leaf.is_active = false;
                continue;
            }
            else {
                leaf.position.y = leaf.initialHeight;
                leaf.position.x = leaf.initialTreePos.x + dis_leaf_offset(gen);
                leaf.position.z = leaf.initialTreePos.z + dis_leaf_offset(gen);
            }
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, leaf.position);
        model = glm::rotate(model, glm::radians(leaf.rotationAngle), leaf.rotationAxis);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.009f));
        leaf_matrices.push_back(model);
    }

    // --- 1. DIBUJAR OBJETOS OPACOS ESTÁTICOS ---
    if (phongShader != nullptr && phongShader->ID != 0) {
        phongShader->use();
        phongShader->setMat4("projection", projection);
        phongShader->setMat4("view", view);
        phongShader->setVec3("lightPosition", theLight.Position);
        phongShader->setVec4("LightColor", theLight.Color);
        phongShader->setVec4("LightPower", dimmedLightPower);
        phongShader->setInt("alphaIndex", theLight.alphaIndex);

        if (terrain_model != nullptr) {
            phongShader->setBool("isTerrain", true);
            phongShader->setVec4("MaterialAmbientColor", defaultMaterial.ambient);
            phongShader->setVec4("MaterialSpecularColor", defaultMaterial.specular);
            phongShader->setFloat("transparency", defaultMaterial.transparency);
            for (const auto& chunk : terrain_chunks) {
                if (cameraFrustum.isBoxInFrustum(chunk.aabb_min, chunk.aabb_max)) {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, chunk.position);
                    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    phongShader->setMat4("model", model);
                    terrain_model->Draw(*phongShader);
                }
            }
        }
        if (mountain_model != nullptr) {
            phongShader->setBool("isTerrain", false);
            phongShader->setVec4("MaterialAmbientColor", mountainMaterial.ambient);
            phongShader->setVec4("MaterialSpecularColor", mountainMaterial.specular);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 11.75f, 170.0f));
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(18.45f, 18.45f, 8.865f));
            phongShader->setMat4("model", model);
            mountain_model->Draw(*phongShader);
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // --- RECOLECTAR MATRICES VISIBLES ---
    std::vector<glm::mat4> visible_grass;
    std::vector<glm::mat4> visible_rocks;
    std::vector<glm::mat4> visible_trees_alive;
    std::vector<glm::mat4> visible_trees_chopped_once;
    std::vector<glm::mat4> visible_trees_burning;
    std::vector<glm::mat4> visible_trees_chopped_twice;
    visible_grass.reserve(terrain_chunks.size() * GRASS_PER_CHUNK);
    visible_rocks.reserve(terrain_chunks.size() * ROCKS_PER_CHUNK);
    visible_trees_alive.reserve(terrain_chunks.size() * TREES_PER_CHUNK);
    visible_trees_chopped_once.reserve(terrain_chunks.size() * TREES_PER_CHUNK);
    visible_trees_burning.reserve(terrain_chunks.size() * TREES_PER_CHUNK);
    visible_trees_chopped_twice.reserve(terrain_chunks.size() * TREES_PER_CHUNK);

    for (const auto& chunk : terrain_chunks) {
        if (cameraFrustum.isBoxInFrustum(chunk.aabb_min, chunk.aabb_max)) {
            visible_grass.insert(visible_grass.end(), chunk.grass_matrices.begin(), chunk.grass_matrices.end());
            visible_rocks.insert(visible_rocks.end(), chunk.rock_matrices.begin(), chunk.rock_matrices.end());
            for (const auto& tree_instance : chunk.tree_instances) {
                switch (tree_instance.state) {
                case TreeState::ALIVE: {
                    visible_trees_alive.push_back(tree_instance.matrix);
                    break;
                }
                case TreeState::CHOPPED_ONCE: {
                    visible_trees_chopped_once.push_back(tree_instance.matrix);
                    break;
                }
                case TreeState::BURNING: {
                    visible_trees_burning.push_back(tree_instance.matrix);
                    break;
                }
                case TreeState::CHOPPED_TWICE: {
                    visible_trees_chopped_twice.push_back(tree_instance.matrix);
                    break;
                }
                }
            }
        }
    }

    // --- 2. DIBUJAR INSTANCIAS OPACAS (ROCAS) ---
    if (instancePhongShader != nullptr && instancePhongShader->ID != 0 && rock_model != nullptr && !visible_rocks.empty()) {
        instancePhongShader->use();
        instancePhongShader->setMat4("projection", projection);
        instancePhongShader->setMat4("view", view);
        instancePhongShader->setVec3("lightPosition", theLight.Position);
        instancePhongShader->setVec4("LightColor", theLight.Color);
        instancePhongShader->setVec4("LightPower", dimmedLightPower);
        instancePhongShader->setInt("alphaIndex", theLight.alphaIndex);
        instancePhongShader->setVec4("MaterialAmbientColor", defaultMaterial.ambient);
        instancePhongShader->setVec4("MaterialSpecularColor", defaultMaterial.specular);
        instancePhongShader->setFloat("transparency", defaultMaterial.transparency);

        glBindBuffer(GL_ARRAY_BUFFER, rockInstanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, visible_rocks.size() * sizeof(glm::mat4), &visible_rocks[0]);

        for (unsigned int i = 0; i < rock_model->meshes.size(); i++) {
            if (!rock_model->meshes[i].textures.empty()) {
                glActiveTexture(GL_TEXTURE0);
                instancePhongShader->setInt("texture_diffuse1", 0);
                glBindTexture(GL_TEXTURE_2D, rock_model->meshes[i].textures[0].id);
            }
            else {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glBindVertexArray(rock_model->meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(rock_model->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(visible_rocks.size()));
            glBindVertexArray(0);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // --- 3. DIBUJAR INSTANCIAS CON ALPHA TEST ---
    if (instanceAlphaTestPhongShader != nullptr && instanceAlphaTestPhongShader->ID != 0) {
        instanceAlphaTestPhongShader->use();
        instanceAlphaTestPhongShader->setMat4("projection", projection);
        instanceAlphaTestPhongShader->setMat4("view", view);
        instanceAlphaTestPhongShader->setVec3("lightPosition", theLight.Position);
        instanceAlphaTestPhongShader->setVec4("LightColor", theLight.Color);
        instanceAlphaTestPhongShader->setVec4("LightPower", dimmedLightPower);
        instanceAlphaTestPhongShader->setInt("alphaIndex", theLight.alphaIndex);
        instanceAlphaTestPhongShader->setFloat("transparency", 1.0f);

        // Árboles Vivos
        if (tree_model != nullptr && !visible_trees_alive.empty()) {
            instanceAlphaTestPhongShader->setVec4("MaterialAmbientColor", treeMaterial.ambient);
            instanceAlphaTestPhongShader->setVec4("MaterialSpecularColor", treeMaterial.specular);
            glBindBuffer(GL_ARRAY_BUFFER, treeInstanceVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, visible_trees_alive.size() * sizeof(glm::mat4), &visible_trees_alive[0]);
            for (unsigned int i = 0; i < tree_model->meshes.size(); i++) {
                if (!tree_model->meshes[i].textures.empty()) {
                    glActiveTexture(GL_TEXTURE0);
                    instanceAlphaTestPhongShader->setInt("texture_diffuse1", 0);
                    glBindTexture(GL_TEXTURE_2D, tree_model->meshes[i].textures[0].id);
                }
                else {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
                glBindVertexArray(tree_model->meshes[i].VAO);
                glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(tree_model->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(visible_trees_alive.size()));
                glBindVertexArray(0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        // Pasto
        if (grass_model != nullptr && !visible_grass.empty()) {
            instanceAlphaTestPhongShader->setVec4("MaterialAmbientColor", defaultMaterial.ambient);
            instanceAlphaTestPhongShader->setVec4("MaterialSpecularColor", glm::vec4(0.0f));
            glBindBuffer(GL_ARRAY_BUFFER, grassInstanceVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, visible_grass.size() * sizeof(glm::mat4), &visible_grass[0]);
            for (unsigned int i = 0; i < grass_model->meshes.size(); i++) {
                if (!grass_model->meshes[i].textures.empty()) {
                    glActiveTexture(GL_TEXTURE0);
                    instanceAlphaTestPhongShader->setInt("texture_diffuse1", 0);
                    glBindTexture(GL_TEXTURE_2D, grass_model->meshes[i].textures[0].id);
                }
                else {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
                glBindVertexArray(grass_model->meshes[i].VAO);
                glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(grass_model->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(visible_grass.size()));
                glBindVertexArray(0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // Nubes
        if (cloud_model != nullptr && !cloud_matrices.empty()) {
            instanceAlphaTestPhongShader->setVec4("MaterialAmbientColor", cloudMaterial.ambient);
            instanceAlphaTestPhongShader->setVec4("MaterialSpecularColor", cloudMaterial.specular);
            glBindBuffer(GL_ARRAY_BUFFER, cloudInstanceVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, cloud_matrices.size() * sizeof(glm::mat4), &cloud_matrices[0]);
            for (unsigned int i = 0; i < cloud_model->meshes.size(); i++) {
                if (!cloud_model->meshes[i].textures.empty()) {
                    glActiveTexture(GL_TEXTURE0);
                    instanceAlphaTestPhongShader->setInt("texture_diffuse1", 0);
                    glBindTexture(GL_TEXTURE_2D, cloud_model->meshes[i].textures[0].id);
                }
                else {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
                glBindVertexArray(cloud_model->meshes[i].VAO);
                glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(cloud_model->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(cloud_matrices.size()));
                glBindVertexArray(0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // Hojas (Activas)
        if (leaf_model != nullptr && !leaf_matrices.empty()) {
            instanceAlphaTestPhongShader->setVec4("MaterialAmbientColor", leafMaterial.ambient);
            instanceAlphaTestPhongShader->setVec4("MaterialSpecularColor", leafMaterial.specular);
            glBindBuffer(GL_ARRAY_BUFFER, leafInstanceVBO);

            GLint buffer_size = 0;
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);
            GLsizeiptr data_size_needed = leaf_matrices.size() * sizeof(glm::mat4);
            GLsizei count_to_draw = static_cast<GLsizei>(leaf_matrices.size());

            if (data_size_needed > (GLsizeiptr)buffer_size) {
                std::cerr << "WARNING: leafInstanceVBO resizing needed! Current: " << buffer_size << ", Needed: " << data_size_needed << std::endl;
                glBufferData(GL_ARRAY_BUFFER, data_size_needed, &leaf_matrices[0], GL_DYNAMIC_DRAW);
                setupInstanceVBO(leafInstanceVBO, data_size_needed / sizeof(glm::mat4), leaf_model);
            }
            else if (data_size_needed > 0) {
                glBufferSubData(GL_ARRAY_BUFFER, 0, data_size_needed, &leaf_matrices[0]);
            }
            else {
                count_to_draw = 0;
            }

            if (count_to_draw > 0) {
                for (unsigned int i = 0; i < leaf_model->meshes.size(); i++) {
                    if (!leaf_model->meshes[i].textures.empty()) {
                        glActiveTexture(GL_TEXTURE0);
                        instanceAlphaTestPhongShader->setInt("texture_diffuse1", 0);
                        glBindTexture(GL_TEXTURE_2D, leaf_model->meshes[i].textures[0].id);
                    }
                    else {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, 0);
                    }
                    glBindVertexArray(leaf_model->meshes[i].VAO);
                    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(leaf_model->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, count_to_draw);
                    glBindVertexArray(0);
                }
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    // --- 3.5 DIBUJAR ÁRBOLES GOLPEADOS UNA VEZ ---
    if (instancePhongShader != nullptr && instancePhongShader->ID != 0 && chopped_once_model != nullptr && !visible_trees_chopped_once.empty()) {
        instancePhongShader->use();
        instancePhongShader->setMat4("projection", projection);
        instancePhongShader->setMat4("view", view);
        instancePhongShader->setVec3("lightPosition", theLight.Position);
        instancePhongShader->setVec4("LightColor", theLight.Color);
        instancePhongShader->setVec4("LightPower", dimmedLightPower);
        instancePhongShader->setInt("alphaIndex", theLight.alphaIndex);
        instancePhongShader->setVec4("MaterialAmbientColor", treeMaterial.ambient * 0.7f);
        instancePhongShader->setVec4("MaterialSpecularColor", glm::vec4(0.0f));
        instancePhongShader->setFloat("transparency", 1.0f);

        glBindBuffer(GL_ARRAY_BUFFER, choppedOnceTreeInstanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, visible_trees_chopped_once.size() * sizeof(glm::mat4), &visible_trees_chopped_once[0]);

        for (unsigned int i = 0; i < chopped_once_model->meshes.size(); i++) {
            if (!chopped_once_model->meshes[i].textures.empty()) {
                glActiveTexture(GL_TEXTURE0);
                instancePhongShader->setInt("texture_diffuse1", 0);
                glBindTexture(GL_TEXTURE_2D, chopped_once_model->meshes[i].textures[0].id);
            }
            else {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glBindVertexArray(chopped_once_model->meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(chopped_once_model->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(visible_trees_chopped_once.size()));
            glBindVertexArray(0);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // --- 3.6 DIBUJAR ÁRBOLES QUEMÁNDOSE ---
    if (instancePhongShader != nullptr && instancePhongShader->ID != 0 && burning_tree_model != nullptr && !visible_trees_burning.empty()) {
        instancePhongShader->use();
        instancePhongShader->setMat4("projection", projection);
        instancePhongShader->setMat4("view", view);
        instancePhongShader->setVec3("lightPosition", theLight.Position);
        instancePhongShader->setVec4("LightColor", theLight.Color);
        instancePhongShader->setVec4("LightPower", dimmedLightPower);
        instancePhongShader->setInt("alphaIndex", theLight.alphaIndex);
        instancePhongShader->setVec4("MaterialAmbientColor", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
        instancePhongShader->setVec4("MaterialSpecularColor", glm::vec4(0.0f));
        instancePhongShader->setFloat("transparency", 1.0f);

        glBindBuffer(GL_ARRAY_BUFFER, burningTreeInstanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, visible_trees_burning.size() * sizeof(glm::mat4), &visible_trees_burning[0]);

        for (unsigned int i = 0; i < burning_tree_model->meshes.size(); i++) {
            if (!burning_tree_model->meshes[i].textures.empty()) {
                glActiveTexture(GL_TEXTURE0);
                instancePhongShader->setInt("texture_diffuse1", 0);
                glBindTexture(GL_TEXTURE_2D, burning_tree_model->meshes[i].textures[0].id);
            }
            else {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glBindVertexArray(burning_tree_model->meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(burning_tree_model->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(visible_trees_burning.size()));
            glBindVertexArray(0);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // --- 3.7 DIBUJAR ÁRBOLES GOLPEADOS DOS VECES ---
    if (instancePhongShader != nullptr && instancePhongShader->ID != 0 && chopped_twice_model != nullptr && !visible_trees_chopped_twice.empty()) {
        instancePhongShader->use();
        instancePhongShader->setMat4("projection", projection);
        instancePhongShader->setMat4("view", view);
        instancePhongShader->setVec3("lightPosition", theLight.Position);
        instancePhongShader->setVec4("LightColor", theLight.Color);
        instancePhongShader->setVec4("LightPower", dimmedLightPower);
        instancePhongShader->setInt("alphaIndex", theLight.alphaIndex);
        instancePhongShader->setVec4("MaterialAmbientColor", treeMaterial.ambient * 0.4f);
        instancePhongShader->setVec4("MaterialSpecularColor", glm::vec4(0.0f));
        instancePhongShader->setFloat("transparency", 1.0f);

        glBindBuffer(GL_ARRAY_BUFFER, choppedTwiceTreeInstanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, visible_trees_chopped_twice.size() * sizeof(glm::mat4), &visible_trees_chopped_twice[0]);

        for (unsigned int i = 0; i < chopped_twice_model->meshes.size(); i++) {
            if (!chopped_twice_model->meshes[i].textures.empty()) {
                glActiveTexture(GL_TEXTURE0);
                instancePhongShader->setInt("texture_diffuse1", 0);
                glBindTexture(GL_TEXTURE_2D, chopped_twice_model->meshes[i].textures[0].id);
            }
            else {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glBindVertexArray(chopped_twice_model->meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(chopped_twice_model->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(visible_trees_chopped_twice.size()));
            glBindVertexArray(0);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // --- 4. DIBUJAR SKYBOX ---
    if (skyboxShader != nullptr && skyboxShader->ID != 0) {
        glDepthFunc(GL_LEQUAL);
        skyboxShader->use();
        skyboxShader->setMat4("projection", projection);
        glm::mat4 view_skybox = glm::mat4(glm::mat3(view));
        skyboxShader->setMat4("view", view_skybox);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1000.0f));
        skyboxShader->setMat4("model", model);
        Model* currentSkybox = isDay ? cubeenv : cubeenv_noche;
        if (currentSkybox != nullptr) {
            currentSkybox->Draw(*skyboxShader);
        }
        glDepthFunc(GL_LESS);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // --- 5. DIBUJAR EL SOL ---
    if (sunShader != nullptr && sunShader->ID != 0 && sun_model != nullptr) {
        glDepthMask(GL_FALSE);
        sunShader->use();
        sunShader->setMat4("projection", projection);
        sunShader->setMat4("view", view);
        float sunVisibleDistance = 950.0f;
        glm::vec3 sunVisiblePosition = glm::vec3(sunVisibleDistance * cos(sunElevRad) * sin(sunAngleRad), sunVisibleDistance * sin(sunElevRad), sunVisibleDistance * cos(sunElevRad) * cos(sunAngleRad));
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, sunVisiblePosition);
        model = glm::scale(model, glm::vec3(15.0f));
        sunShader->setMat4("model", model);
        glm::vec4 sunTintColor = glm::vec4(1.0f, 1.0f, 0.8f, 1.0f) * nightDimFactor;
        sunShader->setVec4("tintColor", sunTintColor);
        sun_model->Draw(*sunShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // --- 6. DIBUJAR LA LUNA ---
    if (sunShader != nullptr && sunShader->ID != 0 && moon_model != nullptr) {
        if (sun_model == nullptr || !(sunShader && sunShader->ID != 0)) {
            glDepthMask(GL_FALSE);
        }
        float moonVisibleDistance = 900.0f;
        glm::vec3 moonVisiblePosition = glm::vec3(-moonVisibleDistance * cos(sunElevRad) * sin(sunAngleRad), -moonVisibleDistance * sin(sunElevRad), -moonVisibleDistance * cos(sunElevRad) * cos(sunAngleRad));
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, moonVisiblePosition);
        model = glm::scale(model, glm::vec3(12.0f));
        sunShader->setMat4("model", model);
        glm::vec4 moonTintColor = glm::vec4(0.8f, 0.85f, 0.95f, 1.0f) * (1.0f - dayNightTransition);
        sunShader->setVec4("tintColor", moonTintColor);
        moon_model->Draw(*sunShader);
        glDepthMask(GL_TRUE);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else {
        glDepthMask(GL_TRUE);
    }

    // --- 7. DIBUJAR LA CRUZ (CROSSHAIR) ---
    glDisable(GL_DEPTH_TEST);
    if (crosshairShader != nullptr && crosshairShader->ID != 0 && crosshairVAO != 0) {
        crosshairShader->use();
        crosshairShader->setVec4("crosshairColor", glm::vec4(1.0f, 1.0f, 1.0f, 0.8f));
        glBindVertexArray(crosshairVAO);
        glLineWidth(2.0f);
        glDrawArrays(GL_LINES, 0, 4);
        glBindVertexArray(0);
        glLineWidth(1.0f);
    }
    // glDisable(GL_DEPTH_TEST) se queda desactivado para la UI

    // --- 8. DIBUJAR UI (--- SECCIÓN COMPLETAMENTE MODIFICADA ---) ---
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (uiShader != nullptr && uiShader->ID != 0 && uiVAO != 0) {
        uiShader->use();
        glm::mat4 projection_ortho = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
        uiShader->setMat4("projection", projection_ortho);
        uiShader->setInt("texture_diffuse1", 0); // Asignar sampler a textura 0

        glBindVertexArray(uiVAO);

        // --- MODIFICADO: Definir variables de layout ---
        float icon_size = 64.0f;
        float legend_height = 20.0f; // Altura de tu imagen de leyenda
        float legend_width = 64.0f;  // Ancho de tu imagen de leyenda
        float padding = 10.0f;
        float y_pos_legend = padding; // La leyenda va abajo
        float y_pos_icon = padding + legend_height; // El icono va arriba de la leyenda
        // ----------------------------------------------


        // --- Icono de Árbol (P) ---
        // MODIFICADO: Mover el X_pos aquí para reusarlo
        float tree_icon_x_pos = SCR_WIDTH - (icon_size * 2.0f) - (padding * 2.0f);

        if (treeTextureID != 0) {
            // MODIFICADO: Posición Y
            glm::vec3 tree_icon_pos(tree_icon_x_pos, y_pos_icon, 0.0f);
            glm::mat4 model_tree = glm::mat4(1.0f);
            model_tree = glm::translate(model_tree, tree_icon_pos);
            model_tree = glm::scale(model_tree, glm::vec3(icon_size, icon_size, 1.0f));
            uiShader->setMat4("model", model_tree);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, treeTextureID);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Dibujar borde si P está presionada
            if (p_key_pressed && highlightTextureID != 0) {
                glBindTexture(GL_TEXTURE_2D, highlightTextureID);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        // --- NUEVO: Dibujar leyenda "P" (ABAJO) ---
        if (legendTreeTextureID != 0) {
            glm::vec3 legend_tree_pos(tree_icon_x_pos, y_pos_legend, 0.0f);
            glm::mat4 model_legend_tree = glm::mat4(1.0f);
            model_legend_tree = glm::translate(model_legend_tree, legend_tree_pos);
            model_legend_tree = glm::scale(model_legend_tree, glm::vec3(legend_width, legend_height, 1.0f));
            uiShader->setMat4("model", model_legend_tree);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, legendTreeTextureID);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        // ---------------------------------------------


        // --- Icono de Fuego (F/G) ---
        // MODIFICADO: Mover el X_pos aquí para reusarlo
        float fire_icon_x_pos = SCR_WIDTH - icon_size - padding;

        if (fireTextureID != 0) {
            // MODIFICADO: Posición Y
            glm::vec3 fire_icon_pos(fire_icon_x_pos, y_pos_icon, 0.0f);
            glm::mat4 model_fire = glm::mat4(1.0f);
            model_fire = glm::translate(model_fire, fire_icon_pos);
            model_fire = glm::scale(model_fire, glm::vec3(icon_size, icon_size, 1.0f));
            uiShader->setMat4("model", model_fire);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, fireTextureID);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Dibujar borde si el incendio está activo
            if (isFireActive && highlightTextureID != 0) {
                glBindTexture(GL_TEXTURE_2D, highlightTextureID);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        // --- NUEVO: Dibujar leyenda "F/G" (ABAJO) ---
        if (legendFireTextureID != 0) {
            glm::vec3 legend_fire_pos(fire_icon_x_pos, y_pos_legend, 0.0f);
            glm::mat4 model_legend_fire = glm::mat4(1.0f);
            model_legend_fire = glm::translate(model_legend_fire, legend_fire_pos);
            model_legend_fire = glm::scale(model_legend_fire, glm::vec3(legend_width, legend_height, 1.0f));
            uiShader->setMat4("model", model_legend_fire);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, legendFireTextureID);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        // ----------------------------------------------

        glBindVertexArray(0);
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    // ----------------------

    // --- Limpieza Frame ---
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glfwSwapBuffers(window);
    glfwPollEvents();
    return true;
}



void setupInstanceVBO(unsigned int& vbo, size_t max_items, Model* model_to_setup) {
    if (model_to_setup == nullptr) {
        std::cout << "ERROR::SETUP_INSTANCE_VBO: El modelo es nulo." << std::endl;
        return;
    }
    if (model_to_setup->meshes.empty()) {
        //std::cout << "WARNING::SETUP_INSTANCE_VBO: Model has no meshes. VBO=" << vbo << std::endl;
        return;
    }
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
    }
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, max_items * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    for (unsigned int i = 0; i < model_to_setup->meshes.size(); ++i) {
        unsigned int VAO = model_to_setup->meshes[i].VAO;
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(11);
        glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(12);
        glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(13);
        glVertexAttribPointer(13, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(14);
        glVertexAttribPointer(14, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
        glVertexAttribDivisor(11, 1);
        glVertexAttribDivisor(12, 1);
        glVertexAttribDivisor(13, 1);
        glVertexAttribDivisor(14, 1);
        glBindVertexArray(0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}






