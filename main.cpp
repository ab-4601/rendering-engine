// main.cpp : This file contains the 'main' function. Program execution begins and ends there.

#define STB_IMAGE_IMPLEMENTATION

#include "Window.h"
#include "Camera.h"
#include "Skybox.h"
#include "Overlay.h"
#include "HDR.h"
#include "Bloom.h"
#include "MouseSelector.h"
#include "SelectionTexture.h"
#include "CoordinateSystem.h"
#include "Icosphere.h"
#include "Cube.h"
#include "UVSphere.h"
#include "Model.h"
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

uint Mesh::meshCount = 0;
std::vector<Mesh*> Mesh::meshList = {};

using namespace std;

const float toRadians = (float)PI / 180;
float rotationAngle = 1.f;

GLfloat deltaTime = 0.f;
GLfloat lastTime = 0.f;
GLfloat currTime = 0.f;
GLfloat prevTime = 0.f;

int main() {
    srand((uint)time(0));

    glm::vec3 lightDirection(5000, 5000, 5000);
    glm::vec3 pointLightPosition1(20.0, 20.f, 20.f);
    glm::vec3 pointLightPosition2(100.f, 30.f, 100.f);
    glm::vec3 spotLightPosition(300.0, 80.f, 300.f);

    Camera camera{ {0, 10, -10} };
    Window window;
    Overlay overlay;
    HDR hdrBuffer;
    Bloom bloom(true);
    Crosshair crosshair;
    Grid grid;
    MouseSelector selection(window.getBufferWidth(), window.getBufferHeight());
    CoordinateSystem coordSystem;
    Skybox skybox;
    DirectionalLight mainLight{ 0.01f, 0.5f, lightDirection };
    LightSources lightSources;

    std::vector<Mesh*> meshes{};

    GLfloat aspect = (float)window.getBufferWidth() / window.getBufferHeight();

    glm::mat4 model(1.f);
    glm::mat4 projection = glm::perspective<GLfloat>(45.f, aspect, 0.1f, (float)10000);
    glm::mat4 view = camera.generateViewMatrix();

    glm::vec3 color(0.f, 0.f, 0.f);

    std::vector<PointLight> pointLights(MAX_POINT_LIGHTS, NULL);
    std::vector<SpotLight> spotLights(MAX_SPOT_LIGHTS, NULL);

    uint pointLightCount = 0;
    uint spotLightCount = 0;

    int index{-1}, prevIndex{-1};

    /*pointLights.at(0) = PointLight(0.01f, 0.7f, pointLightPosition1, 1.f, 0.001f, 0.001f);
    pointLightCount++;

    pointLights.at(1) = PointLight(0.01f, 0.7f, pointLightPosition2, 1.f, 0.0001f, 0.0001f);
    pointLightCount++;

    spotLights.at(0) = SpotLight(0.01f, 0.7f, spotLightPosition, 1.f, 0.0001f, 0.0001f, { 0.f, 0.f, 1.f }, { 0.f, -1.f, 0.f }, 45);
    spotLightCount++;*/

    // Get shader source code, compile and link to shader program
    LightMeshShader lightMeshShader;
    LightingShader shader("blinn-phong.vert", "blinn-phong.frag", "blinn-phong.geom");

    bool drawSkybox = false;
    bool enableBloom = true;

    GLuint gridSize = 250;

    Model sponza("Models/sponza.obj");

    Icosphere sphere;
    sphere.smoothSphere(5);
    sphere.setColor({ 1.f, 0.02f, 0.02f });
    sphere.createMeshWithNormals();

    model = glm::mat4(1.f);
    model = glm::translate(model, glm::vec3(5.f, 0.f, 5.f));

    sphere.setModelMatrix(model);

    Cube cube;
    cube.setColor({ 0.01f, 1.f, 1.f });
    cube.createUnindexedMesh();

    /*Texture gridTex("Textures/prototype.png");
    gridTex.loadTexture();

    Terrain terrain{ gridSize, gridSize, 80 };
    terrain.setColor({ 0.1f, 1.f, 1.f });
    terrain.generateHeightMaps(3);
    terrain.generateTerrain();
    terrain.createMesh();*/

    meshes = Mesh::meshList;

    coordSystem.createCoordinateSystem();

    /*UVSphere sphere(50, 50, 5);
    sphere.generateSphere();
    sphere.createMeshWithNormals();*/

    for (size_t i = 0; i < meshes.size(); i++)
        meshes[i]->setMeshMaterial(1.f, 32.f);


    ParticleTexture partTex("Textures/particleAtlas.png", 4.f);
    glm::vec3 particlePosition{ 20.f, 20.f, 20.f }, velocity{ 10.f, 50.f, 10.f }, particleColor{ 1.f, 0.5f, 0.05f };
    ParticleSystem pSystem(particleColor, 10, 5.f, 1.f, 3.f, partTex);

    ImGuiIO& io = overlay._init(window.getGlfwWindow());

    Texture soilTex{ "Textures/muddy-terrain.png" };
    soilTex.loadTexture();

    hdrBuffer._initMSAA(window.getBufferWidth(), window.getBufferHeight());
    bloom._init(window.getBufferWidth(), window.getBufferHeight());

    std::chrono::milliseconds delay{ 1 };

    // main render loop
    while (!glfwWindowShouldClose(window.getMainWindow())) {
        std::this_thread::sleep_for(delay);
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
        glClearColor(0.f, 0.005f, 0.005f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        overlay._newFrame();

// ----------------------------------------------------------------------------------------------------------------

        lightSources.renderLightSources(projection, view, mainLight, pointLights, spotLights,
            pointLightCount, spotLightCount);

// ----------------------------------------------------------------------------------------------------------------

        particlePosition = camera.getCameraPosition() + camera.getCameraLookDirection() * 90.f;

        if (window.getKeyPress(GLFW_KEY_R)) {
            pSystem.generateParticles(particlePosition, 0.f);
        }

        pSystem.updateParticles(deltaTime, camera.getCameraPosition());
        pSystem.renderParticles(&window, &camera, model, projection);

// ----------------------------------------------------------------------------------------------------------------

        selection.pickingPhase(meshes, projection, view, hdrBuffer.getFramebufferID());

        meshes[0]->setShader(mainLight, pointLights, pointLightCount, spotLights, spotLightCount, projection, view,
            camera.getCameraPosition());

        sponza.renderModel();

        //sponza.renderModel();

        //soilTex.useTexture();

        glm::vec2 mouseClickCoords = window.getViewportCoord();

        if (window.getKeyPress(GLFW_KEY_TAB))
        {
            index = -1;
            prevIndex = -1;
        }

        if (window.getLMBPressed()) {
            index = selection.mouseSelectionResult(window.getWindowHeight(), mouseClickCoords.x, mouseClickCoords.y);

            if (prevIndex != index)
                if(index != -1)
                    prevIndex = index;
            else
                index = prevIndex;
        }

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        /*for (size_t i = 0; i < meshes.size(); i++) {
            if ((int)i != index)
                meshes[i]->renderMesh(GL_TRIANGLES);
        }*/

        glBindTexture(GL_TEXTURE_2D, 0);

        bloom.processFramebufferMSAA(10, hdrBuffer.getColorbufferID(1), hdrBuffer.getFramebufferID(),
            window.getWindowWidth(), window.getWindowHeight());

        meshes[0]->setShader(mainLight, pointLights, pointLightCount, spotLights, spotLightCount, projection, view,
            camera.getCameraPosition());

        //soilTex.useTexture();

        if (index < meshes.size() && index != -1) {
            meshes[index]->renderMeshWithOutline(GL_TRIANGLES, projection, view, mainLight, pointLights,
                pointLightCount, spotLights, spotLightCount, camera.getCameraPosition());

            overlay._updateTransformOperation(window);

            overlay.renderTransformWidget(window.getWindowWidth(), window.getWindowHeight(), projection, view,
                meshes[index]->getModelMatrix());
        }

        //glBindTexture(GL_TEXTURE_2D, 0);

        //bloom.processFramebuffer(10, hdrBuffer.getColorbufferID(1), hdrBuffer.getFramebufferID());

        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

// ----------------------------------------------------------------------------------------------------------------

        //water.render(projection, model, camera);

// ----------------------------------------------------------------------------------------------------------------

        if (drawSkybox)
            skybox.renderSkybox(projection, camera);

// ----------------------------------------------------------------------------------------------------------------

        grid.renderGrid(model, projection, view, camera.getCameraPosition());

// ----------------------------------------------------------------------------------------------------------------

        coordSystem.drawCoordinateSystem(window.getWindowHeight(), window.getWindowWidth(),
            window.getBufferWidth(), window.getBufferHeight(), &camera, model, projection);

// ----------------------------------------------------------------------------------------------------------------

        //crosshair.drawCrosshair();

// ----------------------------------------------------------------------------------------------------------------

        overlay.renderGUIWindow(io, drawSkybox, enableBloom);

        hdrBuffer.disableHDRWriting();
        //hdrBuffer.renderToDefaultBuffer(1.f, bloom.getColorBuffers(), bloom.getBlurFlag(), enableBloom);
        hdrBuffer.renderToDefaultBufferMSAA(
            1.f, bloom.getColorBuffers(), bloom.getBlurFlag(), window.getWindowWidth(), window.getWindowHeight(),
        enableBloom);

        glfwSwapBuffers(window.getMainWindow());
    }

    return 0;
}