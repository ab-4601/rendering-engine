#include "MouseSelector.h"

MouseSelector::MouseSelector(unsigned int windowWidth, unsigned int windowHeight) {
	this->pixelInfo.init(windowWidth, windowHeight);
}

void MouseSelector::pickingPhase(std::vector<Mesh*>& meshes, const glm::mat4& projection,
	const glm::mat4& view, GLuint framebuffer) 
{
	this->pixelInfo.enableWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	this->shader.useProgram();

	this->shader.setMat4("projection", projection);
	this->shader.setMat4("view", view);

	for (size_t i = 0; i < meshes.size(); i++) {
		this->shader.setUint("objectIndex", meshes[i]->getObjectID() + 1);
		this->shader.setUint("drawIndex", static_cast<uint>(i));
		this->shader.setMat4("model", meshes[i]->getModelMatrix());

		meshes[i]->renderMesh(GL_TRIANGLES);
	}

	this->pixelInfo.disableWriting(framebuffer);

	this->shader.endProgram();
}

int MouseSelector::mouseSelectionResult(int windowHeight, int x, int y) {
	int clickedObjectID = -1;
	
	SelectionTexture::PixelInfo pixel = this->pixelInfo.readPixel(x, windowHeight - y - 1);

	if (pixel.objectID != 0) {
		clickedObjectID = pixel.objectID - 1;
		
		return clickedObjectID;
	}

	return -1;
}