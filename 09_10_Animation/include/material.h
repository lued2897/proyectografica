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
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	float     transparency;
	int alphaIndex;

	Material() {
		ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f); 
		diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f); 
		transparency = 1.0f;
		alphaIndex = 128;
	}

	Material(glm::vec4 amb_color, glm::vec4 dif_color, glm::vec4 spec_color, float transp,int alpha) {
		ambient = amb_color;
		diffuse = dif_color;
		specular = spec_color;
		transparency = transp;
		alphaIndex = alpha;
	}

	~Material() {}

private:

};

#endif
