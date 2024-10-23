#pragma once

#include "Core.h"
#include "Mesh.h"
#include "Texture.h"

class Model : public Mesh {
private:
	std::vector<Mesh*> meshList;
	std::vector<Texture*> textureList;
	std::vector<unsigned int> meshToTex;

	void _loadNode(aiNode* node, const aiScene* const scene);
	void _loadMesh(aiMesh* mesh, const aiScene* const scene);
	void _loadMaterial(const aiScene* const scene);

public:
	Model();

	void loadModel(const std::string& fileName = "");
	void clearModel();

	~Model() = default;
};