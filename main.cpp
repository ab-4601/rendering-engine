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
#include "DirectionalShadow.h"
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
GLfloat elapsedTime = 0.f;

int main() {
    srand((uint)time(0));

    glm::vec3 lightDirection(1000, 5000, 0);
    glm::vec3 pointLightPosition1(20.0, 20.f, 20.f);
    glm::vec3 pointLightPosition2(100.f, 30.f, 100.f);
    glm::vec3 spotLightPosition(300.0, 80.f, 300.f);

    Camera camera{ {-300, 100, 0} };
    Window window;
    Overlay overlay;
    HDR hdrBuffer;
    Bloom bloom{ true };
    Crosshair crosshair;
    Grid grid;
    MouseSelector selection{ (uint)window.getBufferWidth(), (uint)window.getBufferHeight() };
    CoordinateSystem coordSystem;
    Skybox skybox;
    DirectionalLight mainLight{ 0.05f, 0.5f, lightDirection };
    LightSources lightSources;
    DirectionalShadow dirShadowMap{ 1700.f, 2000, ::far_plane};

    std::vector<Mesh*> meshes{};

    GLfloat aspect = (float)window.getBufferWidth() / window.getBufferHeight();

    glm::mat4 model(1.f);
    glm::mat4 projection = glm::perspective<GLfloat>(45.f, aspect, near_plane, far_plane);
    glm::mat4 view = camera.generateViewMatrix();

    glm::vec3 color(0.f, 0.f, 0.f);

    std::vector<PointLight> pointLights(MAX_POINT_LIGHTS, NULL);
    std::vector<SpotLight> spotLights(MAX_SPOT_LIGHTS, NULL);

    uint pointLightCount = 0;
    uint spotLightCount = 0;

    int index{ -1 }, prevIndex{ -1 };

    /*pointLights.at(0) = PointLight(0.01f, 0.4f, pointLightPosition1, 1.f, 0.001f, 0.001f);
    pointLightCount++;*/

    /*pointLights.at(1) = PointLight(0.01f, 0.4f, pointLightPosition2, 1.f, 0.0001f, 0.0001f);
    pointLightCount++;

    spotLights.at(0) = SpotLight(0.01f, 0.4f, spotLightPosition, 1.f, 0.0001f, 0.0001f, { 0.f, 0.f, 1.f }, { 0.f, -1.f, 0.f }, 45);
    spotLightCount++;*/

    // Get shader source code, compile and link to shader program
    LightMeshShader lightMeshShader;
    LightingShader shader("blinn-phong.vert", "blinn-phong.frag", "blinn-phong.geom");

    bool drawSkybox = true;
    bool enableBloom = true;
    bool drawWireframe = false;
    float exposure = 1.f;

    GLuint gridSize = 200;

    Icosphere sphere;
    sphere.smoothSphere(5);
    sphere.setColor({ 0.07f, 1.f, 1.f });
    sphere.setMeshMaterial(10.f, 4.f);
    sphere.createMeshWithNormals();

    model = glm::mat4(1.f);
    model = glm::translate(model, glm::vec3(250.f, 150.f, 0.f));
    model = glm::scale(model, glm::vec3(100.f, 100.f, 100.f));

    sphere.setModelMatrix(model);

    model = glm::mat4(1.f);
    model = glm::translate(model, glm::vec3(0.f, 100.f, 0.f));
    model = glm::scale(model, glm::vec3(100.f, 100.f, 100.f));

    Cube cube;
    cube.setColor({ 2.f, 0.07f, 0.07f });
    cube.setModelMatrix(model);
    cube.setMeshMaterial(10.f, 32.f);
    cube.createUnindexedMesh();

    model = glm::mat4(1.f);
    model = glm::translate(model, glm::vec3(0.f, 1.f, 0.f));
    //model = glm::scale(model, glm::vec3(100.f, 100.f, 100.f));

    /*Terrain terrain(gridSize, gridSize);
    terrain.setModelMatrix(model);
    terrain.generateHeightMaps(0);
    terrain.generateTerrain();
    terrain.setMeshMaterial(4.f, 32.f);
    terrain.setColor({ 0.5f, 0.5f, 0.5f });
    terrain.createMeshWithNormals();*/

    Model sponza("Models/Sponza/sponza.obj", "Models/Sponza/");

    meshes = Mesh::meshList;

    coordSystem.createCoordinateSystem();

    ParticleTexture partTex("Textures/particleAtlas.png", 4.f);
    glm::vec3 particlePosition{ 20.f, 20.f, 20.f }, velocity{ 10.f, 50.f, 10.f }, particleColor{ 1.f, 0.5f, 0.05f };
    ParticleSystem pSystem(particleColor, 10, 5.f, 1.f, 6.f, partTex);

    ParticleTexture fire("Textures/fire.png", 8.f);
    glm::vec3 fireParticlePosition{ 1125.f, 120.f, 400.f };
    ParticleSystem fireSystem(particleColor, 30, -30.f, 1.f, 30.f, fire);

    ImGuiIO& io = overlay._init(window.getGlfwWindow());

    hdrBuffer._initMSAA(window.getBufferWidth(), window.getBufferHeight());
    bloom._init(window.getBufferWidth(), window.getBufferHeight());
    dirShadowMap._init();

    // main render loop
    while (!glfwWindowShouldClose(window.getMainWindow())) {
        // Calculate delta time
        currTime = (GLfloat)glfwGetTime();
        deltaTime = currTime - lastTime;
        elapsedTime += deltaTime;
        lastTime = currTime;

        if (rotationAngle >= 360.f)
            rotationAngle = 0.f;
        else
            rotationAngle += 0.0001;

        glfwPollEvents();

        lightDirection.x = 2000.f * cosf(glm::radians(rotationAngle));
        lightDirection.z = 2000.f * sinf(glm::radians(rotationAngle));

        mainLight.updateLightLocation(lightDirection);

        camera.keyFunctionality(window.getCurrWindow(), deltaTime);
        camera.mouseFunctionality(window.getXChange(), window.getYChange(), window.getScrollChange());

        /*pSystem.updateParticles(deltaTime, camera.getCameraPosition());
        fireSystem.updateParticles(deltaTime, camera.getCameraPosition());*/

        if (elapsedTime >= 0.012f)
        {
            elapsedTime = 0.f;

            view = camera.generateViewMatrix();

            overlay._newFrame();

            hdrBuffer.enableHDRWriting();

            // Clear window
            glClearColor(0.f, 0.005f, 0.005f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            if (drawSkybox)
                skybox.renderSkybox(projection, camera);

// ----------------------------------------------------------------------------------------------------------------

            coordSystem.drawCoordinateSystem(window.getWindowHeight(), window.getWindowWidth(),
                window.getBufferWidth(), window.getBufferHeight(), &camera, model, projection);

// ----------------------------------------------------------------------------------------------------------------

            grid.renderGrid(model, projection, view, camera.getCameraPosition());

// ----------------------------------------------------------------------------------------------------------------

            lightSources.renderLightSources(projection, view, mainLight, pointLights, spotLights,
                pointLightCount, spotLightCount);

// ----------------------------------------------------------------------------------------------------------------

            dirShadowMap.calculateShadows(
                window.getWindowWidth(), window.getWindowHeight(), meshes, lightDirection, hdrBuffer.getFramebufferID()
            );

            selection.pickingPhase(meshes, projection, view, hdrBuffer.getFramebufferID());

            meshes[0]->setShader(mainLight, pointLights, pointLightCount, spotLights, spotLightCount, projection, view,
                camera.getCameraPosition());

            if (drawWireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            sponza.renderModel(dirShadowMap.getLightSpaceMatrix(), dirShadowMap.getDirectionalShadowMap());

            glm::vec2 mouseClickCoords = window.getViewportCoord();

            if (window.getKeyPress(GLFW_KEY_TAB))
            {
                index = -1;
                prevIndex = -1;
            }

            if (window.getLMBPressed()) {
                index = selection.mouseSelectionResult(window.getWindowHeight(), mouseClickCoords.x, mouseClickCoords.y);

                if (prevIndex != index)
                    if (index != -1)
                        prevIndex = index;
                    else
                        index = prevIndex;
            }

            for (size_t i = 0; i < meshes.size(); i++) {
                if ((int)i != index && meshes[i]->getObjectID() != -1)
                    meshes[i]->renderMesh(GL_TRIANGLES, dirShadowMap.getLightSpaceMatrix(),
                        dirShadowMap.getDirectionalShadowMap());
            }

            if (index < meshes.size() && index != -1) {
                overlay._updateTransformOperation(window);

                overlay.renderTransformWidget(window.getWindowWidth(), window.getWindowHeight(), projection, view,
                    meshes[index]);

                meshes[index]->renderMeshWithOutline(
                    GL_TRIANGLES, projection, view, mainLight, pointLights,
                    pointLightCount, spotLights, spotLightCount, camera.getCameraPosition(), 
                    dirShadowMap.getLightSpaceMatrix(), dirShadowMap.getDirectionalShadowMap()
                );
            }

            //glBindTexture(GL_TEXTURE_2D, 0);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

// ----------------------------------------------------------------------------------------------------------------

            /*particlePosition = camera.getCameraPosition() + camera.getCameraLookDirection() * 200.f;
            fireSystem.generateParticles(fireParticlePosition, 0.f);

            if (window.getKeyPress(GLFW_KEY_R)) {
                pSystem.generateParticles(particlePosition, 0.f);
            }

            pSystem.renderParticles(&window, &camera, model, projection);
            fireSystem.renderParticles(&window, &camera, model, projection);*/

            bloom.processFramebufferMSAA(10, hdrBuffer.getColorbufferID(1), hdrBuffer.getFramebufferID(),
                window.getWindowWidth(), window.getWindowHeight());

// ----------------------------------------------------------------------------------------------------------------

            //water.render(projection, model, camera);

// ----------------------------------------------------------------------------------------------------------------

            //crosshair.drawCrosshair();

// ----------------------------------------------------------------------------------------------------------------

            overlay.renderGUIWindow(io, exposure, drawSkybox, enableBloom, drawWireframe);

            hdrBuffer.disableHDRWriting();
            hdrBuffer.renderToDefaultBufferMSAA(
                exposure, bloom.getColorBuffers(), bloom.getBlurFlag(), window.getWindowWidth(), window.getWindowHeight(),
                enableBloom);

            glfwSwapBuffers(window.getMainWindow());
        }
    }

    return 0;
}