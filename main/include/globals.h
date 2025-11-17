#ifndef GLOBALS_H
#define GLOBALS_H

// --- 1. INCLUDES ---
// Aquí van TODAS las librerías del proyecto
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <random>
#include <cmath>
#include <limits>
#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/norm.hpp>

#include <shader_m.h>
#include <camera.h>
#include <model.h>
#include <light.h>

#include <material.h>
#include <animatedmodel.h>
#include <animated.h>
#include <irrKlang.h>



using namespace irrklang;


// --- 2. ESTRUCTURAS ---
// Aquí van TODAS las definiciones de structs y enums
struct Plane {
    glm::vec3 normal;
    float distance;
    void normalize() {
        float length = glm::length(normal);
        if (length > 0.0f) {
            normal /= length;
            distance /= length;
        }
    }
    float getSignedDistanceToPoint(const glm::vec3& point) const {
        return glm::dot(normal, point) + distance;
    }
};

struct Frustum {
    Plane planes[6];
    void extractPlanes(const glm::mat4& vp);
    bool isBoxInFrustum(const glm::vec3& min, const glm::vec3& max) const;
};

enum class TreeState {
    ALIVE,
    CHOPPED_ONCE,
    BURNING,
    CHOPPED_TWICE
};


struct TreeInstance {
    glm::mat4 matrix;
    TreeState state = TreeState::ALIVE;
    int id;
    float fireTriggerTime = -1.0f;
    float burnOutTime = -1.0f;
};

struct Chunk {
    glm::vec3 position;
    glm::vec3 aabb_min;
    glm::vec3 aabb_max;
    std::vector<glm::mat4> grass_matrices;
    std::vector<glm::mat4> rock_matrices;
    std::vector<TreeInstance> tree_instances;
};

struct Leaf {
    glm::vec3 position;
    glm::vec3 rotationAxis;
    float rotationAngle;
    float fallSpeed;
    float spinSpeed;
    glm::vec3 initialTreePos;
    float initialHeight;
    int parentTreeId = -1;
    bool is_active = true;
    bool is_explosion_leaf = false;
};

struct UIAssets {//Buffers
    unsigned int crosshairVAO = 0;
    unsigned int crosshairVBO = 0;
    unsigned int uiVAO = 0;
    unsigned int uiVBO = 0;

    // Texturas de UI
    unsigned int fireTextureID = 0;
    unsigned int treeTextureID = 0;
    unsigned int highlightTextureID = 0;
    unsigned int legendFireTextureID = 0;
    unsigned int legendTreeTextureID = 0;
};

// --- VARIABLES GLOBALES: PERSONAJE DEMI Y CÁMARA ---
extern bool g_isThirdPerson;
extern bool g_pressM;
extern Animated* g_demiModel;
extern glm::vec3 g_demiPos;
extern float g_demiRotY;
extern bool g_demiMoving;

// --- NUEVO: VARIABLES MIKU Y SELECTOR ---

// Constantes (Declaración)
extern const float DEMI_SPEED;
extern const float DEMI_SCALE;
extern const float DEMI_CAM_DIST; // <--- AQUÍ ESTÁ LA VARIABLE FALTANTE
extern const float DEMI_OFFSET_Y;
// ... variables de demi ...
extern Animated* g_mikuModel;     // <--- FALTA ESTO
extern int g_activeCharacter;     // <--- FALTA ESTO
extern const float MIKU_SCALE;    // <--- FALTA ESTO

// --- NUEVO: Constantes de Simulación ---
const float FOREST_HEALTH_TRIGGER = 0.25f;    // 25% de vida para que los animales mueran
const float ANIMAL_DEATH_RATE = 5.0f;       // Un animal muere cada 5 segundos
const float ANIMAL_RESPAWN_RATE = 10.0f;    // Un animal reaparece cada 10 segundos


// --- Constantes y Estructuras para Animales ---
const unsigned int ANIMALS_PER_CHUNK = 1; // Lo dejamos en 1 como pediste
const float ANIMAL_MOVE_SPEED = 2.0f; // Metros por segundo
const float ANIMAL_TREE_AVOIDANCE_RADIUS = 1.5f; // Radio para evitar árboles
const int ANIMAL_MAX_PATHFIND_TRIES = 10; // Intentos para encontrar un destino
const float ANIMAL_SPAWN_PROBABILITY = 0.25f; // 25% de probabilidad

enum class AnimalState {
    IDLE,
    WALKING,
    DEAD // <-- NUEVO ESTADO
};

struct AnimalInstance {
    glm::vec3 position;
    float rotationY;
    AnimalState state;
    float stateTimer; // Tiempo restante en el estado actual
    glm::vec3 targetPosition;


    AnimatedModel* walk; //  <-- NUEVO. Puntero para el modelo de animal que estemos usando
    AnimatedModel* idle;

    // Estado de animaci n (movido de AnimatedModel)
    // Estado de animación (movido de AnimatedModel)
    float elapsedTime;
    int animationCount;
    glm::mat4 gBones[MAX_RIGGING_BONES]; // MAX_RIGGING_BONES se define en animatedmodel.h

};
// --- FIN NUEVO ---


//Estructura con los modelos de los assets
struct ForestAssets {
    // Modelos
    Model* terrain_model;
    Model* grass_model;
    Model* rock_model;
    Model* mountain_model;
    Model* tree_model;
    Model* chopped_once_model;
    Model* burning_tree_model;
    Model* chopped_twice_model;
    Model* cubeenv;
    Model* cubeenv_noche;
    Model* sun_model;
    Model* moon_model;
    Model* cloud_model;
    Model* leaf_model;

    Model* sphereDay;   // esfera_cielo.fbx
    Model* sphereNight; // esfera_noche.fbx


    AnimatedModel* character01; // <-- MODELO DE LOBO CORRIENDO
    AnimatedModel* character02; // <-- MODELO DE LOBO ESTATICO (PENDIENTE)
    AnimatedModel* character03; // <-- MODELO DE CASTOR CORRIENDO
    AnimatedModel* character04; // <-- MODELO DE CASTOR ESTATICO
    AnimatedModel* character05; // <-- MODELO DE OSO NEGRO CORRIENDO
    AnimatedModel* character06; // <-- MODELO DE OSO ESTATICO (PENDIENTE)
    AnimatedModel* character07; // <-- MODELO DE ALCE CORRIENDO
    AnimatedModel* character08; // <-- NUEVO DE ALCE ESTATICO

    Model* skull_model; // <-- NUEVO MODELO
    // --- Luz
    Light theLight;
    // Material
    Material defaultMaterial;
    Material mountainMaterial;
    Material treeMaterial;
    Material cloudMaterial;
    Material leafMaterial;
    Material sunMaterial;
    // --> Variables GLOBALES VAO (Vertex Array Object) Y VBO ((Vertex Buffer Object))
    unsigned int grassInstanceVBO = 0;
    unsigned int rockInstanceVBO = 0;
    unsigned int treeInstanceVBO = 0;
    unsigned int choppedOnceTreeInstanceVBO = 0;
    unsigned int burningTreeInstanceVBO = 0;
    unsigned int choppedTwiceTreeInstanceVBO = 0;
    unsigned int cloudInstanceVBO = 0;
    unsigned int leafInstanceVBO = 0;
};

struct TestAssets { //Elementos Para el entorno de Prueba
    Light light01;
    Light light02;
    Model* car;
    Model* luminaire;
    Model* stop;
    Model* floor;
    AnimatedModel *character01;
    Light cameraLight;

    //Modelos
    Model* suelo, *suelo_verde, *metales, *objMadera, *objCristales;
    Model* objPlasticos, *objConcreto, *objLlantas, *objLadrillo;
    Model* bandera, *grafitis, *luzSemaforo, *co2;

    //BASURAS
    Model* basura1, * basura2, * basura3, * basura4, * basura5, * basura6;

    //BOTES
    Model* bote1, * bote2, * bote3, * bote4, * bote5, * bote6, * bote7, * bote8;

    //CONTENEDORES
    Model* contenedor1, * contenedor2, * contenedor3, * contenedor4, * contenedor5, * contenedor6, * contenedor7, * contenedor8;

    //MUEBLES
    Model* mueble1, * mueble2, * mueble3, * mueble4, * mueble5, * mueble6, * mueble7, * mueble8, * mueble9, * mueble10; 

    //Modelos IA
    Model* hopstial, * fabrica, * policia, * tienda, * rascacielos, *edificio2, *banco, *camion;


    


    //Materiales
    Material asphalt, cesped, concreto, goma, ladrillo, tela, gases, focos;
    

    Material steel;
    Material plastic;
    Material traslucido;
    Material madera;
};

struct EspacioAssets {
    // Modelos
    Model* Tierra;
    Model* Canada;
    Model* Luna;
    Model* cubeenv_noche;
    Model* sol;
    // --- Luz
    Light theLight;
    // Material
    Material defaultMaterial;

};

struct MenuAssets {
    // Modelos
    Model* Texto1;
    Model* Texto2;
    Model* Texto3;
    Model* Texto4;
    Model* Texto5;
    Model* Texto6;
    Model* HojaArce_Menu;
    Model* Fondo_Menu;
};

struct GlaciarAssets {
    // Modelos
    Model* BarraAzul;
    Model* BarraRoja;
    Model* TerAzulBase;
    Model* TerRojoBase;
    Model* TrozoH1;
    Model* TrozoH2;
    Model* Agua;
    Model* Iceberg;
    Model* PlacaHielo;
    Model* Glaciares;
    Animated* Oso1;
    Animated* Oso2;

    // --- Luz
    Light theLight;
    // Material
    Material defaultMaterial;
	Material nieveMaterial;
	Material GlaciaresMaterial;
};


extern glm::vec3 osoUltimoGiro[3];
extern float osoYawDeg[3];

// -- Assets del Bosque
extern ForestAssets fa;

// -- Assets de UI
extern UIAssets ui;

// -- Assets de Test
extern TestAssets ta;

extern EspacioAssets ea;

extern MenuAssets ma;

extern GlaciarAssets ga;

// --- 3. DECLARACIONES GLOBALES (EXTERN) ---
// 'extern' le dice al compilador: "esta variable existe,
// pero su memoria está definida en OTRO archivo .cpp"

// pero su memoria está definida en OTRO archivo .cpp"

extern GLFWwindow* window;
extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;

// --- NUEVO: Variables para el modo de pantalla completa ---
extern bool is_fullscreen;
extern int last_window_width;
extern int last_window_height;
extern int last_window_x;
extern int last_window_y;
// --- FIN NUEVO ---

extern Camera camera;
extern float lastX;
extern float lastY;
extern bool firstMouse;
extern bool g_isWindowFocused;

extern float deltaTime;
extern float lastFrame;
extern float sunAngle;
extern float sunElevationAngle;

// --- NUEVO: Globales de Salud del Bosque y Animales ---
extern float g_forestHealth;
extern float g_previousForestHealth;
extern int g_totalInitialTrees;
extern int g_currentLivingTrees;
extern int g_maxAnimalsInWorld;
extern float g_animalDeathTimer;
extern float g_animalRespawnTimer;
// --- FIN NUEVO ---

// --- Globales Random ---
extern std::random_device rd;
extern std::mt19937 gen;
extern const float CHUNK_SIZE;
extern const float fireDuration;
extern const float minBurnDuration;
extern const float maxBurnDuration;

extern std::uniform_real_distribution<float> dis_pos;
extern std::uniform_real_distribution<float> dis_scale_grass;
extern std::uniform_real_distribution<float> dis_scale_rock;
extern std::uniform_real_distribution<float> dis_scale_tree;
extern std::uniform_real_distribution<float> dis_rot;
extern std::uniform_real_distribution<float> dis_axis;
extern std::uniform_real_distribution<float> dis_leaf_offset;
extern std::uniform_real_distribution<float> dis_leaf_height;
extern std::uniform_real_distribution<float> dis_initial_fall;
extern std::uniform_real_distribution<float> dis_initial_spin;
extern std::uniform_real_distribution<float> dis_explode_y;
extern std::uniform_real_distribution<float> dis_explode_angle;
extern std::uniform_real_distribution<float> dis_explode_radius;
extern std::uniform_real_distribution<float> dis_explode_fall;
extern std::uniform_real_distribution<float> dis_explode_spin;
extern std::uniform_real_distribution<float> dis_fire_time;
extern std::uniform_real_distribution<float> dis_burn_duration;

// --- Distribuciones para IA ---
extern std::uniform_real_distribution<float> dis_ai_time; // Tiempo en estado
extern std::uniform_real_distribution<float> dis_ai_target_dist; // Distancia a caminar
extern std::uniform_real_distribution<float> dis_spawn_chance;

extern const float minCloudDistanceSq;
extern const int maxPlacementTries;
extern std::uniform_real_distribution<float> dis_cloud_distant_x;
extern std::uniform_real_distribution<float> dis_cloud_distant_z;
extern std::uniform_real_distribution<float> dis_cloud_y;
extern std::uniform_real_distribution<float> dis_cloud_scale;

// --- Bosque
extern Shader* uiShader;
extern Shader* mLightsShader;
extern Shader* phongShader;
extern Shader* phongShader2;
extern Shader* instancePhongShader;
extern Shader* instanceAlphaTestPhongShader;
extern Shader* skyboxShader;
extern Shader* sunShader;
extern Shader* crosshairShader;
extern Shader* dynamicShader;
extern Shader* fresnelShader;
extern Shader* moonShader;
extern Shader* basicShader;
extern Shader* wavesShader;
extern Shader* wavesShader2;

extern bool isDay;

// Cubemap ambiental para Fresnel
extern unsigned int g_envCubemapTexID; // <- NUEVO

// Modelo de cristal (edificio/panel)
extern Model* g_glassModel;

// --- Contenedores del Mundo ---
extern std::vector<Chunk> terrain_chunks;
extern const int WORLD_SIZE;

// --- Límites del mundo y vector de animales ---
extern float WORLD_MIN_X, WORLD_MAX_X, WORLD_MIN_Z, WORLD_MAX_Z;
extern std::vector<AnimalInstance> g_animals;
// --- FIN NUEVO ---

extern Frustum cameraFrustum;

// -- Arreglo de Camaras
extern std::vector<Light> gLights;

// --- Constantes ---
extern const unsigned int GRASS_PER_CHUNK;
extern const unsigned int ROCKS_PER_CHUNK;
extern const unsigned int TREES_PER_CHUNK;
extern const unsigned int LEAVES_PER_TREE;
extern const unsigned int NUM_DISTANT_CLOUDS;
extern const unsigned int NUM_LOCAL_CLOUDS;
extern const unsigned int TOTAL_CLOUDS;
extern const unsigned int EXPLOSION_LEAVES_PER_HIT;

// --- Vectores de Instancias ---
extern std::vector<glm::mat4> cloud_matrices;
extern std::vector<glm::vec3> cloud_positions;
extern std::vector<Leaf> falling_leaves;
extern std::vector<glm::mat4> leaf_matrices;
extern int next_tree_id;

extern const glm::vec3 tree_trunk_aabb_min;
extern const glm::vec3 tree_trunk_aabb_max;

// --- Globales de Lógica ---
extern bool p_key_pressed;
extern bool f_key_pressed;
extern bool g_key_pressed;
extern bool z_key_pressed;
extern bool r_key_pressed;
extern const float max_plant_distance;
extern bool isFireActive;
extern float fireStartTime;

//Variables para el control del volumen
extern bool plus_key_pressed;  
extern bool minus_key_pressed;

// --- Audio ---
extern ISoundEngine* SoundEngine;

// Bandera Para Entorno de Prueba
extern bool g_runTestEnvironment;

//Variables para Tierra y Glaciar
extern float Time;
extern float TimeA1;

extern float transparenciaC;
extern float wavesTime;
extern float avanceA1;

extern float DT1;
extern float DT2;
extern float DT3;
extern float DT4;
extern float DT5;
extern float DT6;
extern float DHoja;
extern const float velocidadCarga;

extern float temperatura;
extern float barraTF;
extern float barraTC;

extern int escena;

extern bool menu;
extern bool animacion1;
extern bool calor;

// === Variables globales para derretimiento en escena 1 ===
extern float glacierScaleY;          // escala vertical de Glaciares (1 → 0)
extern float meltSpeedBase;         // factor base de derretimiento por °C
extern float osoDropFactor;          // descenso de osos por unidad derretida

// === Movimiento aleatorio segmentado para Oso3, Oso4 y Oso5 ===
// Recorren tramos de 10 unidades y giran ±90° al azar al finalizar cada tramo.
extern const float OSO_SEGMENTO;
extern const float OSO_SPEED;            // unidades/segundo
extern glm::vec3 osoPos[3];

extern glm::vec3 posicionActual;
extern glm::vec3 posicionA1;
extern glm::vec3 posicionCarga;
extern glm::vec3 posicionInicioG;
extern glm::vec3 posicionOrigen;
extern glm::vec3 posicionEscenario1;

// Shaders para materiales
extern Shader* lambertShader;

//Movimiento de camara animacion1
struct CamAni {
    bool   active = false;
    double t0;         // tiempo inicio (segundos)
    double dur;         // duración deseada (segundos)
    glm::vec3 p0;                 // posición inicial cámara
    glm::vec3 p1;                 // posición objetivo
    glm::vec3 lookAt0;            // punto inicial de mira
    glm::vec3 lookAt1;            // punto objetivo de mira
};
extern CamAni g_anim1;


struct MenuAnim {
    double t0;         // tiempo inicio (segundos)
    double dur;         // duración deseada (segundos)
    glm::vec3 p0;                 // posición cámara
    glm::vec3 lookAt;            // opcional: punto inicial de mira
};
extern MenuAnim g_menu;


extern int rrr;
extern int bandera;
#endif