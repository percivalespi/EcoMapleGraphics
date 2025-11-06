#ifndef GLOBALS_H
#define GLOBALS_H

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
#include <irrKlang.h>



using namespace irrklang;


// --- 2. ESTRUCTURAS ---
// Aqu  van TODAS las definiciones de structs y enums
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


// --- NUEVO: Constantes y Estructuras para Animales ---
const unsigned int ANIMALS_PER_CHUNK = 1;
const float ANIMAL_MOVE_SPEED = 2.0f; // Metros por segundo
const float ANIMAL_TREE_AVOIDANCE_RADIUS = 1.5f; // Radio para evitar  rboles
const int ANIMAL_MAX_PATHFIND_TRIES = 10; // Intentos para encontrar un destino

enum class AnimalState {
    IDLE,
    WALKING
};
const float ANIMAL_SPAWN_PROBABILITY = 0.25f;
struct AnimalInstance {
    glm::vec3 position;
    float rotationY;
    AnimalState state;
    float stateTimer; // Tiempo restante en el estado actual
    glm::vec3 targetPosition;


    float elapsedTime;
    int animationCount;
    glm::mat4 gBones[MAX_RIGGING_BONES];
};


extern std::uniform_real_distribution<float> dis_ai_time; // Tiempo en estado
extern std::uniform_real_distribution<float> dis_ai_target_dist; // Distancia a caminar

extern std::uniform_real_distribution<float> dis_spawn_chance;


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
    AnimatedModel* character01;
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
    Material steel;
    Material asphalt;
    Model* car;
    Model* luminaire;
    Model* stop;
    Model* floor;
};

// -- Assets del Bosque
extern ForestAssets fa;

// -- Assets de UI
extern UIAssets ui;

// -- Assets de Test
extern TestAssets ta;



extern GLFWwindow* window;
extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

extern Camera camera;
extern float lastX;
extern float lastY;
extern bool firstMouse;

extern float deltaTime;
extern float lastFrame;
extern float sunAngle;
extern float sunElevationAngle;

// --- Globales Random ---
extern std::random_device rd;
extern std::mt19937 gen;
extern const float CHUNK_SIZE;
extern const float fireDuration;
extern const float minBurnDuration;
extern const float maxBurnDuration;
extern bool isDay;
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


extern std::uniform_real_distribution<float> dis_ai_time; // Tiempo en estado
extern std::uniform_real_distribution<float> dis_ai_target_dist; // Distancia a caminar

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
extern Shader* instancePhongShader;
extern Shader* instanceAlphaTestPhongShader;
extern Shader* skyboxShader;
extern Shader* sunShader;
extern Shader* crosshairShader;
extern Shader* dynamicShader;

// --- Contenedores del Mundo ---
extern std::vector<Chunk> terrain_chunks;
extern const int WORLD_SIZE;


extern float WORLD_MIN_X, WORLD_MAX_X, WORLD_MIN_Z, WORLD_MAX_Z;
extern std::vector<AnimalInstance> g_animals;


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

// --- Globales de L gica ---
extern bool p_key_pressed;
extern bool f_key_pressed;
extern bool g_key_pressed;
extern const float max_plant_distance;
extern bool isFireActive;
extern float fireStartTime;

// --- Audio ---
extern ISoundEngine* SoundEngine;

// Bandera Para Entorno de Prueba
extern bool g_runTestEnvironment;


#endif