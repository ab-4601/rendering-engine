// main.cpp : This file contains the 'main' function. Program execution begins and ends there.

#define STB_IMAGE_IMPLEMENTATION

#include "Window.h"
#include "Camera.h"
#include "Skybox.h"
#include "Overlay.h"
#include "HDR.h"
#include "MouseSelector.h"
#include "SelectionTexture.h"
#include "CoordinateSystem.h"
#include "Icosphere.h"
#include "Cube.h"
#include "UVSphere.h"
#include "Terrain.h"
#include "LightSources.h"
#include "LightMeshShader.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "ParticleSystem.h"
#include "Crosshair.h"
#include "Grid.h"
#include "Water.h"

unsigned int Mesh::meshCount = 0;
std::vector<Mesh*> Mesh::meshList = {};

using namespace std;

const float toRadians = (float)PI / 180;
float rotationAngle = 1.f;

GLfloat deltaTime = 0.f;
GLfloat lastTime = 0.f;
GLfloat currTime = 0.f;
GLfloat prevTime = 0.f;

int main() {
    srand((unsigned int)time(0));

    glm::vec3 lightDirection(5000, 5000, 5000);
    glm::vec3 pointLightPosition1(20.0, 20.f, 20.f);
    glm::vec3 pointLightPosition2(100.f, 30.f, 100.f);
    glm::vec3 spotLightPosition(300.0, 80.f, 300.f);

    Camera camera{ {0, 10, -10} };
    Window window;
    Overlay overlay;
    HDR hdrBuffer;
    Grid grid;
    MouseSelector selection(window.getBufferWidth(), window.getBufferHeight());
    CoordinateSystem coordSystem;
    Skybox skybox;
    DirectionalLight mainLight{ 0.1f, 0.9f, lightDirection };
    LightSources lightSources;

    std::vector<Mesh*> meshes{};

    GLfloat aspect = (float)window.getBufferWidth() / window.getBufferHeight();

    glm::mat4 model(1.f);
    glm::mat4 projection = glm::perspective<GLfloat>(45.f, aspect, 0.1f, (float)INT_MAX);
    glm::mat4 view = camera.generateViewMatrix();

    glm::vec3 color(0.f, 0.f, 0.f);

    std::vector<PointLight> pointLights(MAX_POINT_LIGHTS, NULL);
    std::vector<SpotLight> spotLights(MAX_SPOT_LIGHTS, NULL);

    unsigned int pointLightCount = 0;
    unsigned int spotLightCount = 0;

    int index{};

    pointLights.at(0) = PointLight(0.1f, 1.f, pointLightPosition1, 1.f, 0.001f, 0.001f);
    pointLightCount++;

    pointLights.at(1) = PointLight(0.1f, 1.f, pointLightPosition2, 1.f, 0.0001f, 0.0001f);
    pointLightCount++;

    spotLights.at(0) = SpotLight(0.1f, 1.f, spotLightPosition, 1.f, 0.0001f, 0.0001f, { 2.f, 2.f, 2.f }, { 0.f, -1.f, 0.f }, 45);
    spotLightCount++;

    // Get shader source code, compile and link to shader program
    LightMeshShader lightMeshShader;
    LightingShader shader("blinn-phong.vert", "blinn-phong.frag", "blinn-phong.geom");

    bool drawSkybox = false;

    GLuint gridSize = 500;

    Icosphere sphere;
    sphere.smoothSphere(5);
    sphere.scale(10.f);
    sphere.setColor({ 1.f, 0.05f, 0.05f });
    sphere.createMeshWithNormals();

    model = glm::mat4(1.f);
    model = glm::translate(model, glm::vec3(20.f, 50.f, 20.f));

    sphere.setModelMatrix(model);

    Cube cube;
    cube.setColor({ 1.f, 0.05f, 0.05f });
    cube.createUnindexedMesh();

    /*Terrain terrain{ gridSize, gridSize, 100 };
    terrain.setColor({ 1.f, 0.05f, 0.05f });
    terrain.generateHeightMaps(3);
    terrain.generateTerrain();
    terrain.createMeshFinal();*/

    meshes = Mesh::meshList;

    coordSystem.createCoordinateSystem();

    /*UVSphere sphere(50, 50, 5);
    sphere.generateSphere();
    sphere.createMeshWithNormals();*/

    /*ParticleTexture partTex("Textures/particleAtlas.png", 4.f);
    glm::vec3 particlePosition{ 20.f, 20.f, 20.f }, velocity{ 10.f, 50.f, 10.f }, particleColor{ 1.f, 0.5f, 0.05f };
    ParticleSystem pSystem(particleColor, 10, 0.f, 1.f, 2.f, partTex);*/

    ImGuiIO& io = overlay._init(window.getGlfwWindow());

    /*Texture soilTex{ "Textures/muddy-terrain.png" };
    soilTex.loadTexture();*/

    hdrBuffer._init(window.getBufferWidth(), window.getBufferHeight());

    // main render loop
    while (!glfwWindowShouldClose(window.getMainWindow())) {
        // Calculate delta time
        currTime = (GLfloat)glfwGetTime();
        deltaTime = currTime - lastTime;
        lastTime = currTime;

        glfwPollEvents();

        camera.keyFunctionality(window.getCurrWindow(), deltaTime);
        camera.mouseFunctionality(window.getXChange(), window.getYChange(), window.getScrollChange());

        view = camera.generateViewMatrix();

        hdrBuffer.enableHDRWriting();

        // Clear window
        glClearColor(0.005f, 0.005f, 0.01f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        overlay._newFrame();

        if (drawSkybox)
            skybox.renderSkybox(projection, camera);

// ----------------------------------------------------------------------------------------------------------------

        lightSources.renderLightSources(projection, view, mainLight, pointLights, spotLights, 
            pointLightCount, spotLightCount);

// ----------------------------------------------------------------------------------------------------------------

        selection.pickingPhase(meshes, projection, view, hdrBuffer.getFramebufferID());

        cube.setShader(mainLight, pointLights, pointLightCount, spotLights, spotLightCount, projection, view);

        //soilTex.useTexture();

        glm::vec2 mouseClickCoords = window.getViewportCoord();

        index = selection.mouseSelectionResult(window.getWindowHeight(), mouseClickCoords.x, mouseClickCoords.y);

        if (index < meshes.size() && index != -1) {
            overlay._updateTransformOperation(window);

            overlay.renderTransformWidget(window.getWindowWidth(), window.getWindowHeight(), projection, view,
                meshes[index]->getModelMatrix());

            meshes[index]->renderMeshWithOutline(GL_TRIANGLES, projection, view, mainLight, pointLights,
                pointLightCount, spotLights, spotLightCount);
        }

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        for (size_t i = 0; i < meshes.size(); i++) {
            if ((int)i != index)
                meshes[i]->renderMesh(GL_TRIANGLES);
        }

        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

// ----------------------------------------------------------------------------------------------------------------

        //water.render(projection, model, camera);

// ----------------------------------------------------------------------------------------------------------------

        grid.renderGrid(model, projection, view, camera.getCameraPosition());

// ----------------------------------------------------------------------------------------------------------------

        coordSystem.drawCoordinateSystem(window.getWindowHeight(), window.getWindowWidth(),
            window.getBufferWidth(), window.getBufferHeight(), &camera, model, projection);

// ----------------------------------------------------------------------------------------------------------------

        overlay.renderGUIWindow(io, drawSkybox);

        hdrBuffer.disableHDRWriting();
        hdrBuffer.renderToDefaultBuffer();

        glfwSwapBuffers(window.getMainWindow());
    }

    return 0;
}