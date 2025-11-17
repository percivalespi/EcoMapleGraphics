#ifndef AABBMESH_H
#define AABBMESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <shader_m.h>

struct AABBVertex {
    glm::vec3 Position;
    glm::vec4 Color;
};

class AABBMesh {
public:
    std::vector<AABBVertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;

    AABBMesh(std::vector<AABBVertex> vertices, std::vector<unsigned int> indices)
        : vertices(vertices), indices(indices) {
        setupMesh();
    }

    ~AABBMesh() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void Draw(Shader& shader) {
        glBindVertexArray(VAO);
        glDrawElements(GL_LINES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(AABBVertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Posiciones
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AABBVertex), (void*)0);

        // Colores
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(AABBVertex), (void*)offsetof(AABBVertex, Color));

        glBindVertexArray(0);
    }
};

#endif