#pragma once

#include "Mesh.h"

class Cube : public Mesh {
public:
	Cube(GLfloat specularIntensity = 0.f, GLfloat specularPower = 0.f);

	~Cube() = default;
};