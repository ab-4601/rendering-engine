#pragma once

#include "Core.h"
#include "stb_image.h"

class Texture {
protected:
	GLuint textureID;
	int width, height, bitDepth;

	std::string fileLocation;

public:
	Texture(std::string fileLocation = "");

	Texture(const Texture& source) noexcept;
	Texture(Texture&& source) noexcept;

	Texture& operator=(const Texture& rhs);
	Texture& operator=(Texture&& rhs) noexcept;

	bool loadTexture();
	void useTexture();
	void clearTexture();

	inline GLuint getTextureID() const { return this->textureID; }
	inline int getWidth() const { return this->width; }
	inline int getHeight() const { return this->height; }
	inline int getBitDepth() const { return this->bitDepth; }

	~Texture();
};

