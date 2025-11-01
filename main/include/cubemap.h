#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h> // Make sure stb_image is included *before* this header, or include it here.

#include <string>
#include <vector>
#include <iostream>
#include <shader_m.h> // Include shader_m.h if needed by drawCubeMap

using namespace std;

class CubeMap {
public:
    unsigned int textureID;
    unsigned int VAO, VBO;

    CubeMap() : textureID(0), VAO(0), VBO(0) {
        float size = 1.0f; // Size doesn't matter much if using view matrix trick
        float skyboxVertices[] = {
            // positions
            -size,  size, -size, -size, -size, -size,  size, -size, -size,
             size, -size, -size,  size,  size, -size, -size,  size, -size,
            -size, -size,  size, -size, -size, -size, -size,  size, -size,
            -size,  size, -size, -size,  size,  size, -size, -size,  size,
             size, -size, -size,  size, -size,  size,  size,  size,  size,
             size,  size,  size,  size,  size, -size,  size, -size, -size,
            -size, -size,  size, -size,  size,  size,  size,  size,  size,
             size,  size,  size,  size, -size,  size, -size, -size,  size,
            -size,  size, -size,  size,  size, -size,  size,  size,  size,
             size,  size,  size, -size,  size,  size, -size,  size, -size,
            -size, -size, -size, -size, -size,  size,  size, -size, -size,
             size, -size, -size, -size, -size,  size,  size, -size,  size
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);
    }

    ~CubeMap() {
        // Optional: Clean up OpenGL resources if needed
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteTextures(1, &textureID);
    }

    // --- FUNCIÓN MODIFICADA PARA DEVOLVER bool ---
    bool loadCubemap(vector<std::string> faces)
    {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        bool success = true; // Flag to track success
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            // --- ASEGÚRATE DE QUE stbi_set_flip_vertically_on_load(false) esté llamado antes si es necesario ---
            // stbi_set_flip_vertically_on_load(false); // Cubemaps usually don't need flipping
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                GLenum format = GL_RGB;
                if (nrChannels == 1) format = GL_RED;
                else if (nrChannels == 3) format = GL_RGB;
                else if (nrChannels == 4) format = GL_RGBA;

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data); // Call even if data is null
                success = false; // Mark as failed
            }
        }
        if (!success) return false; // Return early if any image failed

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return true; // Return true if all loaded successfully
    }
    // ---------------------------------------------

    // --- FUNCIÓN drawCubeMap LIGERAMENTE MODIFICADA ---
    void drawCubeMap(Shader& shad, const glm::mat4& projection, const glm::mat4& view) {
        glDepthFunc(GL_LEQUAL); // Change depth function so depth test passes when values are equal to depth buffer's content
        shad.use();
        shad.setMat4("view", view); // View matrix without translation applied in Update
        shad.setMat4("projection", projection);

        // skybox cube
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        shad.setInt("skybox", 0); // Assuming your cubemap shader uses a sampler named "skybox"
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // Set depth function back to default
    }
    // ----------------------------------------------------

private:
    // No EBO needed for glDrawArrays
};

#endif