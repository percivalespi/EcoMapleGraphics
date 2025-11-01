#ifndef MATERIAL_H
#define MATERIAL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Material 
{
public:
	// Material Attributes
	glm::vec4 ambient; //K_a asociado al objeto osea la K_a_o
	glm::vec4 diffuse; // k_d_o
	glm::vec4 specular; // k_s_o
	float     transparency; //transparencia

	Material() {
		ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f); // gris oscuro	
		diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f); // gris intermedio
		specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f); // gris intermedio
		transparency = 1.0f;
	}
	~Material() {}

private:

};

#endif
