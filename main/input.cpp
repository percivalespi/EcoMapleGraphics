#include "input.h"
#include "mechanic.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // --- NUEVO: Actualizar dimensiones globales y viewport ---
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    // --- FIN NUEVO ---
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }
    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll((float)yoffset);
}

#include "input.h"

// --- NUEVO: Implementación de la función para pantalla completa ---
void toggleFullscreen(GLFWwindow* window) {
    if (!is_fullscreen) {
        // Guardar la posición y tamaño de la ventana
        glfwGetWindowPos(window, &last_window_x, &last_window_y);
        glfwGetWindowSize(window, &last_window_width, &last_window_height);

        // Cambiar a modo de pantalla completa
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        is_fullscreen = true;
    }
    else {
        // Restaurar la ventana al modo de ventana
        glfwSetWindowMonitor(window, NULL, last_window_x, last_window_y, last_window_width, last_window_height, 0);
        is_fullscreen = false;
    }
}

// --- NUEVO: Implementación del callback de foco ---
void window_focus_callback(GLFWwindow* window, int focused)
{
    g_isWindowFocused = (focused == GLFW_TRUE);
    if (g_isWindowFocused)
    {
        // La ventana ha ganado el foco, capturar el cursor de nuevo
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        firstMouse = true; // Evita un salto brusco de la cámara
    }
    else
    {
        // La ventana ha perdido el foco, liberar el cursor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}
// --- FIN NUEVO ---


// cuando el tamaño de la ventana cambia (por SO o por el usuario) esta función de devolución de llamada se ejecuta
// ---------------------------------------------------------------------------------------------

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    static bool f11_pressed = false;
    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS && !f11_pressed) {
        toggleFullscreen(window);
        f11_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_RELEASE) {
        f11_pressed = false;
    }
    if (!animacion1 && !menu) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.ProcessKeyboard(FORWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.ProcessKeyboard(LEFT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.ProcessKeyboard(RIGHT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera.ProcessKeyboard(UPWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            camera.ProcessKeyboard(DOWNWARD, deltaTime);
        }

        if (escena == 1) {
            // --- Control Tecla P (Plantar) ---
            if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
                if (!p_key_pressed) {
                    plantTree();
                    p_key_pressed = true;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
                p_key_pressed = false;
            }

            // --- Control Tecla F (Iniciar Incendio) ---
            if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
                if (!f_key_pressed) {
                    startFire();
                    f_key_pressed = true;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
                f_key_pressed = false;
            }

            // --- Control Tecla G (Detener Incendio) ---
            if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
                if (!g_key_pressed) {
                    if (isFireActive) {
                        isFireActive = false;
                        std::cout << "?Incendio detenido manualmente!" << std::endl;
                    }
                    g_key_pressed = true;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
                g_key_pressed = false;
            }

            float horizontalSpeed = 50.0f;
            float verticalSpeed = 40.0f;
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                sunAngle += horizontalSpeed * deltaTime;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                sunAngle -= horizontalSpeed * deltaTime;
            }
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                sunElevationAngle += verticalSpeed * deltaTime;
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                sunElevationAngle -= verticalSpeed * deltaTime;
            }
            sunAngle = fmod(sunAngle, 360.0f);
            if (sunAngle < 0.0f) {
                sunAngle += 360.0f;
            }
            sunElevationAngle = fmod(sunElevationAngle, 360.0f);
            if (sunElevationAngle < 0.0f) {
                sunElevationAngle += 360.0f;
            }
            if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
                if (temperatura < 40.00)temperatura += 0.35f;
                if (temperatura > 0)calor = true;
            }
            if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
                if ((temperatura > -40.00 && !calor) || (temperatura > 0.00 && calor))temperatura -= 0.35f;
            }
        }
       
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
            Time += 0.05f;
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            Time -= 0.05f;
        }
    }
    
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 2000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::vec4 ray_clip(0.0f, 0.0f, -1.0, 1.0);
        glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
        glm::vec3 ray_wor = glm::normalize(glm::vec3(glm::inverse(view) * ray_eye));
        glm::vec3 ray_origin = camera.Position;

        float closest_world_dist = std::numeric_limits<float>::max();
        int hit_chunk_idx = -1;
        size_t hit_tree_idx = -1;
        int hit_tree_id = -1;
        TreeState hit_tree_state = TreeState::ALIVE;
        const float max_hit_distance = 20.0f;

        for (int c_idx = 0; c_idx < terrain_chunks.size(); ++c_idx) {
            if (glm::distance(camera.Position, terrain_chunks[c_idx].position) > CHUNK_SIZE * 1.5f + max_hit_distance) {
                continue;
            }

            for (size_t t_idx = 0; t_idx < terrain_chunks[c_idx].tree_instances.size(); ++t_idx) {
                const auto& tree_instance = terrain_chunks[c_idx].tree_instances[t_idx];
                if (tree_instance.state == TreeState::CHOPPED_TWICE) {
                    continue;
                }

                glm::mat4 inv_model = glm::inverse(tree_instance.matrix);
                glm::vec3 local_ray_origin = glm::vec3(inv_model * glm::vec4(ray_origin, 1.0f));
                glm::vec3 local_ray_dir = glm::normalize(glm::vec3(inv_model * glm::vec4(ray_wor, 0.0f)));

                float t_local;
                if (intersectRayAABB(local_ray_origin, local_ray_dir, tree_trunk_aabb_min, tree_trunk_aabb_max, t_local)) {
                    glm::vec3 world_hit_point = glm::vec3(tree_instance.matrix * glm::vec4(local_ray_origin + local_ray_dir * t_local, 1.0f));
                    float world_distance = glm::distance(ray_origin, world_hit_point);

                    if (world_distance < max_hit_distance && world_distance < closest_world_dist) {
                        // closest_t_local = t_local; // No se usa t_local para comparar
                        closest_world_dist = world_distance;
                        hit_chunk_idx = c_idx;
                        hit_tree_idx = t_idx;
                        hit_tree_id = tree_instance.id;
                        hit_tree_state = tree_instance.state;
                    }
                }
            }
        }

        if (hit_tree_id != -1) {
            std::cout << "?Arbol golpeado! ID: " << hit_tree_id << " Estado: ";
            if (hit_chunk_idx >= 0 && hit_tree_idx < terrain_chunks[hit_chunk_idx].tree_instances.size()) {
                TreeInstance& hit_tree = terrain_chunks[hit_chunk_idx].tree_instances[hit_tree_idx];

                if (hit_tree_state == TreeState::ALIVE && hit_tree.state == TreeState::ALIVE) {
                    std::cout << "ALIVE -> CHOPPED_ONCE" << std::endl;
                    hit_tree.state = TreeState::CHOPPED_ONCE;

                    g_currentLivingTrees--; // <-- PIERDE VIDA POR TALA

                    int leaves_deactivated = 0;
                    for (Leaf& leaf : falling_leaves) {
                        if (leaf.parentTreeId == hit_tree_id) {
                            leaf.is_active = false;
                            leaves_deactivated++;
                        }
                    }
                    std::cout << " -> Hojas originales desactivadas: " << leaves_deactivated << std::endl;

                    glm::vec3 hit_tree_base_pos = glm::vec3(hit_tree.matrix[3]);
                    std::cout << " -> Generando " << EXPLOSION_LEAVES_PER_HIT << " hojas de estallido..." << std::endl;

                    for (int i = 0; i < EXPLOSION_LEAVES_PER_HIT; ++i) {
                        Leaf explosion_leaf;
                        explosion_leaf.parentTreeId = hit_tree_id;
                        explosion_leaf.is_active = true;
                        explosion_leaf.is_explosion_leaf = true;

                        float angle = glm::radians(dis_explode_angle(gen));
                        float radius = dis_explode_radius(gen);
                        float spawn_x = hit_tree_base_pos.x + radius * cos(angle);
                        float spawn_z = hit_tree_base_pos.z + radius * sin(angle);
                        float spawn_y = hit_tree_base_pos.y + dis_explode_y(gen);

                        explosion_leaf.position = glm::vec3(spawn_x, spawn_y, spawn_z);
                        explosion_leaf.initialTreePos = glm::vec3(hit_tree_base_pos.x, 0.0f, hit_tree_base_pos.z);
                        explosion_leaf.initialHeight = spawn_y;

                        explosion_leaf.rotationAxis = glm::normalize(glm::vec3(dis_axis(gen), dis_axis(gen), dis_axis(gen)));
                        if (glm::length(explosion_leaf.rotationAxis) < 0.01f) {
                            explosion_leaf.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
                        }
                        explosion_leaf.rotationAngle = dis_rot(gen);
                        explosion_leaf.fallSpeed = dis_explode_fall(gen);
                        explosion_leaf.spinSpeed = dis_explode_spin(gen);

                        falling_leaves.push_back(explosion_leaf);
                    }

                }
                else if (hit_tree_state == TreeState::CHOPPED_ONCE && hit_tree.state == TreeState::CHOPPED_ONCE) {
                    std::cout << "CHOPPED_ONCE -> CHOPPED_TWICE" << std::endl;
                    hit_tree.state = TreeState::CHOPPED_TWICE;
                }
                else if (hit_tree_state == TreeState::BURNING && hit_tree.state == TreeState::BURNING) {
                    std::cout << "BURNING -> CHOPPED_TWICE" << std::endl;
                    hit_tree.state = TreeState::CHOPPED_TWICE;
                }
            }
            else {
                std::cerr << "ERROR: Indices de árbol golpeado inválidos!" << std::endl;
            }
        }
    }
}

void CalculoCamara(GLFWwindow* window) {
    posicionActual = camera.Position;
    float distancia = glm::length(posicionActual - posicionOrigen);
    if (distancia <= 6) {
        animacion1 = true;
        camera.Position = posicionA1;
        Time = 222.0f;
    }
}

void Animacion1(GLFWwindow* window) {
    transparenciaC += 0.00015f;
    posicionActual = camera.Position + glm::vec3(0.0f, -avanceA1, -avanceA1);
    camera.Position = posicionActual;
    float distancia = glm::length(posicionActual - posicionOrigen);
    if (distancia <= 3) {
        menu = true;
        camera.Position = posicionCarga;
        escena = 1;
        animacion1 = false;
    }

}

void Trancision() {
    if (DT1 < 1.0)DT1 += velocidadCarga;
    else {
        if (DT2 < 1.0)DT2 += velocidadCarga;
        else {
            if (DT3 < 1.0)DT3 += velocidadCarga;
            else {
                if (DT4 < 1.0)DT4 += velocidadCarga;
                else {
                    if (DT5 < 1.0)DT5 += velocidadCarga;
                    else {
                        if (DT6 < 1.0)DT6 += velocidadCarga;
                        else {
                            if (DHoja < 1.3)DHoja += velocidadCarga;
                            else {
                                menu = false;
                                camera.Position = posicionEscenario1;
                            }
                        }
                    }
                }
            }
        }
    }
}