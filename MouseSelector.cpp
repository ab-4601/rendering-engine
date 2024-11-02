#include "MouseSelector.h"

MouseSelector::MouseSelector(unsigned int windowWidth, unsigned int windowHeight) {
	this->pixelInfo.init(windowWidth, windowHeight);
}

void MouseSelector::pickingPhase(std::vector<Mesh*>& meshes, const glm::mat4& projection,
	const glm::mat4& view, GLuint framebuffer) 
{
	this->pixelInfo.enableWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(this->shader.getProgramID());

	glUniformMatrix4fv(this->shader.getUniformProjection(), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(this->shader.getUniformView(), 1, GL_FALSE, glm::value_ptr(view));

	for (size_t i = 0; i < meshes.size(); i++) {
		glUniform1ui(this->shader.getUniformObjectIndex(), meshes[i]->getObjectID() + 1);
		glUniform1ui(this->shader.getUniformDrawIndex(), static_cast<unsigned int>(i));
		glUniformMatrix4fv(this->shader.getUniformModel(), 1, GL_FALSE, glm::value_ptr(meshes[i]->getModelMatrix()));

		meshes[i]->renderMesh(GL_TRIANGLES);
	}

	this->pixelInfo.disableWriting(framebuffer);
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