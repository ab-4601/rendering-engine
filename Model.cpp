#include "Model.h"

Model::Model(std::string fileName) {
	this->meshList.clear();
	this->meshToTex.clear();
	this->textureList.clear();

	this->loadModel(fileName);
}

void Model::_loadNode(aiNode* node, const aiScene* const scene) {
	for (size_t i = 0; i < node->mNumMeshes; i++)
		this->_loadMesh(scene->mMeshes[node->mMeshes[i]], scene);

	for (size_t i = 0; i < node->mNumChildren; i++)
		this->_loadNode(node->mChildren[i], scene);
}

void Model::_loadMesh(aiMesh* mesh, const aiScene* const scene) {
	this->vertices.clear();
	this->indices.clear();

	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		this->vertices.insert(this->vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });
		
		if (mesh->mTextureCoords[0])
			this->vertices.insert(this->vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
		else
			this->vertices.insert(this->vertices.end(), { 0.f, 0.f });

		this->vertices.insert(this->vertices.end(), { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z });
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++) {
			this->indices.push_back(face.mIndices[j]);
		}
	}

	Mesh* currMesh = new Mesh();
	currMesh->setVertices(this->vertices);
	currMesh->setIndices(this->indices);
	currMesh->createMesh();

	this->meshList.push_back(currMesh);
	this->meshToTex.push_back(mesh->mMaterialIndex);
}

void Model::_loadMaterial(const aiScene* const scene) {
	this->textureList.resize(scene->mNumMaterials);

	for (size_t i = 0; i < scene->mNumMaterials; i++) {
		aiMaterial* material = scene->mMaterials[i];

		this->textureList[i] = nullptr;

		if (material->GetTextureCount(aiTextureType_DIFFUSE)) {
			aiString path;

			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Models/") + filename;

				this->textureList[i] = new Texture(texPath);

				if (!this->textureList[i]->loadTexture()) {
					std::cerr << "Failed to load texture at: " << texPath << std::endl;

					delete this->textureList[i];
					this->textureList[i] = nullptr;
				}
			}
		}

		if (this->textureList[i] == nullptr) {
			textureList[i] = new Texture("Textures/prototype.png");
			textureList[i]->loadTexture();
		}
	}
}

void Model::loadModel(std::string fileName) {
	if (fileName == "") {
		std::cout << "No file path specified" << std::endl;
		return;
	}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

	if (!scene) {
		std::cerr << fileName + " failed to load" << std::endl;
		std::cerr << importer.GetErrorString() << std::endl;
		return;
	}

	this->_loadNode(scene->mRootNode, scene);
	this->_loadMaterial(scene);
}

void Model::renderModel() {
	for (size_t i = 0; i < this->meshList.size(); i++) {
		unsigned int materialIndex = this->meshToTex[i];

		if (materialIndex < this->textureList.size() && textureList[materialIndex]) {
			this->textureList[materialIndex]->useTexture();
		}

		meshList[i]->renderMesh(GL_TRIANGLES);
	}
}

void Model::renderModelWithOutline() {
	
}

void Model::clearModel() {
	for (size_t i = 0; i < this->meshList.size(); i++) {
		if (this->meshList[i]) {
			delete this->meshList[i];
			this->meshList[i] = nullptr;
		}
	}

	for (size_t i = 0; i < this->textureList.size(); i++) {
		if (this->textureList[i]) {
			delete this->textureList[i];
			this->textureList[i] = nullptr;
		}
	}
}