#include "CoordinateSystem.h"

CoordinateSystem::CoordinateSystem() {
    this->axes.clear();
}

void CoordinateSystem::createCoordinateSystem() {
    glm::vec3 RED(1.f, 0.f, 0.f), GREEN(0.f, 1.f, 0.f), BLUE(0.f, 0.1f, 1.f);
    glm::vec3 origin(0.f, 0.f, 0.f);

    Line* X = new Line(glm::vec3(0.5f, 0.f, 0.f), origin, RED);
    Line* Y = new Line(glm::vec3(0.f, 0.5f, 0.f), origin, GREEN);
    Line* Z = new Line(glm::vec3(0.f, 0.f, 0.5f), origin, BLUE);

    Line* XNegative = new Line(glm::vec3(-0.5f, 0.f, 0.f), origin, RED);
    Line* YNegative = new Line(glm::vec3(0.f, -0.5f, 0.f), origin, GREEN);
    Line* ZNegative = new Line(glm::vec3(0.f, 0.f, -0.5f), origin, BLUE);

    X->createMesh();
    Y->createMesh();
    Z->createMesh();

    XNegative->createMesh();
    YNegative->createMesh();
    ZNegative->createMesh();

    this->axes.push_back(X);
    this->axes.push_back(XNegative);

    this->axes.push_back(Y);
    this->axes.push_back(YNegative);

    this->axes.push_back(Z);
    this->axes.push_back(ZNegative);
}

void CoordinateSystem::drawCoordinateSystem(GLint windowHeight, GLint windowWidth, GLint bufferWidth,
    GLint bufferHeight, const Camera* const camera, glm::mat4& model, const glm::mat4& projection) {

    glUseProgram(this->shader.getProgramID());

    model = glm::mat4(1.f);

    glUniformMatrix4fv(shader.getUniformProjection(), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(shader.getUniformView(), 1, GL_FALSE, glm::value_ptr((camera->generateViewMatrix())));

    model = glm::scale(model, glm::vec3(INT_MAX, INT_MAX, INT_MAX));
    glUniformMatrix4fv(shader.getUniformModel(), 1, GL_FALSE, glm::value_ptr(model));

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(5.f);

    for (const auto& elem : this->axes) {
        glUniform3fv(shader.getUniformColor(), 1, glm::value_ptr(elem->getColor()));
        elem->renderMesh();
    }

    model = glm::mat4(1.f);

    glm::vec3 axisPosition = camera->getCameraPosition() + glm::normalize(camera->getCameraLookDirection()) * 0.5f;
    model = glm::translate(model, axisPosition);
    model = glm::scale(model, glm::vec3(2.f, 2.f, 2.f));

    glUniformMatrix4fv(shader.getUniformModel(), 1, GL_FALSE, glm::value_ptr(model));

    glViewport(windowWidth / 50, windowHeight / 50, bufferWidth / 12, bufferHeight / 12);

    glLineWidth(3.f);
    for (const auto& elem : this->axes) {
        glUniform3fv(shader.getUniformColor(), 1, glm::value_ptr(elem->getColor()));
        elem->renderMesh();
    }

    glViewport(0, 0, bufferWidth, bufferHeight);

    glLineWidth((GLfloat)1.f);
    glDisable(GL_LINE_SMOOTH);
}

CoordinateSystem::~CoordinateSystem() {
    for (auto& elem : this->axes)
        delete elem;
}