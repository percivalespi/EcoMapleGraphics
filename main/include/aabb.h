#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>

class AABB {
public:
    glm::vec3 position;
    float size;
    glm::vec3 color;

    AABB(glm::vec3 pos = glm::vec3(0.0f), float s = 1.0f, glm::vec3 col = glm::vec3(1.0f, 1.0f, 0.0f))
        : position(pos), size(s), color(col) {
    }

    // Métodos básicos
    bool collidesWith(const AABB& other) const {
        return (abs(position.x - other.position.x) * 2 < (size + other.size)) &&
            (abs(position.y - other.position.y) * 2 < (size + other.size)) &&
            (abs(position.z - other.position.z) * 2 < (size + other.size));
    }

    glm::vec3 getMin() const {
        float halfSize = size / 2.0f;
        return glm::vec3(position.x - halfSize, position.y - halfSize, position.z - halfSize);
    }

    glm::vec3 getMax() const {
        float halfSize = size / 2.0f;
        return glm::vec3(position.x + halfSize, position.y + halfSize, position.z + halfSize);
    }
};

#endif