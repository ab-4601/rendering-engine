#pragma once

#include "Core.h"
#include "Mesh.h"
#include "Texture.h"

class Model {
private:
	std::vector<Mesh*> meshList;
	std::vector<Texture*> textureList;
	std::vector<Texture*> normalMaps;
	std::vector<unsigned int> meshToTex;

	std::vector<GLfloat> vertices;
	std::vector<uint> indices;

	void _loadNode(aiNode* node, const aiScene* const scene);
	void _loadMesh(aiMesh* mesh, const aiScene* const scene);
	void _loadMaterialTextures(const aiScene* const scene);
	void _loadMaterialNormalMaps(const aiScene* const scene);

public:
	Model(std::string fileName = "");

	void loadModel(std::string fileName = "");
	void renderModel();
	void clearModel();

	~Model() {
		this->clearModel();
	}
};