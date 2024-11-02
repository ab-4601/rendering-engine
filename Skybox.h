#pragma once

#include "Core.h"
#include "Camera.h"
#include "Texture.h"
#include "SkyboxShader.h"

class Skybox : public Texture {
private:
	std::vector<std::string> faces;

    GLuint VAO, VBO;

    GLfloat vertices[108] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    SkyboxShader shader;

public:
	/*Skybox(std::vector<std::string> faces = { {"Textures/skybox/right.jpg", "Textures/skybox/left.jpg", 
	"Textures/skybox/top.jpg", "Textures/skybox/bottom.jpg", "Textures/skybox/front.jpg", "Textures/skybox/back.jpg"}});*/

    Skybox(std::vector<std::string> faces = { {"Textures/skybox/right.png", "Textures/skybox/left.png",
    "Textures/skybox/top.png", "Textures/skybox/bottom.png", "Textures/skybox/back.png", "Textures/skybox/front.png"} });

	void loadCubemap();
    void loadHDRCubemap(const char* file_name);
    void renderSkybox(const glm::mat4& projection, const Camera& camera);

	~Skybox() = default;
};

