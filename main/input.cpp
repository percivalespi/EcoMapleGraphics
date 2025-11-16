#include "input.h"
#include "mechanic.h"
#include "audio.h"

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

// --- NUEVO: Implementaci�n de la funci�n para pantalla completa ---
void toggleFullscreen(GLFWwindow* window) {
    if (!is_fullscreen) {
        // Guardar la posici�n y tama�o de la ventana
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

// --- NUEVO: Implementaci�n del callback de foco ---
void window_focus_callback(GLFWwindow* window, int focused)
{
    g_isWindowFocused = (focused == GLFW_TRUE);
    if (g_isWindowFocused)
    {
        // La ventana ha ganado el foco, capturar el cursor de nuevo
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        firstMouse = true; // Evita un salto brusco de la c�mara
    }
    else
    {
        // La ventana ha perdido el foco, liberar el cursor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}
// --- FIN NUEVO ---


// cuando el tama�o de la ventana cambia (por SO o por el usuario) esta funci�n de devoluci�n de llamada se ejecuta
// ---------------------------------------------------------------------------------------------

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    //Sube --> el volumen al presionar (U)
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        if (!plus_key_pressed) { 
            changeGlobalVolume(0.1f); // Sube 10%
            plus_key_pressed = true;  
        }
    }
    else {
        plus_key_pressed = false; 
    }

    //Baja --> al presionar (J)
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        if (!minus_key_pressed) { 
            changeGlobalVolume(-0.1f); // Baja 10%
            minus_key_pressed = true;   
        }
    }
    else {
        minus_key_pressed = false;     }


    // --- CAMBIO DE PERSONAJE (1 y 2) ---
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        g_activeCharacter = 1; // Seleccionar Demi
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        g_activeCharacter = 2; // Seleccionar Miku
    }

    // --- TOGGLE MODO C�MARA (TECLA M) ---
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        if (!g_pressM) {
            g_isThirdPerson = !g_isThirdPerson;
            g_pressM = true;
            if (g_isThirdPerson) {
                // Al entrar, ajustamos un poco la c�mara para ver al personaje
                camera.Pitch = -15.0f;
                camera.ProcessMouseMovement(0, 0); // Actualizar vectores
            }
        }
    }
    else {
        g_pressM = false;
    }

    if (!animacion1 && !menu) {

        if (g_isThirdPerson) {
            // =================================================
            // === MODO 3RA PERSONA (CON SUAVIZADO) ===
            // =================================================
            g_demiMoving = false;

            // 1. Vectores de c�mara (planos)
            glm::vec3 camFront = camera.Front;
            camFront.y = 0.0f;
            glm::vec3 camRight = camera.Right;
            camRight.y = 0.0f;

            if (glm::length(camFront) > 0.01f) camFront = glm::normalize(camFront);
            if (glm::length(camRight) > 0.01f) camRight = glm::normalize(camRight);

            glm::vec3 moveDir(0.0f);

            // 2. Input WASD
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir += camFront;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir -= camFront;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir -= camRight;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir += camRight;

            // 3. Movimiento y Rotaci�n SUAVE
            if (glm::length(moveDir) > 0.01f) {
                moveDir = glm::normalize(moveDir);

                // --- A. Movimiento ---
                g_demiPos += moveDir * DEMI_SPEED * deltaTime;
                g_demiMoving = true;

                // --- B. Rotaci�n Suave (Interpolaci�n) ---
                // Calculamos el �ngulo objetivo hacia donde queremos ir
                float targetRotation = atan2(moveDir.x, moveDir.z);

                // L�gica matem�tica para rotar suavemente sin dar vueltas locas (360->0)
                // Velocidad de giro (ajusta el 10.0f si quieres que gire m�s r�pido o lento)
                float rotationSpeed = 10.0f * deltaTime;

                // Calcular diferencia de �ngulos
                float angleDiff = targetRotation - g_demiRotY;

                // Ajustar para que tome el camino m�s corto
                const float PI = 3.14159265359f;
                if (angleDiff > PI) angleDiff -= 2.0f * PI;
                if (angleDiff < -PI) angleDiff += 2.0f * PI;

                // Aplicar la rotaci�n progresiva
                g_demiRotY += angleDiff * rotationSpeed;
            }

            // 4. Restricciones de altura
            if (g_demiPos.y < DEMI_OFFSET_Y) g_demiPos.y = DEMI_OFFSET_Y;

            // 5. C�mara Orbital SUAVE (LERP)
            // Calculamos d�nde DEBER�A estar la c�mara (Target)
            glm::vec3 targetCamPos = g_demiPos - (camera.Front * DEMI_CAM_DIST) + glm::vec3(0.0f, 5.0f, 0.0f);

            // En lugar de asignarlo directo, nos movemos un % hacia all� (0.1f es la "dureza" del resorte)
            // Cuanto m�s bajo el valor (ej 0.05f), m�s suave pero tardada. Cuanto m�s alto (0.5f), m�s r�gida.
            float cameraSmoothness = 15.0f * deltaTime;
            // Asegurar que no exceda 1.0 para evitar oscilaci�n
            if (cameraSmoothness > 1.0f) cameraSmoothness = 1.0f;

            camera.Position = glm::mix(camera.Position, targetCamPos, cameraSmoothness);

        }
        else {
            // ... (Aqu� sigue tu else de c�mara libre original) ...
            // =================================================
            // === MODO C�MARA LIBRE (Tu c�digo original) ===
            // =================================================
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                camera.ProcessKeyboard(FORWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                camera.ProcessKeyboard(BACKWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                camera.ProcessKeyboard(LEFT, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                camera.ProcessKeyboard(RIGHT, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                camera.ProcessKeyboard(UPWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                camera.ProcessKeyboard(DOWNWARD, deltaTime);
        }
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
        StartAnim1(/*posDestino*/ glm::vec3(-0.7, 3.5, 2.5),
            /*lookAtDestino*/ glm::vec3(0, 1.5, 0),   // o punto objetivo
            /*duración*/ 20.0f);
        Time = 222.0f;
    }
}

void StartAnim1(const glm::vec3& camTargetPos, const glm::vec3& camTargetLookAt, double durationSec)
{
    g_anim1.active = true;
    g_anim1.t0 = glfwGetTime();          // o std::chrono::steady_clock
    g_anim1.dur = durationSec;

    g_anim1.p0 = camera.Position;
    g_anim1.p1 = camTargetPos;

    camera.Front = glm::normalize(glm::vec3(-10.0f, 0.0f, 3.0f) - camera.Position);
   
    g_anim1.lookAt0 = camera.Position + camera.Front;  // punto de mira actual
    g_anim1.lookAt1 = camTargetLookAt;                 // punto de mira destino
}

static inline float Smooth01(float x) {
    x = glm::clamp(x, 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}

void UpdateAnim1(GLFWwindow* window)
{
    if (!g_anim1.active) return;

    double now = glfwGetTime();                        // reloj estable
    double u = (now - g_anim1.t0) / g_anim1.dur;     // progreso [0,∞)
    float  su = Smooth01((float)u);   // progreso suavizado [0,1]
    transparenciaC = su;

    // Interpola posición
    glm::vec3 pos = glm::mix(g_anim1.p0, g_anim1.p1, su);
    camera.Position = pos;

    // Interpola dirección de mirada hacia un punto objetivo (opcional)
    glm::vec3 lookAt = glm::mix(g_anim1.lookAt0, g_anim1.lookAt1, su);
    glm::vec3 dir = glm::normalize(lookAt - camera.Position);

    // Reconstruye la base de cámara (sin roll)
    glm::vec3 worldUp(0, 1, 0);
    camera.Front = dir;
    camera.Right = glm::normalize(glm::cross(camera.Front, worldUp));
    camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));

    // Fin de la animación
    if (u >= 1.0) {
        g_anim1.active = false;
        escena = 1;
        animacion1 = false;
        camera.Position = posicionCarga;
        StartMenu(/*lookAtDestino*/ glm::vec3(0.0f, -0.20f, -1.0f),   // o punto objetivo
            /*duración por línea*/ 1.0f);
        menu = true;
    }
}

void StartMenu(const glm::vec3& camTargetLookAt, double durationSec)
{
    g_menu.t0 = glfwGetTime();          // o std::chrono::steady_clock
    g_menu.dur = durationSec;

    g_menu.p0 = camera.Position;


    g_menu.lookAt = camTargetLookAt;  // punto de mira fijo
}

void Transicion(GLFWwindow* window) {
    
    if (DT1 < 1.0) {  
        double now = glfwGetTime(); // reloj estable
        double u = (now - g_menu.t0) / g_menu.dur;
        DT1 = Smooth01((float)u);
    }
    else {
        if (DT2 < 1.0){
            double now = glfwGetTime(); // reloj estable
            double u = (now - g_menu.t0 - g_menu.dur) / g_menu.dur; 
            DT2 = Smooth01((float)u);  
        }   
        else {
            if (DT3 < 1.0) {
                double now = glfwGetTime(); // reloj estable
                double u = (now - g_menu.t0 - 2.0f*g_menu.dur) / g_menu.dur;
                DT3 = Smooth01((float)u);
            }
            else {
                if (DT4 < 1.0){
                    double now = glfwGetTime(); // reloj estable
                    double u = (now - g_menu.t0 - 3.0f*g_menu.dur) / g_menu.dur;
                    DT4 = Smooth01((float)u); 
                }
                else {
                    if (DT5 < 1.0) {
                        double now = glfwGetTime(); // reloj estable
                        double u = (now - g_menu.t0 - 4.0f*g_menu.dur) / g_menu.dur;
                        DT5 = Smooth01((float)u);
                    }
                    else {
                        if (DT6 < 1.0) {
                            double now = glfwGetTime(); // reloj estable
                            double u = (now - g_menu.t0 - 5.0f*g_menu.dur) / g_menu.dur;
                            DT6 = Smooth01((float)u);
                        }
                        else {
                            if (DHoja < 1.3) {
                                double now = glfwGetTime(); // reloj estable
                                double u = (now - g_menu.t0 - 6.0f*g_menu.dur) / g_menu.dur;
                                DHoja = Smooth01((float)u)*1.4f;
                            }
                            else {
                                menu = false;
                                camera.Position = posicionEscenario1;
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
    // Reconstruye la base de cámara (sin roll)
    glm::vec3 worldUp(0, 1, 0);
    camera.Front = g_menu.lookAt;
    camera.Right = glm::normalize(glm::cross(camera.Front, worldUp));
    camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
}

