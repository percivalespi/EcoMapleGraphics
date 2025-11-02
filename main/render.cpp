#include "render.h"
#include "globals.h"

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

void renderScene() {
    glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 2000.0f);
    glm::mat4 view = camera.GetViewMatrix();
    cameraFrustum.extractPlanes(projection * view);

    float lightDistance = 200.0f;
    float sunElevRad = glm::radians(sunElevationAngle);
    float sunAngleRad = glm::radians(sunAngle);
    fa.theLight.Position = glm::vec3(lightDistance * cos(sunElevRad) * sin(sunAngleRad), lightDistance * sin(sunElevRad), lightDistance * cos(sunElevRad) * cos(sunAngleRad));
    float minDimFactor = 0.15f;
    float normalizedSunY = glm::clamp(fa.theLight.Position.y / lightDistance, -1.0f, 1.0f);
    float dayNightTransition = glm::smoothstep(-0.1f, 0.1f, normalizedSunY);
    float nightDimFactor = glm::mix(minDimFactor, 1.0f, dayNightTransition);
    glm::vec4 dimmedLightPower = fa.theLight.Power * nightDimFactor;
    isDay = (normalizedSunY > -0.1f);



    // --- 1. DIBUJAR OBJETOS OPACOS ESTÁTICOS ---
    if (phongShader != nullptr && phongShader->ID != 0) {
        phongShader->use();
        phongShader->setMat4("projection", projection);
        phongShader->setMat4("view", view);
        phongShader->setVec3("lightPosition", fa.theLight.Position);
        phongShader->setVec4("LightColor", fa.theLight.Color);
        phongShader->setVec4("LightPower", dimmedLightPower);
        phongShader->setInt("alphaIndex", fa.theLight.alphaIndex);

        if (fa.terrain_model != nullptr) {
            phongShader->setBool("isTerrain", true);
            phongShader->setVec4("MaterialAmbientColor", fa.defaultMaterial.ambient);
            phongShader->setVec4("MaterialSpecularColor", fa.defaultMaterial.specular);
            phongShader->setFloat("transparency", fa.defaultMaterial.transparency);
            for (const auto& chunk : terrain_chunks) {
                if (cameraFrustum.isBoxInFrustum(chunk.aabb_min, chunk.aabb_max)) {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, chunk.position);
                    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    phongShader->setMat4("model", model);
                    fa.terrain_model->Draw(*phongShader);
                }
            }
        }
        if (fa.mountain_model != nullptr) {
            phongShader->setBool("isTerrain", false);
            phongShader->setVec4("MaterialAmbientColor", fa.mountainMaterial.ambient);
            phongShader->setVec4("MaterialSpecularColor", fa.mountainMaterial.specular);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 11.75f, 170.0f));
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(18.45f, 18.45f, 8.865f));
            phongShader->setMat4("model", model);
            fa.mountain_model->Draw(*phongShader);
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
    if (instancePhongShader != nullptr && instancePhongShader->ID != 0 && fa.rock_model != nullptr && !visible_rocks.empty()) {
        instancePhongShader->use();
        instancePhongShader->setMat4("projection", projection);
        instancePhongShader->setMat4("view", view);
        instancePhongShader->setVec3("lightPosition", fa.theLight.Position);
        instancePhongShader->setVec4("LightColor", fa.theLight.Color);
        instancePhongShader->setVec4("LightPower", dimmedLightPower);
        instancePhongShader->setInt("alphaIndex", fa.theLight.alphaIndex);
        instancePhongShader->setVec4("MaterialAmbientColor", fa.defaultMaterial.ambient);
        instancePhongShader->setVec4("MaterialSpecularColor", fa.defaultMaterial.specular);
        instancePhongShader->setFloat("transparency", fa.defaultMaterial.transparency);

        glBindBuffer(GL_ARRAY_BUFFER, fa.rockInstanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, visible_rocks.size() * sizeof(glm::mat4), &visible_rocks[0]);

        for (unsigned int i = 0; i < fa.rock_model->meshes.size(); i++) {
            if (!fa.rock_model->meshes[i].textures.empty()) {
                glActiveTexture(GL_TEXTURE0);
                instancePhongShader->setInt("texture_diffuse1", 0);
                glBindTexture(GL_TEXTURE_2D, fa.rock_model->meshes[i].textures[0].id);
            }
            else {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glBindVertexArray(fa.rock_model->meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(fa.rock_model->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(visible_rocks.size()));
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
        instanceAlphaTestPhongShader->setVec3("lightPosition", fa.theLight.Position);
        instanceAlphaTestPhongShader->setVec4("LightColor", fa.theLight.Color);
        instanceAlphaTestPhongShader->setVec4("LightPower", dimmedLightPower);
        instanceAlphaTestPhongShader->setInt("alphaIndex", fa.theLight.alphaIndex);
        instanceAlphaTestPhongShader->setFloat("transparency", 1.0f);

        // Árboles Vivos
        if (fa.tree_model != nullptr && !visible_trees_alive.empty()) {
            instanceAlphaTestPhongShader->setVec4("MaterialAmbientColor", fa.treeMaterial.ambient);
            instanceAlphaTestPhongShader->setVec4("MaterialSpecularColor", fa.treeMaterial.specular);
            glBindBuffer(GL_ARRAY_BUFFER, fa.treeInstanceVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, visible_trees_alive.size() * sizeof(glm::mat4), &visible_trees_alive[0]);
            for (unsigned int i = 0; i < fa.tree_model->meshes.size(); i++) {
                if (!fa.tree_model->meshes[i].textures.empty()) {
                    glActiveTexture(GL_TEXTURE0);
                    instanceAlphaTestPhongShader->setInt("texture_diffuse1", 0);
                    glBindTexture(GL_TEXTURE_2D, fa.tree_model->meshes[i].textures[0].id);
                }
                else {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
                glBindVertexArray(fa.tree_model->meshes[i].VAO);
                glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(fa.tree_model->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(visible_trees_alive.size()));
                glBindVertexArray(0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        // Pasto
        if (fa.grass_model != nullptr && !visible_grass.empty()) {
            instanceAlphaTestPhongShader->setVec4("MaterialAmbientColor", fa.defaultMaterial.ambient);
            instanceAlphaTestPhongShader->setVec4("MaterialSpecularColor", glm::vec4(0.0f));
            glBindBuffer(GL_ARRAY_BUFFER, fa.grassInstanceVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, visible_grass.size() * sizeof(glm::mat4), &visible_grass[0]);
            for (unsigned int i = 0; i < fa.grass_model->meshes.size(); i++) {
                if (!fa.grass_model->meshes[i].textures.empty()) {
                    glActiveTexture(GL_TEXTURE0);
                    instanceAlphaTestPhongShader->setInt("texture_diffuse1", 0);
                    glBindTexture(GL_TEXTURE_2D, fa.grass_model->meshes[i].textures[0].id);
                }
                else {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
                glBindVertexArray(fa.grass_model->meshes[i].VAO);
                glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(fa.grass_model->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(visible_grass.size()));
                glBindVertexArray(0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // Nubes
        if (fa.cloud_model != nullptr && !cloud_matrices.empty()) {
            instanceAlphaTestPhongShader->setVec4("MaterialAmbientColor", fa.cloudMaterial.ambient);
            instanceAlphaTestPhongShader->setVec4("MaterialSpecularColor", fa.cloudMaterial.specular);
            glBindBuffer(GL_ARRAY_BUFFER, fa.cloudInstanceVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, cloud_matrices.size() * sizeof(glm::mat4), &cloud_matrices[0]);
            for (unsigned int i = 0; i < fa.cloud_model->meshes.size(); i++) {
                if (!fa.cloud_model->meshes[i].textures.empty()) {
                    glActiveTexture(GL_TEXTURE0);
                    instanceAlphaTestPhongShader->setInt("texture_diffuse1", 0);
                    glBindTexture(GL_TEXTURE_2D, fa.cloud_model->meshes[i].textures[0].id);
                }
                else {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
                glBindVertexArray(fa.cloud_model->meshes[i].VAO);
                glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(fa.cloud_model->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(cloud_matrices.size()));
                glBindVertexArray(0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // Hojas (Activas)
        if (fa.leaf_model != nullptr && !leaf_matrices.empty()) {
            instanceAlphaTestPhongShader->setVec4("MaterialAmbientColor", fa.leafMaterial.ambient);
            instanceAlphaTestPhongShader->setVec4("MaterialSpecularColor", fa.leafMaterial.specular);
            glBindBuffer(GL_ARRAY_BUFFER, fa.leafInstanceVBO);

            GLint buffer_size = 0;
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);
            GLsizeiptr data_size_needed = leaf_matrices.size() * sizeof(glm::mat4);
            GLsizei count_to_draw = static_cast<GLsizei>(leaf_matrices.size());

            if (data_size_needed > (GLsizeiptr)buffer_size) {
                std::cerr << "WARNING: leafInstanceVBO resizing needed! Current: " << buffer_size << ", Needed: " << data_size_needed << std::endl;
                glBufferData(GL_ARRAY_BUFFER, data_size_needed, &leaf_matrices[0], GL_DYNAMIC_DRAW);
                setupInstanceVBO(fa.leafInstanceVBO, data_size_needed / sizeof(glm::mat4), fa.leaf_model);
            }
            else if (data_size_needed > 0) {
                glBufferSubData(GL_ARRAY_BUFFER, 0, data_size_needed, &leaf_matrices[0]);
            }
            else {
                count_to_draw = 0;
            }

            if (count_to_draw > 0) {
                for (unsigned int i = 0; i < fa.leaf_model->meshes.size(); i++) {
                    if (!fa.leaf_model->meshes[i].textures.empty()) {
                        glActiveTexture(GL_TEXTURE0);
                        instanceAlphaTestPhongShader->setInt("texture_diffuse1", 0);
                        glBindTexture(GL_TEXTURE_2D, fa.leaf_model->meshes[i].textures[0].id);
                    }
                    else {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, 0);
                    }
                    glBindVertexArray(fa.leaf_model->meshes[i].VAO);
                    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(fa.leaf_model->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, count_to_draw);
                    glBindVertexArray(0);
                }
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    // --- 3.5 DIBUJAR ÁRBOLES GOLPEADOS UNA VEZ ---
    if (instancePhongShader != nullptr && instancePhongShader->ID != 0 && fa.chopped_once_model != nullptr && !visible_trees_chopped_once.empty()) {
        instancePhongShader->use();
        instancePhongShader->setMat4("projection", projection);
        instancePhongShader->setMat4("view", view);
        instancePhongShader->setVec3("lightPosition", fa.theLight.Position);
        instancePhongShader->setVec4("LightColor", fa.theLight.Color);
        instancePhongShader->setVec4("LightPower", dimmedLightPower);
        instancePhongShader->setInt("alphaIndex", fa.theLight.alphaIndex);
        instancePhongShader->setVec4("MaterialAmbientColor", fa.treeMaterial.ambient * 0.7f);
        instancePhongShader->setVec4("MaterialSpecularColor", glm::vec4(0.0f));
        instancePhongShader->setFloat("transparency", 1.0f);

        glBindBuffer(GL_ARRAY_BUFFER, fa.choppedOnceTreeInstanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, visible_trees_chopped_once.size() * sizeof(glm::mat4), &visible_trees_chopped_once[0]);

        for (unsigned int i = 0; i < fa.chopped_once_model->meshes.size(); i++) {
            if (!fa.chopped_once_model->meshes[i].textures.empty()) {
                glActiveTexture(GL_TEXTURE0);
                instancePhongShader->setInt("texture_diffuse1", 0);
                glBindTexture(GL_TEXTURE_2D, fa.chopped_once_model->meshes[i].textures[0].id);
            }
            else {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glBindVertexArray(fa.chopped_once_model->meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(fa.chopped_once_model->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(visible_trees_chopped_once.size()));
            glBindVertexArray(0);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // --- 3.6 DIBUJAR ÁRBOLES QUEMÁNDOSE ---
    if (instancePhongShader != nullptr && instancePhongShader->ID != 0 && fa.burning_tree_model != nullptr && !visible_trees_burning.empty()) {
        instancePhongShader->use();
        instancePhongShader->setMat4("projection", projection);
        instancePhongShader->setMat4("view", view);
        instancePhongShader->setVec3("lightPosition", fa.theLight.Position);
        instancePhongShader->setVec4("LightColor", fa.theLight.Color);
        instancePhongShader->setVec4("LightPower", dimmedLightPower);
        instancePhongShader->setInt("alphaIndex", fa.theLight.alphaIndex);
        instancePhongShader->setVec4("MaterialAmbientColor", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
        instancePhongShader->setVec4("MaterialSpecularColor", glm::vec4(0.0f));
        instancePhongShader->setFloat("transparency", 1.0f);

        glBindBuffer(GL_ARRAY_BUFFER, fa.burningTreeInstanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, visible_trees_burning.size() * sizeof(glm::mat4), &visible_trees_burning[0]);

        for (unsigned int i = 0; i < fa.burning_tree_model->meshes.size(); i++) {
            if (!fa.burning_tree_model->meshes[i].textures.empty()) {
                glActiveTexture(GL_TEXTURE0);
                instancePhongShader->setInt("texture_diffuse1", 0);
                glBindTexture(GL_TEXTURE_2D, fa.burning_tree_model->meshes[i].textures[0].id);
            }
            else {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glBindVertexArray(fa.burning_tree_model->meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(fa.burning_tree_model->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(visible_trees_burning.size()));
            glBindVertexArray(0);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // --- 3.7 DIBUJAR ÁRBOLES GOLPEADOS DOS VECES ---
    if (instancePhongShader != nullptr && instancePhongShader->ID != 0 && fa.chopped_twice_model != nullptr && !visible_trees_chopped_twice.empty()) {
        instancePhongShader->use();
        instancePhongShader->setMat4("projection", projection);
        instancePhongShader->setMat4("view", view);
        instancePhongShader->setVec3("lightPosition", fa.theLight.Position);
        instancePhongShader->setVec4("LightColor", fa.theLight.Color);
        instancePhongShader->setVec4("LightPower", dimmedLightPower);
        instancePhongShader->setInt("alphaIndex", fa.theLight.alphaIndex);
        instancePhongShader->setVec4("MaterialAmbientColor", fa.treeMaterial.ambient * 0.4f);
        instancePhongShader->setVec4("MaterialSpecularColor", glm::vec4(0.0f));
        instancePhongShader->setFloat("transparency", 1.0f);

        glBindBuffer(GL_ARRAY_BUFFER, fa.choppedTwiceTreeInstanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, visible_trees_chopped_twice.size() * sizeof(glm::mat4), &visible_trees_chopped_twice[0]);

        for (unsigned int i = 0; i < fa.chopped_twice_model->meshes.size(); i++) {
            if (!fa.chopped_twice_model->meshes[i].textures.empty()) {
                glActiveTexture(GL_TEXTURE0);
                instancePhongShader->setInt("texture_diffuse1", 0);
                glBindTexture(GL_TEXTURE_2D, fa.chopped_twice_model->meshes[i].textures[0].id);
            }
            else {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glBindVertexArray(fa.chopped_twice_model->meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(fa.chopped_twice_model->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(visible_trees_chopped_twice.size()));
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
        Model* currentSkybox = isDay ? fa.cubeenv : fa.cubeenv_noche;
        if (currentSkybox != nullptr) {
            currentSkybox->Draw(*skyboxShader);
        }
        glDepthFunc(GL_LESS);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // --- 5. DIBUJAR EL SOL ---
    if (sunShader != nullptr && sunShader->ID != 0 && fa.sun_model != nullptr) {
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
        fa.sun_model->Draw(*sunShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // --- 6. DIBUJAR LA LUNA ---
    if (sunShader != nullptr && sunShader->ID != 0 && fa.moon_model != nullptr) {
        if (fa.sun_model == nullptr || !(sunShader && sunShader->ID != 0)) {
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
        fa.moon_model->Draw(*sunShader);
        glDepthMask(GL_TRUE);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else {
        glDepthMask(GL_TRUE);
    }

}

void renderUI() {
    // --- 7. DIBUJAR LA CRUZ (CROSSHAIR) ---
    glDisable(GL_DEPTH_TEST);
    if (crosshairShader != nullptr && crosshairShader->ID != 0 && ui.crosshairVAO != 0) {
        crosshairShader->use();
        crosshairShader->setVec4("crosshairColor", glm::vec4(1.0f, 1.0f, 1.0f, 0.8f));
        glBindVertexArray(ui.crosshairVAO);
        glLineWidth(2.0f);
        glDrawArrays(GL_LINES, 0, 4);
        glBindVertexArray(0);
        glLineWidth(1.0f);
    }
    // glDisable(GL_DEPTH_TEST) se queda desactivado para la UI

    // --- 8. DIBUJAR UI (--- SECCIÓN COMPLETAMENTE MODIFICADA ---) ---
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    if (uiShader != nullptr && uiShader->ID != 0 && ui.uiVAO != 0) {
        uiShader->use();
        glm::mat4 projection_ortho = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
        uiShader->setMat4("projection", projection_ortho);
        uiShader->setInt("texture_diffuse1", 0); // Asignar sampler a textura 0

        glBindVertexArray(ui.uiVAO);

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

        if (ui.treeTextureID != 0) {
            // MODIFICADO: Posición Y
            glm::vec3 tree_icon_pos(tree_icon_x_pos, y_pos_icon, 0.0f);
            glm::mat4 model_tree = glm::mat4(1.0f);
            model_tree = glm::translate(model_tree, tree_icon_pos);
            model_tree = glm::scale(model_tree, glm::vec3(icon_size, icon_size, 1.0f));
            uiShader->setMat4("model", model_tree);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ui.treeTextureID);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Dibujar borde si P está presionada
            if (p_key_pressed && ui.highlightTextureID != 0) {
                glBindTexture(GL_TEXTURE_2D, ui.highlightTextureID);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        // --- NUEVO: Dibujar leyenda "P" (ABAJO) ---
        if (ui.legendTreeTextureID != 0) {
            glm::vec3 legend_tree_pos(tree_icon_x_pos, y_pos_legend, 0.0f);
            glm::mat4 model_legend_tree = glm::mat4(1.0f);
            model_legend_tree = glm::translate(model_legend_tree, legend_tree_pos);
            model_legend_tree = glm::scale(model_legend_tree, glm::vec3(legend_width, legend_height, 1.0f));
            uiShader->setMat4("model", model_legend_tree);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ui.legendTreeTextureID);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        // ---------------------------------------------


        // --- Icono de Fuego (F/G) ---
        // MODIFICADO: Mover el X_pos aquí para reusarlo
        float fire_icon_x_pos = SCR_WIDTH - icon_size - padding;

        if (ui.fireTextureID != 0) {
            // MODIFICADO: Posición Y
            glm::vec3 fire_icon_pos(fire_icon_x_pos, y_pos_icon, 0.0f);
            glm::mat4 model_fire = glm::mat4(1.0f);
            model_fire = glm::translate(model_fire, fire_icon_pos);
            model_fire = glm::scale(model_fire, glm::vec3(icon_size, icon_size, 1.0f));
            uiShader->setMat4("model", model_fire);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ui.fireTextureID);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Dibujar borde si el incendio está activo
            if (isFireActive && ui.highlightTextureID != 0) {
                glBindTexture(GL_TEXTURE_2D, ui.highlightTextureID);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        // --- NUEVO: Dibujar leyenda "F/G" (ABAJO) ---
        if (ui.legendFireTextureID != 0) {
            glm::vec3 legend_fire_pos(fire_icon_x_pos, y_pos_legend, 0.0f);
            glm::mat4 model_legend_fire = glm::mat4(1.0f);
            model_legend_fire = glm::translate(model_legend_fire, legend_fire_pos);
            model_legend_fire = glm::scale(model_legend_fire, glm::vec3(legend_width, legend_height, 1.0f));
            uiShader->setMat4("model", model_legend_fire);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ui.legendFireTextureID);
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
}




void initializeRenderBuffers(UIAssets& ui) {
    // --- Configuración Cruz ---
    float crosshairSize = 0.03f;
    float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    float crosshairVertices[] = {
        -crosshairSize / aspectRatio, 0.0f,
         crosshairSize / aspectRatio, 0.0f,
         0.0f, -crosshairSize,
         0.0f,  crosshairSize

    };

    glGenVertexArrays(1, &ui.crosshairVAO);
    glGenBuffers(1, &ui.crosshairVBO);
    glBindVertexArray(ui.crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, ui.crosshairVBO);
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
    glGenVertexArrays(1, &ui.uiVAO);
    glGenBuffers(1, &ui.uiVBO);
    glBindVertexArray(ui.uiVAO);
    glBindBuffer(GL_ARRAY_BUFFER, ui.uiVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uiQuadVertices), &uiQuadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

}


void initializeInstanceBuffers(ForestAssets& fa) {
    // Configurar VBOs Instancias
    size_t max_initial_trees = WORLD_SIZE * WORLD_SIZE * TREES_PER_CHUNK;
    size_t max_plantable_trees = 200;
    size_t max_total_trees = max_initial_trees + max_plantable_trees;
    size_t initial_leaves_count = falling_leaves.size();
    size_t max_explosion_leaves = max_total_trees * EXPLOSION_LEAVES_PER_HIT;
    size_t max_total_leaves = initial_leaves_count + max_explosion_leaves;

    setupInstanceVBO(fa.grassInstanceVBO, WORLD_SIZE * WORLD_SIZE * GRASS_PER_CHUNK, fa.grass_model);
    setupInstanceVBO(fa.rockInstanceVBO, WORLD_SIZE * WORLD_SIZE * ROCKS_PER_CHUNK, fa.rock_model);
    setupInstanceVBO(fa.treeInstanceVBO, max_total_trees, fa.tree_model);
    
    if (fa.chopped_once_model != nullptr) {
        setupInstanceVBO(fa.choppedOnceTreeInstanceVBO, max_total_trees, fa.chopped_once_model);
    }
    if (fa.burning_tree_model != nullptr) {
        setupInstanceVBO(fa.burningTreeInstanceVBO, max_total_trees, fa.burning_tree_model);
    }
    if (fa.chopped_twice_model != nullptr) {
        setupInstanceVBO(fa.choppedTwiceTreeInstanceVBO, max_total_trees, fa.chopped_twice_model);
    }
    
    setupInstanceVBO(fa.cloudInstanceVBO, TOTAL_CLOUDS, fa.cloud_model);

    std::cout << "Initial leaves: " << initial_leaves_count << ", Max total leaves (VBO): " << max_total_leaves << std::endl;
    leaf_matrices.reserve(max_total_leaves); 
    setupInstanceVBO(fa.leafInstanceVBO, max_total_leaves, fa.leaf_model);
}

