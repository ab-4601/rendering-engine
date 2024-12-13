#pragma once

#include "Core.h"
#include "stb_image.h"

constexpr auto FOURCC_DXT1 = 0x31545844;
constexpr auto FOURCC_DXT3 = 0x33545844;
constexpr auto FOURCC_DXT5 = 0x35545844;
constexpr auto FOURCC_ATI1N = 0x31495441;
constexpr auto FOURCC_ATI2N = 0x32495441;
constexpr auto FOURCC_BC6H = 0x48364342;
constexpr auto FOURCC_BC7 = 0x75364342;

struct DDSHeader {
	uint32_t magic;
	uint32_t size;
	uint32_t flags;
	uint32_t height;
	uint32_t width;
	uint32_t pitchOrLinearSize;
	uint32_t depth;
	uint32_t mipMapCount;
	uint32_t reserved[11];
	
	struct {
		uint32_t size;
		uint32_t flags;
		uint32_t fourCC;
		uint32_t rgbBitCount;
		uint32_t rBitMask;
		uint32_t gBitMask;
		uint32_t bBitMask;
		uint32_t aBitMask;
	} pixelFormat;

	uint32_t caps[4];
	uint32_t reserved2;
};

class Texture {
protected:
	GLuint textureID;
	int width, height, bitDepth;

	std::string fileLocation;
	bool isCompressed = false;

public:
	Texture(std::string fileLocation = "");

	Texture(const Texture& source) noexcept;
	Texture(Texture&& source) noexcept;

	Texture& operator=(const Texture& rhs);
	Texture& operator=(Texture&& rhs) noexcept;

	bool loadDDSTexture();
	bool loadTexture();
	void useTexture(GLenum textureLocation = GL_TEXTURE0) const;
	void clearTexture();

	inline GLuint getTextureID() const { return this->textureID; }
	inline int getWidth() const { return this->width; }
	inline int getHeight() const { return this->height; }
	inline int getBitDepth() const { return this->bitDepth; }

	~Texture();
};

