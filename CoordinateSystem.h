#pragma once

#include "Line.h"
#include "CoordinateShader.h"
#include "CoordinateSystem.h"
#include "Camera.h"

class CoordinateSystem {
private:
	std::vector<Line*> axes;

	CoordinateShader shader;

public:
	CoordinateSystem();

	void createCoordinateSystem();
	void drawCoordinateSystem(GLint windowHeight, GLint windowWidth, GLint bufferWidth,
		GLint bufferHeight, const Camera* const camera, glm::mat4& model, const glm::mat4& projection);

	~CoordinateSystem();
};

