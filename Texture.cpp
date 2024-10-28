#include "Texture.h"

Texture::Texture(std::string fileLocation)
	: fileLocation{ fileLocation }, width{ 0 }, height{ 0 }, bitDepth{ 0 }, textureID{ 0 } {

}

Texture::Texture(const Texture& source) noexcept
	: Texture(source.fileLocation) {

}

Texture::Texture(Texture&& source) noexcept
	: Texture(source.fileLocation) {

	source.~Texture();
}

Texture& Texture::operator=(const Texture& rhs) {
	if (this == &rhs)
		return *this;

	this->fileLocation = rhs.fileLocation;

	return *this;
}

Texture& Texture::operator=(Texture&& rhs) noexcept {
	if (this == &rhs)
		return *this;

	this->fileLocation = rhs.fileLocation;

	rhs.~Texture();

	return *this;
}

bool Texture::loadTexture() {
	unsigned char* texData = stbi_load(this->fileLocation.data(), &this->width, &this->height, &this->bitDepth, 4);

	if (!texData) {
		std::cerr << "Invalid file location. Error loading file" << std::endl;
		return false;
	}

	glGenTextures(1, &this->textureID);
	glBindTexture(GL_TEXTURE_2D, this->textureID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(texData);

	return true;
}

void Texture::useTexture() const {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textureID);
}

void Texture::useNormalMap() const {
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->textureID);
}

void Texture::useDepthMap() const {
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, this->textureID);
}

void Texture::clearTexture() {
	glDeleteTextures(1, &this->textureID);

	this->textureID = 0;
	this->width = 0;
	this->height = 0;
	this->bitDepth = 0;
	this->fileLocation = "";
}

Texture::~Texture() {
	this->clearTexture();
}