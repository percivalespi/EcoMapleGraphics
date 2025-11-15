#include "mechanic.h"
#include "globals.h"
//Hola
void plantTree() {
    // 1. Calcular Rayo en Coordenadas del Mundo
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 2000.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::vec4 ray_clip(0.0f, 0.0f, -1.0, 1.0);
    glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
    glm::vec3 ray_wor = glm::normalize(glm::vec3(glm::inverse(view) * ray_eye));
    glm::vec3 ray_origin = camera.Position;

    // 2. Intersecci n Rayo-Plano (Suelo en Y=0)
    glm::vec3 plane_normal(0.0f, 1.0f, 0.0f);
    float plane_d = 0.0f;
    float denom = glm::dot(plane_normal, ray_wor);
    float t_ground = -1.0f;
    glm::vec3 ground_hit_pos;

    if (std::abs(denom) > 1e-6) {
        float t = -(glm::dot(plane_normal, ray_origin) + plane_d) / denom;
        if (t > 0.0f && t < max_plant_distance) {
            t_ground = t;
            ground_hit_pos = ray_origin + ray_wor * t_ground;
        }
    }

    // 3. Intersecci n Rayo-Bases de  rboles (CHOPPED_TWICE)
    float closest_t_tree_local = std::numeric_limits<float>::max(); // CORREGIDO: Declarado
    float closest_t_tree_world = std::numeric_limits<float>::max();
    int hit_chunk_idx = -1;
    size_t hit_tree_idx = -1;
    int hit_tree_id = -1;
    glm::vec3 tree_base_hit_pos;

    for (int c_idx = 0; c_idx < terrain_chunks.size(); ++c_idx) {
        if (glm::distance(camera.Position, terrain_chunks[c_idx].position) > CHUNK_SIZE * 1.5f + max_plant_distance) {
            continue;
        }

        for (size_t t_idx = 0; t_idx < terrain_chunks[c_idx].tree_instances.size(); ++t_idx) {
            const auto& tree_instance = terrain_chunks[c_idx].tree_instances[t_idx];

            if (tree_instance.state != TreeState::CHOPPED_TWICE) {
                continue;
            }

            glm::mat4 inv_model = glm::inverse(tree_instance.matrix);
            glm::vec3 local_ray_origin = glm::vec3(inv_model * glm::vec4(ray_origin, 1.0f));
            glm::vec3 local_ray_dir = glm::normalize(glm::vec3(inv_model * glm::vec4(ray_wor, 0.0f)));

            float t_local;
            if (intersectRayAABB(local_ray_origin, local_ray_dir, tree_trunk_aabb_min, tree_trunk_aabb_max, t_local)) {
                glm::vec3 world_hit_point = glm::vec3(tree_instance.matrix * glm::vec4(local_ray_origin + local_ray_dir * t_local, 1.0f));
                float world_distance = glm::distance(ray_origin, world_hit_point);

                if (world_distance < max_plant_distance && world_distance < closest_t_tree_world) {
                    closest_t_tree_local = t_local;
                    closest_t_tree_world = world_distance;
                    hit_chunk_idx = c_idx;
                    hit_tree_idx = t_idx;
                    hit_tree_id = tree_instance.id;
                    tree_base_hit_pos = glm::vec3(tree_instance.matrix[3]);
                }
            }
        }
    }

    // 4. Decidir d nde plantar
    bool hit_ground = (t_ground > 0.0f);
    bool hit_tree_base = (hit_tree_id != -1);

    glm::vec3 final_plant_pos;
    int target_chunk_idx = -1;
    bool should_plant = false;
    bool remove_old_base = false;
    size_t old_base_index = (size_t)-1;

    if (!hit_ground && !hit_tree_base) {
        std::cout << "Mire al suelo o a una base cortada para plantar." << std::endl;
        return;
    }

    if (hit_ground && (!hit_tree_base || t_ground <= closest_t_tree_world)) {
        final_plant_pos = ground_hit_pos;
        final_plant_pos.y = 0.0f;
        should_plant = true;

        int chunk_x = static_cast<int>(std::floor((final_plant_pos.x + CHUNK_SIZE / 2.0f) / CHUNK_SIZE));
        int chunk_z = static_cast<int>(std::floor((final_plant_pos.z + CHUNK_SIZE / 2.0f) / CHUNK_SIZE));
        int chunk_idx_x = chunk_x + WORLD_SIZE / 2;
        int chunk_idx_z = chunk_z + WORLD_SIZE / 2;
        target_chunk_idx = chunk_idx_x * WORLD_SIZE + chunk_idx_z;

    }
    else if (hit_tree_base) {
        final_plant_pos = tree_base_hit_pos;
        final_plant_pos.y = 0.0f;
        should_plant = true;
        remove_old_base = true;
        target_chunk_idx = hit_chunk_idx;
        old_base_index = hit_tree_idx;
    }

    // 5. Ejecutar Plantado
    if (should_plant) {
        if (target_chunk_idx >= 0 && target_chunk_idx < (int)terrain_chunks.size()) {

            Chunk& target_chunk = terrain_chunks[target_chunk_idx];

            if (remove_old_base) {
                std::cout << "Reemplazando base de  rbol ID: " << hit_tree_id << std::endl;
                if (old_base_index < target_chunk.tree_instances.size() &&
                    target_chunk.tree_instances[old_base_index].id == hit_tree_id)
                {
                    target_chunk.tree_instances.erase(target_chunk.tree_instances.begin() + old_base_index);
                }
                else {
                    bool found = false;
                    for (auto it = target_chunk.tree_instances.begin(); it != target_chunk.tree_instances.end(); ++it) {
                        if (it->id == hit_tree_id) {
                            target_chunk.tree_instances.erase(it);
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        std::cerr << "ERROR: No se pudo encontrar la base del  rbol para borrarla." << std::endl;
                    }
                }
            }
            else {
                std::cout << "Plantando nuevo  rbol en el suelo." << std::endl;
            }

            // Crear nueva instancia de  rbol
            TreeInstance new_tree;
            new_tree.id = next_tree_id++;
            new_tree.state = TreeState::ALIVE;
            new_tree.fireTriggerTime = -1.0f;
            new_tree.burnOutTime = -1.0f;

            if (isFireActive) {
                float fire_elapsed = (float)glfwGetTime() - fireStartTime;
                float remaining_time = fireDuration - fire_elapsed;
                if (remaining_time > maxBurnDuration) {
                    std::uniform_real_distribution<float> dis_new_fire_time(fire_elapsed, fireDuration - maxBurnDuration);
                    new_tree.fireTriggerTime = dis_new_fire_time(gen);
                    new_tree.burnOutTime = new_tree.fireTriggerTime + dis_burn_duration(gen);
                    std::cout << " ->  rbol plantado se quemar  en " << (new_tree.fireTriggerTime - fire_elapsed) << "s" << std::endl;
                }
                else {
                    std::cout << " ->  rbol plantado demasiado tarde, no se quemar ." << std::endl;
                }
            }

            float scale_factor = dis_scale_tree(gen);
            new_tree.matrix = glm::mat4(1.0f);
            new_tree.matrix = glm::translate(new_tree.matrix, final_plant_pos);
            new_tree.matrix = glm::rotate(new_tree.matrix, glm::radians(dis_rot(gen)), glm::vec3(0.0f, 1.0f, 0.0f));
            new_tree.matrix = glm::rotate(new_tree.matrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            new_tree.matrix = glm::scale(new_tree.matrix, glm::vec3(scale_factor));

            target_chunk.tree_instances.push_back(new_tree);

            // --- NUEVO: Incrementar salud del bosque ---
            g_currentLivingTrees++;
            // --- FIN NUEVO ---


            // A adir hojas para el nuevo  rbol
            glm::vec3 treeBasePos = final_plant_pos;
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
                leaf.parentTreeId = new_tree.id;
                leaf.is_active = true;
                leaf.is_explosion_leaf = false;
                falling_leaves.push_back(leaf);
            }

        }
        else {
            std::cout << "Intento de plantar en un chunk inv lido." << std::endl;
        }
    }
}

void startFire() {
    if (isFireActive) {
        std::cout << "El incendio ya est  en progreso." << std::endl;
        return;
    }

    std::cout << " Iniciando incendio!" << std::endl;
    isFireActive = true;
    fireStartTime = (float)glfwGetTime();

    for (Chunk& chunk : terrain_chunks) {
        for (TreeInstance& tree : chunk.tree_instances) {
            if (tree.state == TreeState::ALIVE || tree.state == TreeState::CHOPPED_ONCE) {
                tree.fireTriggerTime = dis_fire_time(gen);
                tree.burnOutTime = tree.fireTriggerTime + dis_burn_duration(gen);
            }
        }
    }
}

bool intersectRayAABB(const glm::vec3& ray_origin, const glm::vec3& ray_dir, const glm::vec3& aabb_min, const glm::vec3& aabb_max, float& t) {
    glm::vec3 inv_dir;
    inv_dir.x = (ray_dir.x == 0.0f) ? std::numeric_limits<float>::infinity() : 1.0f / ray_dir.x;
    inv_dir.y = (ray_dir.y == 0.0f) ? std::numeric_limits<float>::infinity() : 1.0f / ray_dir.y;
    inv_dir.z = (ray_dir.z == 0.0f) ? std::numeric_limits<float>::infinity() : 1.0f / ray_dir.z;
    glm::vec3 tMin = (aabb_min - ray_origin) * inv_dir;
    glm::vec3 tMax = (aabb_max - ray_origin) * inv_dir;
    glm::vec3 t1 = glm::min(tMin, tMax);
    glm::vec3 t2 = glm::max(tMin, tMax);
    float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
    float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);
    if (tNear > tFar || tFar < 0.0f) {
        return false;
    }
    t = tNear < 0.0f ? tFar : tNear;
    return true;
}

void Frustum::extractPlanes(const glm::mat4& vp) {
    planes[0].normal = glm::vec3(vp[0][3] + vp[0][0], vp[1][3] + vp[1][0], vp[2][3] + vp[2][0]);
    planes[0].distance = vp[3][3] + vp[3][0];
    planes[1].normal = glm::vec3(vp[0][3] - vp[0][0], vp[1][3] - vp[1][0], vp[2][3] - vp[2][0]);
    planes[1].distance = vp[3][3] - vp[3][0];
    planes[2].normal = glm::vec3(vp[0][3] + vp[0][1], vp[1][3] + vp[1][1], vp[2][3] + vp[2][1]);
    planes[2].distance = vp[3][3] + vp[3][1];
    planes[3].normal = glm::vec3(vp[0][3] - vp[0][1], vp[1][3] - vp[1][1], vp[2][3] - vp[2][1]);
    planes[3].distance = vp[3][3] - vp[3][1];
    planes[4].normal = glm::vec3(vp[0][3] + vp[0][2], vp[1][3] + vp[1][2], vp[2][3] + vp[2][2]);
    planes[4].distance = vp[3][3] + vp[3][2];
    planes[5].normal = glm::vec3(vp[0][3] - vp[0][2], vp[1][3] - vp[1][2], vp[2][3] - vp[2][2]);
    planes[5].distance = vp[3][3] - vp[3][2];
    for (int i = 0; i < 6; ++i) {
        planes[i].normalize();
    }
}

bool Frustum::isBoxInFrustum(const glm::vec3& min, const glm::vec3& max) const {
    for (int i = 0; i < 6; ++i) {
        glm::vec3 p_vertex = min;
        if (planes[i].normal.x >= 0) {
            p_vertex.x = max.x;
        }
        if (planes[i].normal.y >= 0) {
            p_vertex.y = max.y;
        }
        if (planes[i].normal.z >= 0) {
            p_vertex.z = max.z;
        }
        if (planes[i].getSignedDistanceToPoint(p_vertex) < 0) {
            return false;
        }
    }
    return true;
}

// --- NUEVA FUNCI N: Comprueba si una posici n es v lida (dentro del mundo y no en un  rbol) ---
bool isPositionSafe(glm::vec3 pos) {
    // 1. Comprobar l mites del mundo
    if (pos.x < WORLD_MIN_X || pos.x > WORLD_MAX_X || pos.z < WORLD_MIN_Z || pos.z > WORLD_MAX_Z) {
        return false;
    }

    // 2. Comprobar colisi n con  rboles
    // (Optimizaci n: podr amos usar una grid, pero esto es m s simple por ahora)
    for (const auto& chunk : terrain_chunks) {
        // Optimizaci n r pida: si el chunk est  muy lejos, no lo compruebes
        if (glm::distance2(glm::vec2(pos.x, pos.z), glm::vec2(chunk.position.x, chunk.position.z)) > (CHUNK_SIZE * 2.0f) * (CHUNK_SIZE * 2.0f)) {
            continue;
        }

        for (const auto& tree : chunk.tree_instances) {
            if (tree.state == TreeState::CHOPPED_TWICE) continue; // Puede caminar sobre tocones

            glm::vec3 treePos = glm::vec3(tree.matrix[3]);
            if (glm::distance(glm::vec2(pos.x, pos.z), glm::vec2(treePos.x, treePos.z)) < ANIMAL_TREE_AVOIDANCE_RADIUS) {
                return false; // Demasiado cerca de un  rbol
            }
        }
    }

    return true; // Es seguro
}

// --- NUEVA FUNCI N: Actualiza la IA de todos los animales ---
void updateAnimalAI(float deltaTime) {
    // No se necesitan comprobaciones iniciales, el bucle manejar  una lista vac a.
    for (AnimalInstance& animal : g_animals) {

        // Saltarse a los muertos o a los que no tienen modelos de animaci n asignados
        if (animal.state == AnimalState::DEAD || animal.walk == nullptr || animal.idle == nullptr) {
            continue;
        }

        animal.stateTimer -= deltaTime;

        // --- L gica de cambio de estado ---
        if (animal.stateTimer <= 0.0f) {
            if (animal.state == AnimalState::IDLE) {
                // Cambiar a WALKING
                animal.state = AnimalState::WALKING;
                animal.stateTimer = dis_ai_time(gen); // Tiempo que pasar  caminando

                // Encontrar un nuevo destino v lido
                int tries = 0;
                bool foundTarget = false;
                do {
                    float angle = glm::radians(dis_rot(gen));
                    float dist = dis_ai_target_dist(gen);
                    animal.targetPosition = animal.position + glm::vec3(sin(angle) * dist, 0.0f, cos(angle) * dist);

                    // isPositionSafe ya comprueba los l mites del mundo y la colisi n con  rboles
                    foundTarget = isPositionSafe(animal.targetPosition);
                    tries++;
                } while (!foundTarget && tries < ANIMAL_MAX_PATHFIND_TRIES);

                if (!foundTarget) { // No se encontr  destino, quedarse quieto
                    animal.state = AnimalState::IDLE;
                    animal.stateTimer = dis_ai_time(gen) / 2.0f; // Esperar menos tiempo
                }

            }
            else { // animal.state == AnimalState::WALKING
                // Cambiar a IDLE
                animal.state = AnimalState::IDLE;
                animal.stateTimer = dis_ai_time(gen); // Tiempo que pasar  quieto
                animal.animationCount = 0; // Reiniciar conteo de animaci n para un bucle limpio
            }
        }

        // --- L gica de movimiento y animaci n ---
        if (animal.state == AnimalState::WALKING) {
            glm::vec3 direction = glm::normalize(animal.targetPosition - animal.position);
            float distanceToTarget = glm::distance(animal.position, animal.targetPosition);

            if (distanceToTarget < 0.5f) {
                // Lleg  al destino
                animal.state = AnimalState::IDLE;
                animal.stateTimer = dis_ai_time(gen);
                animal.animationCount = 0; // Reiniciar conteo de animaci n
            }
            else {
                // Moverse hacia el destino
                animal.position += direction * ANIMAL_MOVE_SPEED * deltaTime;

                // --- CORRECCI N DE ROTACI N ---
                // Calcular el  ngulo base
                animal.rotationY = atan2(direction.x, direction.z); // Rotar para mirar en la direcci n


                // Actualizar animaci n de caminar (WALKING)
                animal.elapsedTime += deltaTime;
                if (animal.elapsedTime > 1.0f / animal.walk->fps) {
                    animal.elapsedTime = 0.0f;
                    animal.animationCount++;
                    if (animal.animationCount >= animal.walk->keys) {
                        animal.animationCount = 0; // Bucle de animaci n
                    }
                    // Actualiza el array gBones de ESTA instancia con SU animaci n de caminar
                    animal.walk->SetPose((float)animal.animationCount, animal.gBones);
                }
            }
        }
        else if (animal.state == AnimalState::IDLE) {
            // Actualizar animaci n de reposo (IDLE)
            animal.elapsedTime += deltaTime;
            if (animal.elapsedTime > 1.0f / animal.idle->fps) {
                animal.elapsedTime = 0.0f;
                animal.animationCount++;
                if (animal.animationCount >= animal.idle->keys) {
                    animal.animationCount = 0; // Bucle de animaci n
                }
                // Actualiza el array gBones de ESTA instancia con SU animaci n de reposo
                animal.idle->SetPose((float)animal.animationCount, animal.gBones);
            }
        }

        // --- CORRECCI N: Forzar la posici n dentro de los l mites del mundo ---
        animal.position.x = glm::clamp(animal.position.x, WORLD_MIN_X, WORLD_MAX_X - 20.0f);
        animal.position.z = glm::clamp(animal.position.z, WORLD_MIN_Z, WORLD_MAX_Z - 20.0f);
    }
}


// --- NUEVA FUNCI N ---
// Esta funci n ahora maneja la IA y la simulaci n de vida/muerte
void updateForestHealthAndAnimals(float deltaTime) {
    // 1. Actualizar Salud del Bosque
    if (g_totalInitialTrees > 0) {
        g_forestHealth = (float)g_currentLivingTrees / (float)g_totalInitialTrees;
    }
    else {
        g_forestHealth = 0.0f; // No hab a  rboles para empezar
    }
    g_forestHealth = glm::clamp(g_forestHealth, 0.0f, 1.0f);

    // Imprimir solo si la salud cambia
    if (g_forestHealth != g_previousForestHealth) {
        std::cout << "Vida del Bosque: " << (g_forestHealth * 100.0f) << "% (" << g_currentLivingTrees << "/" << g_totalInitialTrees << ")" << std::endl;
        g_previousForestHealth = g_forestHealth; // Actualizar el valor anterior
    }

    // 2. Actualizar IA de animales vivos
    updateAnimalAI(deltaTime);

    // 3. L gica de Muerte de Animales
    if (g_forestHealth <= FOREST_HEALTH_TRIGGER) {
        g_animalDeathTimer -= deltaTime;
        if (g_animalDeathTimer <= 0.0f) {
            g_animalDeathTimer = ANIMAL_DEATH_RATE; // Reiniciar temporizador

            // Buscar un animal vivo para matarlo
            for (AnimalInstance& animal : g_animals) {
                if (animal.state != AnimalState::DEAD) {
                    animal.state = AnimalState::DEAD;
                    std::cout << "Un animal ha muerto debido a la baja salud del bosque." << std::endl;
                    break; // Solo matar uno por ciclo
                }
            }
        }
    }

    // 4. L gica de Respawn de Animales
    // Contar cu ntos animales est n vivos actualmente
    int living_animals = 0;
    for (const auto& animal : g_animals) {
        if (animal.state != AnimalState::DEAD) {
            living_animals++;
        }
    }

    if (!isFireActive && g_forestHealth > FOREST_HEALTH_TRIGGER && living_animals < g_maxAnimalsInWorld) {
        g_animalRespawnTimer -= deltaTime;
        if (g_animalRespawnTimer <= 0.0f) {
            g_animalRespawnTimer = ANIMAL_RESPAWN_RATE; // Reiniciar temporizador

            // Buscar un animal muerto para reaparecer
            for (AnimalInstance& animal : g_animals) {
                if (animal.state == AnimalState::DEAD) {
                    //  Reaparecer este! Encontrar una posici n segura
                    int tries = 0;
                    bool respawned = false;
                    do {
                        Chunk& randomChunk = terrain_chunks[gen() % terrain_chunks.size()];
                        glm::vec3 spawnPos = randomChunk.position + glm::vec3(dis_pos(gen), 0.0f, dis_pos(gen));

                        if (isPositionSafe(spawnPos)) {
                            animal.position = spawnPos;
                            animal.rotationY = glm::radians(dis_rot(gen));
                            animal.state = AnimalState::IDLE;
                            animal.stateTimer = dis_ai_time(gen);
                            animal.targetPosition = animal.position;
                            animal.elapsedTime = 0.0f;
                            animal.animationCount = 0;
                            // Asegurarse de que la pose inicial es la de reposo (idle)
                            if (animal.idle) {
                                animal.idle->SetPose(0.0f, animal.gBones);
                            }

                            std::cout << "Un animal ha reaparecido." << std::endl;
                            respawned = true;
                            break; // Salir del bucle for
                        }
                        tries++;
                    } while (tries < 10); // Intentar 10 veces encontrar un lugar

                    if (respawned) {
                        break; // Salir del bucle for principal si ya se ha reaparecido uno
                    }
                }
            }
        }
    }
}


void updateGameLogic() {
    if (g_runTestEnvironment) return;

    if (isFireActive) {
        float fire_elapsed = (float)glfwGetTime() - fireStartTime;

        // --- L GICA DE FIN DE INCENDIO ---
        // Comprueba si el tiempo total del incendio ha terminado
        if (fire_elapsed >= fireDuration) {
            isFireActive = false;
            std::cout << "El incendio ha terminado." << std::endl;
            // Limpieza: Cualquier  rbol que SIGA quem ndose, se convierte en tronco
            for (Chunk& chunk : terrain_chunks) {
                for (TreeInstance& tree : chunk.tree_instances) {
                    if (tree.state == TreeState::BURNING) {
                        tree.state = TreeState::CHOPPED_TWICE;
                    }
                }
            }
        }
        // --- L GICA DURANTE EL INCENDIO ---
        else {
            for (Chunk& chunk : terrain_chunks) {
                for (TreeInstance& tree : chunk.tree_instances) {

                    // Transici n 1: ALIVE/CHOPPED_ONCE -> BURNING
                    // Comprueba si es hora de que este  rbol comience a arder
                    if (tree.fireTriggerTime >= 0.0f && fire_elapsed >= tree.fireTriggerTime) {
                        if (tree.state == TreeState::ALIVE || tree.state == TreeState::CHOPPED_ONCE) {
                            if (tree.state == TreeState::ALIVE) {
                                for (Leaf& leaf : falling_leaves) {
                                    if (leaf.parentTreeId == tree.id) {
                                        leaf.is_active = false;
                                    }
                                }
                                g_currentLivingTrees--; // <-- PIERDE VIDA POR FUEGO
                            }
                            tree.state = TreeState::BURNING;
                            tree.fireTriggerTime = -1.0f; // Previene que se active de nuevo
                        }
                    }

                    // ---   L GICA CLAVE FALTANTE!! ---
                    // Transici n 2: BURNING -> CHOPPED_TWICE
                    // Comprueba si este  rbol ya termin  de quemarse (su tiempo individual)
                    if (tree.burnOutTime >= 0.0f && fire_elapsed >= tree.burnOutTime) {
                        if (tree.state == TreeState::BURNING) {
                            tree.state = TreeState::CHOPPED_TWICE;
                            tree.burnOutTime = -1.0f; // Previene que se active de nuevo
                        }
                    }
                }
            }
        }
    }

    // --- L gica de Hojas ---
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

    // --- L gica de Animales ---
    updateForestHealthAndAnimals(deltaTime);
}


int getRandomWalkIndex(int modelListLength) { //ELIGE INDICES PARES QUE CORRESPONDEN A ANIMACI N WALW

    if (modelListLength < 2) return 0;

    int maxAnimals = modelListLength / 2;

    std::uniform_int_distribution<int> dis(0, maxAnimals - 1);
    int randomAnimal = dis(gen);

    return randomAnimal * 2;
}


void generateForest() {
    if (g_runTestEnvironment) return;

    const float TERRAIN_AABB_HEIGHT = 35.0f;

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

            // Vector de pares (walk, idle) para cada tipo de animal
            std::vector<std::pair<AnimatedModel*, AnimatedModel*>> animalPairs = {
                {fa.character01, fa.character02}, // LOBO: character01=walk, character02=idle
                {fa.character03, fa.character04},  // CASTOR: character03=walk, character04=idle
                { fa.character05, fa.character06 }  // OSO NEGRO: character05=walk, character06=idle
                ,{ fa.character07, fa.character08 }  // ALCE: character07=walk, character08=idle
            };

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

            // --- GENERAR ANIMALES ---
            if (fa.character01 != nullptr && fa.character03 != nullptr) {
                if (dis_spawn_chance(gen) < ANIMAL_SPAWN_PROBABILITY) {
                    for (unsigned int i = 0; i < ANIMALS_PER_CHUNK; i++) {
                        AnimalInstance animal;
                        animal.position = chunk.position + glm::vec3(dis_pos(gen), 0.0f, dis_pos(gen));

                        if (!isPositionSafe(animal.position)) {
                            continue;
                        }

                        animal.rotationY = glm::radians(dis_rot(gen));
                        animal.state = AnimalState::IDLE;
                        animal.stateTimer = dis_ai_time(gen);
                        animal.targetPosition = animal.position;
                        animal.elapsedTime = 0.0f;
                        animal.animationCount = 0;

                        // ASIGNAR EL PAR COMPLETO (walk e idle) DEL MISMO TIPO
                        std::uniform_int_distribution<int> dis_species(0, animalPairs.size() - 1);
                        int selectedPair = dis_species(gen);

                        animal.walk = animalPairs[selectedPair].first;   // walk
                        animal.idle = animalPairs[selectedPair].second;  // idle DEL MISMO TIPO

                        // Pose inicial con la animaci n idle
                        if (animal.idle) {
                            animal.idle->SetPose(0.0f, animal.gBones);
                        }

                        g_animals.push_back(animal);
                    }
                }
            }
            // --- FIN GENERAR ANIMALES ---
        }
    }

    // Generaci n Nubes
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

    // --- Comprobaci n ---
    std::cout << "Se generaron " << g_animals.size() << " animales en el mundo." << std::endl;
    // --- FIN Comprobaci n ---
}

void generateCity() {
    //proximamente a implementar
}