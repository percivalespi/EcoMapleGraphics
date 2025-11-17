#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Shader.h"
#include "Mesh.h"

// Clase AABB para cajas de colisión
class AABB {
public:
    glm::vec3 position;
    float size;
    Mesh* mesh;
    glm::vec3 color;

    AABB(glm::vec3 pos, float s, glm::vec3 col = glm::vec3(1.0f, 1.0f, 0.0f));
    ~AABB();

    void updateMesh();
    void setColor(glm::vec3 newColor);
    void Draw(Shader& shader, glm::mat4& projection, glm::mat4& view);

    bool collidesWith(const AABB& other) const;
    glm::vec3 getMin() const;
    glm::vec3 getMax() const;
    bool rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& t) const;

private:
    std::vector<Vertex> getVerticesWithColor(glm::vec3 color) const;
    std::vector<unsigned int> getIndices() const;
};

#endif