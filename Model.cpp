#include "Model.h"

Model::Model(std::string fileName, std::string texFolderPath) : texFolderPath{ texFolderPath } {
	this->meshList.clear();
	this->meshToTex.clear();
	this->diffuseMaps.clear();
	this->heightMaps.clear();

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

		this->vertices.insert(this->vertices.end(), { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z });
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++) {
			this->indices.push_back(face.mIndices[j]);
		}
	}

	Mesh* currMesh = new Mesh();
	currMesh->setObjectID(-1);
	currMesh->setVertices(this->vertices);
	currMesh->setIndices(this->indices);
	currMesh->setMeshMaterial(1.f, 0.f, 0.f);
	currMesh->createModel();

	this->meshList.push_back(currMesh);
	this->meshToTex.push_back(mesh->mMaterialIndex);
}

void Model::_loadMaterialMap(const aiScene* const scene, std::vector<Texture*>& maps, aiTextureType textureType) {
	maps.resize(scene->mNumMaterials);

	for (size_t i = 0; i < scene->mNumMaterials; i++) {
		aiMaterial* material = scene->mMaterials[i];
		maps[i] = nullptr;

		if (material->GetTextureCount(textureType)) {
			aiString path;

			if (material->GetTexture(textureType, 0, &path) == AI_SUCCESS) {
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				/*int extIdx = filename.rfind(".");
				std::string extension = filename.substr(extIdx + 1);

				if (extension != "jpg" || extension != "png") {
					filename.replace(extIdx + 1, 3, "jpg");
				}*/

				std::string texPath = this->texFolderPath + filename;

				maps[i] = new Texture(texPath);

				if (!maps[i]->loadTexture()) {
					std::cerr << "Failed to load material at: " << texPath << std::endl;

					delete maps[i];
					maps[i] = nullptr;
				}
			}
		}

		if (maps[i] == nullptr) {
			maps[i] = new Texture("Textures/prototype.png");
			maps[i]->loadTexture();
		}
	}
}

void Model::loadModel(std::string fileName) {
	if (fileName == "") {
		std::cout << "No file path specified" << std::endl;
		return;
	}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName, ASSIMP_LOAD_FLAGS);

	if (!scene) {
		std::cerr << fileName + " failed to load" << std::endl;
		std::cerr << importer.GetErrorString() << std::endl;
		return;
	}

	this->_loadNode(scene->mRootNode, scene);
	this->_loadMaterialMap(scene, this->diffuseMaps, aiTextureType_DIFFUSE);
	this->_loadMaterialMap(scene, this->normalMaps, aiTextureType_NORMALS);
	this->_loadMaterialMap(scene, this->metalnessMaps, aiTextureType_METALNESS);
	this->_loadMaterialMap(scene, this->heightMaps, aiTextureType_DIFFUSE_ROUGHNESS);
}

void Model::renderModel(glm::mat4 lightSpaceTransform, GLuint directionalShadowMap,
	GLuint pointShadowMap, GLuint irradianceMap, GLuint brdfMap, GLuint prefilterMap) 
{
	for (size_t i = 0; i < this->meshList.size(); i++) {
		uint materialIndex = this->meshToTex[i];

		if (materialIndex < this->diffuseMaps.size() && diffuseMaps[materialIndex]) {
			this->diffuseMaps[materialIndex]->useTexture();
		}

		if (materialIndex < this->normalMaps.size() && this->normalMaps[materialIndex]) {
			this->normalMaps[materialIndex]->useNormalMap();
		}

		if (materialIndex < this->heightMaps.size() && this->heightMaps[materialIndex]) {
			this->heightMaps[materialIndex]->useDepthMap();
		}

		if (materialIndex < this->metalnessMaps.size() && this->metalnessMaps[materialIndex]) {
			this->metalnessMaps[materialIndex]->useMetalnessMap();
		}

		meshList[i]->renderMesh(
			GL_TRIANGLES, lightSpaceTransform, directionalShadowMap, pointShadowMap, irradianceMap, brdfMap, prefilterMap
		);

		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Model::clearModel() {
	for (size_t i = 0; i < this->meshList.size(); i++) {
		if (this->meshList[i]) {
			delete this->meshList[i];
			this->meshList[i] = nullptr;
		}
	}

	for (size_t i = 0; i < this->diffuseMaps.size(); i++) {
		if (this->diffuseMaps[i]) {
			delete this->diffuseMaps[i];
			this->diffuseMaps[i] = nullptr;
		}
	}

	for (size_t i = 0; i < this->diffuseMaps.size(); i++) {
		if (this->normalMaps[i]) {
			delete this->normalMaps[i];
			this->normalMaps[i] = nullptr;
		}
	}
}