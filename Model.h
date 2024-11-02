#pragma once

#include "Core.h"
#include "Mesh.h"
#include "Texture.h"

class Model {
private:
	std::string texFolderPath;

	std::vector<Mesh*> meshList;
	std::vector<Texture*> diffuseMaps;
	std::vector<Texture*> normalMaps;
	std::vector<Texture*> heightMaps;
	std::vector<unsigned int> meshToTex;

	std::vector<GLfloat> vertices;
	std::vector<uint> indices;

	void _loadNode(aiNode* node, const aiScene* const scene);
	void _loadMesh(aiMesh* mesh, const aiScene* const scene);
	void _loadMaterialTextures(const aiScene* const scene);
	void _loadMaterialNormalMaps(const aiScene* const scene);
	void _loadMaterialHeightMaps(const aiScene* const scene);

public:
	Model(std::string fileName = "", std::string texFolderPath = "");

	void loadModel(std::string fileName = "");
	void renderModel(
		glm::mat4 lightSpaceTransform = glm::mat4(1.f), GLuint directionalShadowMap = 0, GLuint pointShadowMap = 0
	);
	void clearModel();

	~Model() {
		this->clearModel();
	}
};