#include "render.h"
#include "globals.h" //Angel no me pegues

/* -------------------------------------------- - Manejo de Recursos TEST------------------------------------------*/

// --- HELPER 1: Configuración básica ---
static void setupStaticShader(Shader* shader, const glm::mat4& projection, const glm::mat4& view) {
    shader->use();
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
}

// --- HELPER 2: Luces (Envía TODO, el shader decidirá qué usar) ---
static void setupShaderLights(Shader* shader, const std::vector<Light>& lights) {
    shader->setInt("numLights", (int)lights.size());
    for (size_t i = 0; i < lights.size(); ++i) {
        std::string base = "allLights[" + std::to_string(i) + "]";
        shader->setVec3(base + ".Position", lights[i].Position);
        shader->setVec3(base + ".Direction", lights[i].Direction);
        shader->setVec4(base + ".Color", lights[i].Color);
        shader->setVec4(base + ".Power", lights[i].Power);
        shader->setInt(base + ".alphaIndex", lights[i].alphaIndex);
        shader->setFloat(base + ".distance", lights[i].distance);
    }
}

// --- HELPER 3: Dibujado (Envía TODO el material) ---
static void drawObject(Shader* shader, Model* model, const Material& material, const glm::mat4& modelMatrix) {
    if (material.transparency < 1.0f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    shader->setMat4("model", modelMatrix);
    shader->setVec4("MaterialAmbientColor", material.ambient);
    shader->setVec4("MaterialDiffuseColor", material.diffuse);
    // Estos dos se enviarán siempre. Lambert los ignorará, Phong los usará.
    shader->setVec4("MaterialSpecularColor", material.specular);
    shader->setFloat("transparency", material.transparency);

    model->Draw(*shader);

    if (material.transparency < 1.0f) {
        glDisable(GL_BLEND);
    }
}

void renderTestEnvironment(const glm::mat4& projection, const glm::mat4& view) {
    if (skyboxShader != nullptr && skyboxShader->ID != 0) {
        glDepthFunc(GL_LEQUAL);
        skyboxShader->use();
        skyboxShader->setMat4("projection", projection);
        glm::mat4 view_skybox = glm::mat4(glm::mat3(view));
        skyboxShader->setMat4("view", view_skybox);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1000.0f));
        Model* currentSkybox = isDay ? fa.cubeenv : fa.cubeenv_noche;
        if (currentSkybox != nullptr) {
            currentSkybox->Draw(*skyboxShader);
        }
        glDepthFunc(GL_LESS);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

    }
    setupStaticShader(mLightsShader, projection, view);
    setupShaderLights(mLightsShader, gLights);

    // Coche (Metálico -> Phong)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, (-WORLD_SIZE * 35) - 2.5));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

    drawObject(mLightsShader, ta.suelo, ta.asphalt, model);
    drawObject(mLightsShader, ta.suelo_verde, ta.cesped, model);
    drawObject(mLightsShader, ta.metales, ta.steel, model);
    drawObject(mLightsShader, ta.objMadera, ta.madera, model);
    //drawObject(mLightsShader, ta.objCristales, ta.traslucido, model);
    drawObject(mLightsShader, ta.objPlasticos, ta.plastic, model);
    drawObject(mLightsShader, ta.objConcreto, ta.concreto, model);
    drawObject(mLightsShader, ta.objLlantas, ta.goma, model);
    drawObject(mLightsShader, ta.objLadrillo, ta.ladrillo, model);
    drawObject(mLightsShader, ta.grafitis, ta.plastic, model);
    drawObject(mLightsShader, ta.bandera, ta.tela, model);
    drawObject(mLightsShader, ta.fabrica, ta.concreto, model);
    drawObject(mLightsShader, ta.rascacielos, ta.concreto, model);
    drawObject(mLightsShader, ta.edificio2, ta.concreto, model);
    drawObject(mLightsShader, ta.tienda, ta.concreto, model);
    drawObject(mLightsShader, ta.hopstial, ta.concreto, model);
    drawObject(mLightsShader, ta.banco, ta.concreto, model);
    drawObject(mLightsShader, ta.policia, ta.concreto, model);
    drawObject(mLightsShader, ta.luzSemaforo, ta.traslucido, model);
    //drawObject(mLightsShader, ta.co2, ta.gases, model);
    drawObject(mLightsShader, ta.camion, ta.plastic, model);
    drawObject(mLightsShader, ta.arboles, ta.plastic, model);
    glUseProgram(0);

    {
        smoke->use();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        smoke->setMat4("projection", projection);
        smoke->setMat4("view", view);
        smoke->setMat4("model", model);

        smoke->setFloat("time", timeSmoke);
        smoke->setFloat("intensity", 4.0f);   // FUERTE
        smoke->setFloat("speed", 4.0f);       // RÁPIDO
        smoke->setFloat("scale", 0.3f);       // RUIDO DETALLADO

        smoke->setVec4("baseColor", glm::vec4(0.12f, 0.12f, 0.12f, 0.55f));

        ta.co2->Draw(*smoke);   // <- aquí pasas *shader, no el puntero

        timeSmoke += 0.05f;
    }
    glUseProgram(0);  
}

void renderFresnelCristal(const glm::mat4& projection, const glm::mat4& view)
{
    if (!fresnelShader || !g_glassModel || g_envCubemapTexID == 0) return;

    fresnelShader->use();

    fresnelShader->setMat4("projection", projection);
    fresnelShader->setMat4("view", view);

    // Cámara
    fresnelShader->setVec3("cameraPosition", camera.Position);

    // Parámetros Fresnel típicos para vidrio
    fresnelShader->setFloat("mRefractionRatio", 1.0f / 1.52f); // ~0.658
    fresnelShader->setFloat("_Bias", 0.02f);
    fresnelShader->setFloat("_Scale", 0.98f);
    fresnelShader->setFloat("_Power", 5.0f);

    // Bind del cubemap en la unidad 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, g_envCubemapTexID);
    fresnelShader->setInt("cubetex", 0);

    // Si el mesh tiene difusa, la ponemos en unidad 1 (opcional)
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    fresnelShader->setInt("texture_diffuse1", 1);

    // Matriz de modelo: coloca/escala el “cristal de edificio”
    glm::mat4 model = glm::mat4(1.0f);
    // Ajusta a tu escena (altura, rotación, escala)
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, (-WORLD_SIZE * 35) - 2.5));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    fresnelShader->setMat4("model", model);

    // Dibujar el modelo
    for (unsigned int i = 0; i < g_glassModel->meshes.size(); ++i)
    {
        // Si el mesh trae difusa, la usamos en la unidad 1
        if (!g_glassModel->meshes[i].textures.empty()) {
            unsigned int tex2D = 0;
            for (const auto& t : g_glassModel->meshes[i].textures) {
                if (t.type == "texture_diffuse") { tex2D = t.id; break; }
            }
            if (tex2D != 0) {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, tex2D);
            }
        }
        // VAO y draw
        glBindVertexArray(g_glassModel->meshes[i].VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(g_glassModel->meshes[i].indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // Limpieza mínima
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glUseProgram(0);
}

void renderScene() {
    glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 2000.0f);
    glm::mat4 view = camera.GetViewMatrix();
    cameraFrustum.extractPlanes(projection * view);

    //2. ENRUTADOR DE ESCENA 
    if (escena == 0) {
        renderEspacioScene(projection, view);
    }
    else if (escena == 1) {
        renderForestScene(projection, view);
        renderGlaciarScene(projection, view);
        renderTestEnvironment(projection, view);
        renderFresnelCristal(projection, view);

    }
    if (menu)renderMenuScene(projection, view);

}

//Renderizado Tierra glaciar 
void renderMenuScene(const glm::mat4& projection, const glm::mat4& view) {
    {
        basicShader->use();

        basicShader->setMat4("projection", projection);
        basicShader->setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 10.0f, DT1));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        basicShader->setMat4("model", model);

        ma.Texto1->Draw(*basicShader);

    }
    glUseProgram(0);
    {
        basicShader->use();

        basicShader->setMat4("projection", projection);
        basicShader->setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 10.0f, DT2));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        basicShader->setMat4("model", model);

        ma.Texto2->Draw(*basicShader);
    }
    glUseProgram(0);
    {
        basicShader->use();

        basicShader->setMat4("projection", projection);
        basicShader->setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 10.0f, DT3));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        basicShader->setMat4("model", model);

        ma.Texto3->Draw(*basicShader);
    }
    glUseProgram(0);
    {
        basicShader->use();

        basicShader->setMat4("projection", projection);
        basicShader->setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 10.0f, DT4));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        basicShader->setMat4("model", model);

        ma.Texto4->Draw(*basicShader);
    }
    glUseProgram(0);
    {
        basicShader->use();

        basicShader->setMat4("projection", projection);
        basicShader->setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 10.0f, DT5));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        basicShader->setMat4("model", model);

        ma.Texto5->Draw(*basicShader);
    }
    glUseProgram(0);
    {
        basicShader->use();

        basicShader->setMat4("projection", projection);
        basicShader->setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 10.0f, DT6));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        basicShader->setMat4("model", model);

        ma.Texto6->Draw(*basicShader);
    }
    glUseProgram(0);
    {
        basicShader->use();

        basicShader->setMat4("projection", projection);
        basicShader->setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 10.0f, DHoja));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        basicShader->setMat4("model", model);

        ma.HojaArce_Menu->Draw(*basicShader);
        Time += 0.05f;
    }
    glUseProgram(0);
    {
        basicShader->use();

        basicShader->setMat4("projection", projection);
        basicShader->setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        basicShader->setMat4("model", model);

        ma.Fondo_Menu->Draw(*basicShader);
    }
    glUseProgram(0);

}

void renderEspacioScene(const glm::mat4& projection, const glm::mat4& view) {
    glm::mat4 view_skybox = glm::mat4(glm::mat3(view));

    if (skyboxShader != nullptr && skyboxShader->ID != 0) {
        glDepthFunc(GL_LEQUAL);
        skyboxShader->use();
        skyboxShader->setMat4("projection", projection);
        skyboxShader->setMat4("view", view_skybox);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1000.0f));
        skyboxShader->setMat4("model", model);
        Model* currentSkybox = ea.cubeenv_noche;
        if (currentSkybox != nullptr) {
            currentSkybox->Draw(*skyboxShader);
        }
        glDepthFunc(GL_LESS);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Dibujamos un objeto cualquiera
    {
        // Activamos el shader de Phong
        phongShader2->use();

        // Activamos para objetos transparentes
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Aplicamos transformaciones de proyección y cámara (si las hubiera)
        phongShader2->setMat4("projection", projection);
        phongShader2->setMat4("view", view);

        // Aplicamos transformaciones del modelo
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(Time), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        phongShader2->setMat4("model", model);

        // Configuramos propiedades de fuentes de luz
        phongShader2->setVec4("LightColor", ea.theLight.Color);
        phongShader2->setVec4("LightPower", ea.theLight.Power);
        phongShader2->setInt("alphaIndex", ea.theLight.alphaIndex);
        phongShader2->setFloat("distance",ea.theLight.distance);
        phongShader2->setVec3("lightPosition", ea.theLight.Position);
        phongShader2->setVec3("lightDirection", ea.theLight.Direction);
        phongShader2->setVec3("eye", camera.Position);

        // Aplicamos propiedades materiales
        phongShader2->setVec4("MaterialAmbientColor", ea.defaultMaterial.ambient);
        phongShader2->setVec4("MaterialDiffuseColor", ea.defaultMaterial.diffuse);
        phongShader2->setVec4("MaterialSpecularColor", ea.defaultMaterial.specular);
        phongShader2->setFloat("transparency", ea.defaultMaterial.transparency);

        ea.Tierra->Draw(*phongShader2);

        phongShader2->setFloat("transparency", transparenciaC);

        ea.Canada->Draw(*phongShader2);



    }

    {
		basicShader->use();
		basicShader->setMat4("projection", projection); 
		basicShader->setMat4("view", view); 

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-20.0f, 0.0f, 3.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        basicShader->setMat4("model", model);

		ea.sol->Draw(*basicShader);
    }

    glUseProgram(0);


    {
        moonShader->use();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.37f, 0.37f, 0.37f));

        moonShader->setMat4("projection", projection);
        moonShader->setMat4("view", view);

        moonShader->setMat4("model", model);

        moonShader->setFloat("time", Time / 20);
        moonShader->setFloat("radius", 5.0f);
        moonShader->setFloat("height", 5.0f);

        ea.Luna->Draw(*moonShader);
    }

    glUseProgram(0);

}

void renderGlaciarScene(const glm::mat4& projection, const glm::mat4& view) {

    float desp = 359.5f;

    if (temperatura > 0.0f && glacierScaleY > 0.0f) {
        float speed = meltSpeedBase * temperatura;     // más calor → más rápido
        glacierScaleY = glm::max(0.0f, glacierScaleY - speed * deltaTime);
    }
    setupStaticShader(mLightsShader, projection, view);
    setupShaderLights(mLightsShader, gLights);

    // Coche (Metálico -> Phong)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, desp));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

    drawObject(mLightsShader, ga.Glaciares, ga.GlaciaresMaterial, model);
    drawObject(mLightsShader, ga.PlacaHielo, ga.nieveMaterial, model);
    if (glacierScaleY > 0.0f)
    {
        glm::mat4 mG = model;
        mG = glm::scale(mG, glm::vec3(1.0f, 1.0f, glacierScaleY));
        drawObject(mLightsShader, ga.Iceberg, ga.nieveMaterial, mG);

    }

    {
        wavesShader2->use();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, desp));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

        wavesShader2->setMat4("projection", projection);
        wavesShader2->setMat4("view", view);

        wavesShader2->setMat4("model", model);

        wavesShader2->setFloat("time", wavesTime);
        if (calor) {
            wavesShader2->setFloat("radius", 0.7f);
            wavesShader2->setFloat("height", 0.7f);
        }
        else {
            wavesShader2->setFloat("radius", 0.2f);
            wavesShader2->setFloat("height", 0.2f);
        }

        ga.Agua->Draw(*wavesShader2);

        wavesTime += 0.01f;
    }
    glUseProgram(0);
    // Descenso de osos 1 y 2 en función del derretimiento
    float osoDown = 8 * glacierScaleY - 8; // 0→sin bajar, 1→máximo

    if (glacierScaleY > 0.0f) {

        setupStaticShader(mLightsShader, projection, view);
        setupShaderLights(mLightsShader, gLights);

        // Coche (Metálico -> Phong)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, desp+osoDown));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, glacierScaleY));

        drawObject(mLightsShader, ga.TrozoH1, ga.nieveMaterial, model);
        drawObject(mLightsShader, ga.TrozoH2, ga.nieveMaterial, model);
    }
    glUseProgram(0);

    {
        ga.Oso1->UpdateAnimation(deltaTime);

        dynamicShader->use();
        dynamicShader->setMat4("projection", projection);
        dynamicShader->setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 7.6f+osoDown, desp));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));

        dynamicShader->setMat4("model", model);
        dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, ga.Oso1->gBones);
        ga.Oso1->Draw(*dynamicShader);
    }
    glUseProgram(0);
    {
        ga.Oso2->UpdateAnimation(deltaTime);

        dynamicShader->use();
        dynamicShader->setMat4("projection", projection);
        dynamicShader->setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 6.4+osoDown, -42.4+desp));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));

        dynamicShader->setMat4("model", model);
        dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, ga.Oso2->gBones);
        ga.Oso2->Draw(*dynamicShader);
    }
    glUseProgram(0);
    
    
    // ===== Oso3, Oso4, Oso5: caminar en tramos de 10 y girar ±90° aleatorio =====
        auto avanzarOso = [&](int idx) {
            // Dirección de avance a partir de yaw (solo Yaw sobre eje Y)
            float yawRad = glm::radians(osoYawDeg[idx]);
            glm::vec3 forward = glm::vec3(-std::cos(yawRad), 0.0f, std::sin(yawRad)); // +X al inicio

            // Avance por frame
            osoPos[idx] += forward * (OSO_SPEED * deltaTime);

            // Si ya recorrió 10 unidades desde el último giro, elige izquierda o derecha al azar
            float distTramo = glm::length(osoPos[idx] - osoUltimoGiro[idx]);
            if (distTramo >= OSO_SEGMENTO) {
                int signo = (std::rand() & 1) ? 1 : -1;   // 1 o -1
                osoYawDeg[idx] += 90.0f * signo;
                // Normaliza yaw a [-180,180] opcional
                if (osoYawDeg[idx] > 180.0f) osoYawDeg[idx] -= 360.0f;
                if (osoYawDeg[idx] < -180.0f) osoYawDeg[idx] += 360.0f;
                // Nuevo origen del siguiente tramo
                osoUltimoGiro[idx] = osoPos[idx];
            }
            };

        // Oso3
        {
            ga.Oso2->UpdateAnimation(deltaTime);
            avanzarOso(0);

            dynamicShader->use();
            dynamicShader->setMat4("projection", projection);
            dynamicShader->setMat4("view", view);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, osoPos[0]);
            model = glm::translate(model, glm::vec3(80.0f, 9.5f, -11.4f+desp));
            model = glm::rotate(model, glm::radians(osoYawDeg[0]), glm::vec3(0.0f, 1.0f, 0.0f)); // yaw
            model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));

            dynamicShader->setMat4("model", model);
            dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, ga.Oso2->gBones);
            ga.Oso2->Draw(*dynamicShader);
        }
        glUseProgram(0);

        // Oso4
        {
            ga.Oso2->UpdateAnimation(deltaTime);

            avanzarOso(1);

            dynamicShader->use();
            dynamicShader->setMat4("projection", projection);
            dynamicShader->setMat4("view", view);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, osoPos[1]);
            model = glm::translate(model, glm::vec3(80.0f, 9.2f, -5.759+desp));
            model = glm::rotate(model, glm::radians(osoYawDeg[1]), glm::vec3(0.0f, 1.0f, 0.0f)); // yaw
            model = glm::scale(model, glm::vec3(0.008f, 0.008f, 0.008f));

            dynamicShader->setMat4("model", model);
            dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, ga.Oso2->gBones);
            ga.Oso2->Draw(*dynamicShader);
        }
        glUseProgram(0);

        // Oso5
        {
            ga.Oso2->UpdateAnimation(deltaTime);
            avanzarOso(2);

            dynamicShader->use();
            dynamicShader->setMat4("projection", projection);
            dynamicShader->setMat4("view", view);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, osoPos[2]);
            model = glm::translate(model, glm::vec3(80.0f, 9.2f, -16.23f+desp));
            model = glm::rotate(model, glm::radians(osoYawDeg[2]), glm::vec3(0.0f, 1.0f, 0.0f)); // yaw
            model = glm::scale(model, glm::vec3(0.008f, 0.008f, 0.008f));

            dynamicShader->setMat4("model", model);
            dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, ga.Oso2->gBones);
            ga.Oso2->Draw(*dynamicShader);
        }
        glUseProgram(0);
    

    if (!menu) {
        {
            // === Termómetro pegado a la cámara (lateral izquierdo) ===
            basicShader->use();
            basicShader->setMat4("projection", projection);
            basicShader->setMat4("view", view);

            // Base de cámara desde la vista
            glm::mat4 invV = glm::inverse(view);
            glm::vec3 eye = glm::vec3(invV[3]);
            glm::vec3 f = glm::normalize(glm::vec3(invV * glm::vec4(0, 0, -1, 0))); // forward
            glm::vec3 u = glm::normalize(glm::vec3(invV * glm::vec4(0, 1, 0, 0))); // up
            glm::vec3 r = glm::normalize(glm::cross(f, u));                       // right

            // Colocación
            const float d = 0.5f;      // distancia fija delante
            const float x = 0.25f;     // desplazamiento a la izquierda (en unidades de mundo)
            const float y = 0.00f;     // desplazamiento vertical

            glm::vec3 center = eye + f * d - r * x + u * y;

            // Orientación tipo billboard: columnas = (right, up, -forward)
            glm::mat4 R(1.0f);
            R[0] = glm::vec4(r, 0.0f);
            R[1] = glm::vec4(u, 0.0f);
            R[2] = glm::vec4(-f, 0.0f);

            // Escala del icono
            glm::vec3 S(0.05f, 0.05f, 0.06f);

            glm::mat4 B = glm::rotate(glm::mat4(1.0f), glm::radians(-66.0f), glm::vec3(0, 1, 0));
            glm::mat4 C = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
            // Modelo final
            glm::mat4 model =
                glm::translate(glm::mat4(1.0f), center) *
                R *
                B *
                C *
                glm::scale(glm::mat4(1.0f), S);

            basicShader->setMat4("model", model);
            if (temperatura < 0.0f) {
                // Contorno
                ga.TerAzulBase->Draw(*basicShader);

                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f + barraTF));

                basicShader->setMat4("model", model);
               ga. BarraAzul->Draw(*basicShader);
            }
            else {
                // Contorno
                ga.TerRojoBase->Draw(*basicShader);
                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f + barraTC));
                basicShader->setMat4("model", model);
                ga.BarraRoja->Draw(*basicShader);
            }
        }
        glUseProgram(0);
    }

}

/* --------------------------------------------- Renderizado Procedural------------------------------------------*/

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

void renderForestScene(const glm::mat4& projection, const glm::mat4& view) {
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
    float escala;
    glm::mat4 view_skybox = glm::mat4(glm::mat3(view));


    //La iluminación del bosque sigue a la del sol
    if (!gLights.empty())
    {
        gLights[0].Position = fa.theLight.Position;
        gLights[0].Color = fa.theLight.Color;

        float cityMultiplier = 2.5f;  // Ajustable
        gLights[0].Power = dimmedLightPower * cityMultiplier;

        gLights[0].alphaIndex = fa.theLight.alphaIndex;
    }

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

    // --- 3.8 DIBUJAR MODELOS ANIMADOS (LOBO, CASTOR, OSO, ALCE) ---
    // --- BLOQUE CORREGIDO (Lógica de cráneo + Rotaciones correctas) ---
    if ((dynamicShader != nullptr || phongShader != nullptr) && !g_animals.empty())
    {
        for (const AnimalInstance& animal : g_animals)
        {
            if (animal.state == AnimalState::DEAD)
            {
                // --- DIBUJAR CRÁNEO ---
                // (Esta lógica es correcta y dibuja el cráneo)
                if (fa.skull_model == nullptr || phongShader == nullptr) continue;

                phongShader->use();
                phongShader->setMat4("projection", projection);
                phongShader->setMat4("view", view);
                phongShader->setVec3("lightPosition", fa.theLight.Position);
                phongShader->setVec4("LightColor", fa.theLight.Color);
                phongShader->setVec4("LightPower", dimmedLightPower);
                phongShader->setInt("alphaIndex", fa.theLight.alphaIndex);
                phongShader->setVec4("MaterialAmbientColor", fa.defaultMaterial.ambient);
                phongShader->setVec4("MaterialSpecularColor", fa.defaultMaterial.specular);
                phongShader->setFloat("transparency", 1.0f);
                phongShader->setBool("isTerrain", false);

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, animal.position);
                model = glm::rotate(model, animal.rotationY, glm::vec3(0.0f, 1.0f, 0.0f));

                // Rotación clave para que el cráneo mire hacia arriba
                model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

                model = glm::scale(model, glm::vec3(0.5f));
                phongShader->setMat4("model", model);

                fa.skull_model->Draw(*phongShader);
            }
            else // --- ANIMAL VIVO (WALKING o IDLE) ---
            {
                // 1. Determinar qué modelo y escala usar
                AnimatedModel* modelToDraw = nullptr;
                float escala = 0.5f;
                glm::mat4 speciesTransform = glm::mat4(1.0f); // Transformación base de la especie

                if (animal.state == AnimalState::WALKING) {
                    modelToDraw = animal.walk;
                }
                else { // IDLE
                    modelToDraw = animal.idle;
                }

                if (modelToDraw == nullptr || dynamicShader == nullptr) continue;

                // 2. Determinar transformaciones de la especie (usando 'walk' como ID)
                if (animal.walk == fa.character01 || animal.walk == fa.character02) { //Lobo
                    escala = 0.06f;
                    // El lobo no necesita rotación base, pero su animación sí (manejado en mechanic.cpp)
                }
                else if (animal.walk == fa.character03 || animal.walk == fa.character04) { //Castor
                    escala = 0.02f;
                    speciesTransform = glm::rotate(speciesTransform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    speciesTransform = glm::translate(speciesTransform, glm::vec3(0.0f, 1.8f, 0.0f));
                }
                else if (animal.walk == fa.character05 || animal.walk == fa.character06) { //Oso
                    escala = 0.016f;
                    speciesTransform = glm::rotate(speciesTransform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    speciesTransform = glm::translate(speciesTransform, glm::vec3(0.0f, 0.5f, 0.0f));
                }
                else if (animal.walk == fa.character07 || animal.walk == fa.character08) { //Alce
                    escala = 0.02f;
                    speciesTransform = glm::rotate(speciesTransform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    speciesTransform = glm::translate(speciesTransform, glm::vec3(0.0f, 3.7f, 0.0f));
                }

                // 3. Configurar Shader
                dynamicShader->use();
                dynamicShader->setMat4("projection", projection);
                dynamicShader->setMat4("view", view);
                dynamicShader->setVec3("lightPosition", fa.theLight.Position);
                dynamicShader->setVec4("LightColor", fa.theLight.Color);
                dynamicShader->setVec4("LightPower", dimmedLightPower);
                dynamicShader->setInt("alphaIndex", fa.theLight.alphaIndex);
                dynamicShader->setVec4("MaterialAmbientColor", fa.defaultMaterial.ambient);
                dynamicShader->setVec4("MaterialSpecularColor", fa.defaultMaterial.specular);
                dynamicShader->setFloat("transparency", 1.0f);

                // 4. Aplicar TODAS las transformaciones en el orden correcto
                // Orden: Escala -> Rotación Especie -> Rotación Dirección -> Posición
                // (Se aplica al revés en código: Posición * Rotación Dirección * Rotación Especie * Escala)

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, animal.position);
                model = glm::rotate(model, animal.rotationY, glm::vec3(0.0f, 1.0f, 0.0f));

                // Aplicamos la rotación/translación base de la especie
                model = model * speciesTransform;

                model = glm::scale(model, glm::vec3(escala));
                dynamicShader->setMat4("model", model);

                // 5. Enviar huesos y dibujar
                for (unsigned int i = 0; i < MAX_RIGGING_BONES; ++i) {
                    std::string uniformName = "gBones[" + std::to_string(i) + "]";
                    dynamicShader->setMat4(uniformName, animal.gBones[i]);
                }

                modelToDraw->Draw(*dynamicShader);
            }
        }
    }
    // --- FIN 3.8 ---


    // --- FIN 3.8 ---
    // --- 5. DIBUJAR EL SOL (ANTES DEL SKYBOX) ---
    if (sunShader != nullptr && sunShader->ID != 0 && fa.sun_model != nullptr) {
        // Escribir en depth para que montañas puedan ocultarlo
        sunShader->use();
        sunShader->setMat4("projection", projection);
        sunShader->setMat4("view", view_skybox); // sin traslación
        float sunVisibleDistance = 950.0f;
        glm::vec3 sunVisiblePosition = glm::vec3(sunVisibleDistance * cos(sunElevRad) * sin(sunAngleRad), sunVisibleDistance * sin(sunElevRad), sunVisibleDistance * cos(sunElevRad) * cos(sunAngleRad));
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, sunVisiblePosition);
        model = glm::scale(model, glm::vec3(15.0f));
        sunShader->setMat4("model", model);
        glm::vec4 sunTintColor = glm::vec4(1.0f, 1.0f, 0.8f, 1.0f) * nightDimFactor;
        sunShader->setVec4("tintColor", sunTintColor);
        fa.sun_model->Draw(*sunShader);
    }

    // --- 6. DIBUJAR LA LUNA (ANTES DEL SKYBOX) ---
    if (sunShader != nullptr && sunShader->ID != 0 && fa.moon_model != nullptr) {
        sunShader->use();
        sunShader->setMat4("projection", projection);
        sunShader->setMat4("view", view_skybox);
        float moonVisibleDistance = 900.0f;
        glm::vec3 moonVisiblePosition = glm::vec3(-moonVisibleDistance * cos(sunElevRad) * sin(sunAngleRad), -moonVisibleDistance * sin(sunElevRad), -moonVisibleDistance * cos(sunElevRad) * cos(sunAngleRad));
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, moonVisiblePosition);
        model = glm::scale(model, glm::vec3(12.0f));
        sunShader->setMat4("model", model);
        glm::vec4 moonTintColor = glm::vec4(0.8f, 0.85f, 0.95f, 1.0f) * (1.0f - dayNightTransition);
        sunShader->setVec4("tintColor", moonTintColor);
        fa.moon_model->Draw(*sunShader);
    }

    // --- DIBUJAR PERSONAJE DEMI ---

 // --- DIBUJAR PERSONAJE (3RA PERSONA) ---

    if (g_isThirdPerson) {

        // 1. Determinar qué modelo dibujar
        Animated* modelToRender = nullptr;
        float scaleToUse = 1.0f;

        if (g_activeCharacter == 1) {
            modelToRender = g_demiModel;
            scaleToUse = DEMI_SCALE;
        }
        else if (g_activeCharacter == 2) {
            modelToRender = g_mikuModel;
            scaleToUse = MIKU_SCALE;
        }

        // 2. Dibujar si el modelo es válido
        if (modelToRender != nullptr) {
            dynamicShader->use();
  
            dynamicShader->setMat4("projection", projection);
            dynamicShader->setMat4("view", view);
            dynamicShader->setVec3("lightPosition", fa.theLight.Position);
            dynamicShader->setVec4("LightColor", fa.theLight.Color);
            dynamicShader->setVec4("LightPower", dimmedLightPower);
            dynamicShader->setInt("alphaIndex", fa.theLight.alphaIndex);
            dynamicShader->setVec4("MaterialAmbientColor", fa.defaultMaterial.ambient);
            dynamicShader->setVec4("MaterialSpecularColor", fa.defaultMaterial.specular);
            dynamicShader->setFloat("transparency", 1.0f);

            glm::mat4 model = glm::mat4(1.0f);

  
            model = glm::translate(model, g_demiPos + glm::vec3(0.0f, DEMI_OFFSET_Y, 0.0f));

        
            model = glm::rotate(model, g_demiRotY, glm::vec3(0.0f, 1.0f, 0.0f));

     
            //model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));


            model = glm::scale(model, glm::vec3(scaleToUse));

            dynamicShader->setMat4("model", model);

            dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, modelToRender->gBones);

            modelToRender->Draw(*dynamicShader);
        }
    }

    // --- 4. DIBUJAR SKYBOX (AL FINAL PARA NO TAPAR SOL/LUNA) ---
// --- 6. SKYDOME (ESFERAS FBX) ---
    // Usamos el shader simple que ya tenías para el cubo, funciona igual para la esfera
    if (skyboxShader != nullptr) {
        glDepthFunc(GL_LEQUAL); // Truco: Dibujar solo donde la profundidad sea <= (al fondo)
        skyboxShader->use();

        // 1. Proyección normal
        skyboxShader->setMat4("projection", projection);

        // 2. Vista SIN TRASLACIÓN (Para que el cielo te siga y no te acerques nunca)
        glm::mat4 viewNoTrans = glm::mat4(glm::mat3(view));
        skyboxShader->setMat4("view", viewNoTrans);

        // 3. Transformación del Modelo
        glm::mat4 model = glm::mat4(1.0f);

        // Rotación opcional si la textura quedó chueca en Blender
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 

        // Escalamos la esfera para que sea gigante y cubra todo
        model = glm::scale(model, glm::vec3(1000.0f));

        skyboxShader->setMat4("model", model);

        // 4. Elegir cuál dibujar
        if (isDay) {
            if (fa.sphereDay) fa.sphereDay->Draw(*skyboxShader);
        }
        else {
            if (fa.sphereNight) fa.sphereNight->Draw(*skyboxShader);
        }

        glDepthFunc(GL_LESS); // Regresar profundidad a la normalidad
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
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

/* --------------------------------------------- Renderizado de Iluminacion ------------------------------------------*/
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
void SetLightUniformInt(Shader* shader, const char* propertyName, size_t lightIndex, int value) {
    std::ostringstream ss;
    ss << "allLights[" << lightIndex << "]." << propertyName;
    std::string uniformName = ss.str();

    shader->setInt(uniformName.c_str(), value);
}
void SetLightUniformFloat(Shader* shader, const char* propertyName, size_t lightIndex, float value) {
    std::ostringstream ss;
    ss << "allLights[" << lightIndex << "]." << propertyName;
    std::string uniformName = ss.str();

    shader->setFloat(uniformName.c_str(), value);
}
void SetLightUniformVec4(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec4 value) {
    std::ostringstream ss;
    ss << "allLights[" << lightIndex << "]." << propertyName;
    std::string uniformName = ss.str();

    shader->setVec4(uniformName.c_str(), value);
}
void SetLightUniformVec3(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec3 value) {
    std::ostringstream ss;
    ss << "allLights[" << lightIndex << "]." << propertyName;
    std::string uniformName = ss.str();

    shader->setVec3(uniformName.c_str(), value);
}