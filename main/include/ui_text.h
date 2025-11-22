// ui_text.h
#ifndef UI_TEXT_H
#define UI_TEXT_H

#include "globals.h"
#include <map>

struct Character {
    unsigned int TextureID; // ID del handle de la textura del atlas
    glm::vec2 Size;         // Tamaño
    glm::vec2 Bearing;      // Offset desde la línea base
    unsigned int Advance;   // Offset para avanzar al siguiente glifo
};

// Sistema simple para renderizar texto usando un Atlas de Fuente (Bitmap Font)
class TextRenderer {
public:
    unsigned int VAO, VBO;
    Shader* textShader;
    Shader* boxShader; // Shader para el fondo de la notificación
    unsigned int fontTexture;

    TextRenderer() {}

    void Init(const char* fontPath) {
        // 1. Cargar Shader de Texto
        textShader = new Shader("shaders/text.vs", "shaders/text.fs");
        boxShader = new Shader("shaders/ui_box.vs", "shaders/ui_box.fs");

        // 2. Configurar VAO/VBO para quads dinámicos
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // 3. Cargar la textura de la fuente (El Atlas)
        fontTexture = TextureFromFile(fontPath, "."); // Asume que path es completo o relativo a ejecución
    }

    // Dibuja la caja de fondo estilo Minecraft
    void RenderBox(float x, float y, float width, float height, float screenW, float screenH) {
        boxShader->use();
        
        // Matriz de proyección ortográfica
        glm::mat4 projection = glm::ortho(0.0f, screenW, 0.0f, screenH);
        boxShader->setMat4("projection", projection);
        
        // Configurar modelo (posición y escala)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y, 0.0f));
        model = glm::scale(model, glm::vec3(width, height, 1.0f));
        boxShader->setMat4("model", model);

        // Colores estilo Minecraft
        boxShader->setVec4("bgColor", glm::vec4(0.0f, 0.0f, 0.0f, 0.7f)); // Negro semitransparente
        boxShader->setVec4("borderColor", glm::vec4(0.7f, 0.7f, 0.7f, 1.0f)); // Gris claro
        boxShader->setFloat("borderWidth", 0.02f); // Grosor relativo

        // Usamos el mismo VAO cuadrado que ya tenemos (o el de la UI general)
        glBindVertexArray(ui.uiVAO); // Reutilizamos el quad de UIAssets si es posible
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    // Dibuja el texto carácter por carácter
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color, float screenW, float screenH) {
        textShader->use();
        textShader->setVec3("textColor", color);
        textShader->setMat4("projection", glm::ortho(0.0f, screenW, 0.0f, screenH));
        textShader->setInt("text", 0);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fontTexture);
        glBindVertexArray(VAO);

        // Iterar sobre el texto
        float startX = x;
        for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
            char character = *c;
            
            // Calcular coordenadas UV en el Atlas (asumiendo rejilla 16x16 estándar ASCII)
            float uvSize = 1.0f / 16.0f;
            float u = (character % 16) * uvSize;
            float v = 1.0f - ((character / 16) * uvSize) - uvSize; // Invertir Y

            float w = 30 * scale; // Ancho base letra
            float h = 40 * scale; // Alto base letra
            
            // Ajuste básico de espaciado
            float xpos = x;
            float ypos = y;

            // Vértices del quad para la letra actual
            float vertices[6][4] = {
                { xpos,     ypos + h,   u,          v },
                { xpos,     ypos,       u,          v + uvSize },
                { xpos + w, ypos,       u + uvSize, v + uvSize },

                { xpos,     ypos + h,   u,          v },
                { xpos + w, ypos,       u + uvSize, v + uvSize },
                { xpos + w, ypos + h,   u + uvSize, v }
            };

            // Actualizar VBO
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Dibujar
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Avanzar cursor (espaciado simple monoespaciado o ajustado)
            x += (w * 0.6f); // 0.6 es para reducir el espacio entre letras
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

// --- SISTEMA DE NOTIFICACIONES ---
struct NotificationSystem {
    TextRenderer renderer;
    std::string currentMessage;
    float displayTime;
    float timer;
    bool active;

    void Init() {
        // IMPORTANTE: Necesitas una imagen 'font.png' que sea un grid de letras
        renderer.Init("models/image/font.png"); 
        active = false;
    }

    void Show(std::string message, float duration) {
        currentMessage = message;
        displayTime = duration;
        timer = 0.0f;
        active = true;
    }

    void Update(float dt) {
        if (active) {
            timer += dt;
            if (timer >= displayTime) {
                active = false;
            }
        }
    }

    void Draw(float screenW, float screenH) {
        if (!active) return;

        // Calcular efecto de desvanecimiento (Fade In / Out)
        float alpha = 1.0f;
        if (timer < 0.5f) alpha = timer / 0.5f; // Fade In
        if (timer > displayTime - 0.5f) alpha = (displayTime - timer) / 0.5f; // Fade Out

        // 1. Dibujar Caja de Fondo (Centrada)
        float boxWidth = currentMessage.length() * 15.0f + 40.0f; // Ancho dinámico según texto
        float boxHeight = 50.0f;
        float x = (screenW / 2.0f) - (boxWidth / 2.0f);
        float y = screenH * 0.2f; // Al 20% de la altura (abajo)

        renderer.RenderBox(x, y, boxWidth, boxHeight, screenW, screenH);

        // 2. Dibujar Texto (Centrado en la caja)
        // Pequeño ajuste de posición para el texto
        renderer.RenderText(currentMessage, x + 20.0f, y + 10.0f, 0.7f, glm::vec3(1.0f, 1.0f, 1.0f), screenW, screenH);
    }
};

extern NotificationSystem g_notifications;

#endif