#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"

#include <iostream>
#include <random>
#include <vector>
#include <ctime>

// window
gps::Window myWindow;
int glWindowWidth = 1920;
int glWindowHeight = 1080;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// POINT LIGHT
glm::vec3 pointLightPosition;
glm::vec3 pointLightColor;
float constant = 1.0f;
float linear = 0.007f;
float quadratic = 0.0002f;
GLint constantLoc, linearLoc, quadraticLoc;
GLint pointLightPosLoc, pointLightColorLoc;

// A DOUA LUMINĂ DIRECȚIONALĂ
glm::vec3 secondLightDir;
glm::vec3 secondLightColor;
float secondLightAngle = 0.0f;
float secondLightRotationSpeed = 0.0f;
bool secondLightRotating = false;
GLint secondLightDirLoc, secondLightColorLoc;

// Variabile pentru animația de prezentare
bool presentationMode = false;
float presentationAngle = 0.0f;
float presentationRadius = 50.0f;
float presentationHeight = 18.0f;
float presentationSpeed = 15.0f;

// Variabile pentru ceață
bool fogEnabled = true;
float fogDensity = 0.03f;
glm::vec3 fogColor = glm::vec3(0.7f, 0.8f, 0.9f);

// Variabile pentru camera
glm::vec3 originalCameraPosition = glm::vec3(0.0f, 5.0f, 15.0f);
glm::vec3 originalCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 originalCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool savedOriginalPosition = false;

// Variabile pentru animația obiectului (Ceainic)
bool objectAnimation = false;
float objectRotationAngle = 0.0f;
float objectRotationSpeed = 90.0f;
glm::vec3 objectPosition = glm::vec3(0.0f, 0.0f, 0.0f);

// =================== VARIABILE PENTRU VULTUR ===================
glm::vec3 eaglePosition = glm::vec3(10.0f, 30.0f, 0.0f);
float eagleScaleFactor = 1.0f;
float eagleRotationAngle = 0.0f;
bool eagleSetupMode = true;
float eagleFlightSpeed = 30.0f;
float eagleFlightRadius = 0.0f;

// =================== VARIABILE PENTRU PLOAIE REALĂ ===================
bool rainEnabled = false;
int nrRaindrops = 10000;
float rainSpeed = 2.0f;
float windStrength = 0.3f;
float windDirection = 0.0f;

GLuint rainVAO = 0;
GLuint rainVBO = 0;
std::vector<glm::vec3> rainPositions;

// Uniform locations generale
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
GLint fogEnabledLoc;
GLint fogDensityLoc;
GLint fogColorLoc;
GLint cameraPosLoc;

// camera
gps::Camera myCamera(
    glm::vec3(4.3f, 4.1f, 0.4f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.5f;
GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
GLfloat angle = 0.0f;
gps::Model3D vultur;

// shaders
gps::Shader myBasicShader;
gps::Shader rainShader;

// Mouse & Time
double lastX = glWindowWidth / 2.0;
double lastY = glWindowHeight / 2.0;
bool firstMouse = true;
float mouseSensitivity = 0.1f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLenum glCheckError_(const char* file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
        case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void initRain() {
    rainPositions.clear();
    for (int i = 0; i < nrRaindrops; i++) {
        glm::vec3 pos = glm::vec3(
            (rand() % 200) - 100.0f,
            (rand() % 100) + 20.0f,
            (rand() % 200) - 100.0f
        );
        rainPositions.push_back(pos);
    }
    std::vector<glm::vec3> rainVertices;
    for (const auto& pos : rainPositions) {
        rainVertices.push_back(pos);
        rainVertices.push_back(pos + glm::vec3(0.0f, -2.0f, 0.0f));
    }
    glGenVertexArrays(1, &rainVAO);
    glBindVertexArray(rainVAO);
    glGenBuffers(1, &rainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rainVBO);
    glBufferData(GL_ARRAY_BUFFER, rainVertices.size() * sizeof(glm::vec3), rainVertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    rainShader.loadShader("shaders/rain.vert", "shaders/rain.frag");
}

void updateRain() {
    if (!rainEnabled || rainVAO == 0) return;
    std::vector<glm::vec3> rainVertices;
    for (auto& pos : rainPositions) {
        float windX = cos(windDirection) * windStrength * deltaTime * 10.0f;
        float windZ = sin(windDirection) * windStrength * deltaTime * 10.0f;
        pos.x += windX;
        pos.y -= rainSpeed * deltaTime * 20.0f;
        pos.z += windZ;
        if (pos.y < 0.0f) {
            pos.y = (rand() % 100) + 50.0f;
            pos.x = (rand() % 200) - 100.0f;
            pos.z = (rand() % 200) - 100.0f;
        }
        rainVertices.push_back(pos);
        rainVertices.push_back(pos + glm::vec3(0.0f, -0.5f, 0.0f));
    }
    glBindBuffer(GL_ARRAY_BUFFER, rainVBO);
    glBufferData(GL_ARRAY_BUFFER, rainVertices.size() * sizeof(glm::vec3), rainVertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void renderRain() {
    if (!rainEnabled || rainVAO == 0) return;
    rainShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(rainShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(rainShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(rainVAO);
    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, nrRaindrops * 2);
    glBindVertexArray(0);
}

void updateEagleAnimation() {
    if (!eagleSetupMode) {
        eagleRotationAngle += eagleFlightSpeed * deltaTime;
        if (eagleRotationAngle >= 360.0f) eagleRotationAngle -= 360.0f;
        float rad = glm::radians(eagleRotationAngle);
        eaglePosition.x = eagleFlightRadius * cos(rad);
        eaglePosition.z = eagleFlightRadius * sin(rad);
    }
}

void renderEagle(gps::Shader shader) {
    shader.useShaderProgram();
    glm::mat4 eagleModel = glm::mat4(1.0f);
    eagleModel = glm::translate(eagleModel, eaglePosition);
    if (!eagleSetupMode) {
        eagleModel = glm::rotate(eagleModel, glm::radians(-eagleRotationAngle - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else {
        eagleModel = glm::rotate(eagleModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    eagleModel = glm::scale(eagleModel, glm::vec3(eagleScaleFactor));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(eagleModel));
    glm::mat3 eagleNormalMatrix = glm::mat3(glm::inverseTranspose(view * eagleModel)); // Notă: pentru basic.frag modificat ar fi mai bine NormalMatrix fără View, dar păstrăm compatibilitatea
    glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(eagleNormalMatrix));
    vultur.Draw(shader);
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    projection = glm::perspective(glm::radians(70.0f), (float)width / (float)height, 0.1f, 1000.0f);
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // CONTROL CEAȚĂ [ ]
    if (key == GLFW_KEY_LEFT_BRACKET && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        fogDensity -= 0.002f;
        if (fogDensity < 0.0f) fogDensity = 0.0f;
        std::cout << "Fog Density: " << fogDensity << std::endl;
    }
    if (key == GLFW_KEY_RIGHT_BRACKET && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        fogDensity += 0.002f;
        std::cout << "Fog Density: " << fogDensity << std::endl;
    }

    // CONTROL VULTUR
    if (eagleSetupMode) {
        float moveSpeed = 2.0f;
        float scaleSpeed = 0.1f;
        if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) eaglePosition.z -= moveSpeed;
        if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) eaglePosition.z += moveSpeed;
        if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) eaglePosition.x -= moveSpeed;
        if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) eaglePosition.x += moveSpeed;
        if ((key == GLFW_KEY_T || key == GLFW_KEY_PAGE_UP) && (action == GLFW_PRESS || action == GLFW_REPEAT)) eaglePosition.y += moveSpeed;
        if ((key == GLFW_KEY_G || key == GLFW_KEY_PAGE_DOWN) && (action == GLFW_PRESS || action == GLFW_REPEAT)) eaglePosition.y -= moveSpeed;
        if (key == GLFW_KEY_INSERT && (action == GLFW_PRESS || action == GLFW_REPEAT)) eagleScaleFactor += scaleSpeed;
        if (key == GLFW_KEY_DELETE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
            eagleScaleFactor -= scaleSpeed; if (eagleScaleFactor < 0.01f) eagleScaleFactor = 0.01f;
        }
        if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
            eagleFlightRadius = sqrt(pow(eaglePosition.x, 2) + pow(eaglePosition.z, 2));
            eagleRotationAngle = glm::degrees(atan2(eaglePosition.z, eaglePosition.x));
            eagleSetupMode = false;
        }
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); glLineWidth(1.5f); }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) { glPolygonMode(GL_FRONT_AND_BACK, GL_SMOOTH); }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS) { glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); glPointSize(4.0f); }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS) { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

    // POINT LIGHT TOGGLE (L)
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        static int lightMode = 0;
        lightMode = (lightMode + 1) % 3;
        if (lightMode == 0) pointLightColor = glm::vec3(0.8f, 0.8f, 0.9f);
        else if (lightMode == 1) pointLightColor = glm::vec3(4.0f, 4.0f, 4.5f);
        else pointLightColor = glm::vec3(0.0f, 0.0f, 0.0f);
        std::cout << "Point Light switched. Mode: " << lightMode << std::endl;
        // NOTA: Trimitem uniforma in renderScene, nu aici, ca sa fim siguri.
    }

    // SECOND LIGHT ROTATION (N)
    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        secondLightRotating = !secondLightRotating;
        secondLightRotationSpeed = secondLightRotating ? 45.0f : 0.0f;
        std::cout << "Rotation Second Light: " << (secondLightRotating ? "ON" : "OFF") << std::endl;
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        presentationMode = !presentationMode;
        if (presentationMode) {
            if (!savedOriginalPosition) {
                originalCameraPosition = myCamera.getPosition();
                originalCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
                originalCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
                savedOriginalPosition = true;
            }
            presentationAngle = 0.0f;
        }
        else {
            myCamera = gps::Camera(originalCameraPosition, originalCameraFront, originalCameraUp);
            firstMouse = true;
            view = myCamera.getViewMatrix();
        }
    }

    if (key == GLFW_KEY_O && action == GLFW_PRESS) objectAnimation = !objectAnimation;
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        fogEnabled = !fogEnabled;
        if (fogEnabled) glClearColor(fogColor.r, fogColor.g, fogColor.b, 1.0f);
        else glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    }
    if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
        rainEnabled = !rainEnabled;
        if (rainEnabled && rainVAO == 0) initRain();
    }
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) pressedKeys[key] = true;
        else if (action == GLFW_RELEASE) pressedKeys[key] = false;
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (presentationMode) { if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; } return; }
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; return; }
    float xoffset = static_cast<float>(xpos - lastX);
    float yoffset = static_cast<float>(lastY - ypos);
    lastX = xpos; lastY = ypos;
    xoffset *= mouseSensitivity; yoffset *= mouseSensitivity;
    myCamera.rotate(yoffset, xoffset);
}

void updateSecondLightDirection() {
    if (secondLightRotationSpeed != 0.0f) {
        secondLightAngle += secondLightRotationSpeed * deltaTime;
        if (secondLightAngle >= 360.0f) secondLightAngle -= 360.0f;
        float radAngle = glm::radians(secondLightAngle);
        secondLightDir = glm::vec3(cos(radAngle), 0.5f, sin(radAngle));
        secondLightDir = glm::normalize(secondLightDir);
    }
}

void updatePresentationMode() {
    if (presentationMode) {
        presentationAngle += presentationSpeed * deltaTime;
        if (presentationAngle >= 360.0f) presentationAngle -= 360.0f;
        float radAngle = glm::radians(presentationAngle);
        float cameraX = sin(radAngle) * presentationRadius;
        float cameraZ = cos(radAngle) * presentationRadius;
        glm::vec3 cameraPosition(cameraX, presentationHeight, cameraZ);
        glm::vec3 cameraTarget(0.0f, presentationHeight * 0.3f, 0.0f);
        glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
        view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
    }
    else {
        view = myCamera.getViewMatrix();
    }
}

void updateObjectAnimation() {
    if (objectAnimation) {
        objectRotationAngle += objectRotationSpeed * deltaTime;
        if (objectRotationAngle >= 360.0f) objectRotationAngle -= 360.0f;
        objectPosition.x = sin(glfwGetTime() * 1.5f) * 5.0f;
        objectPosition.y = abs(sin(glfwGetTime() * 2.0f)) * 1.0f;
        model = glm::translate(glm::mat4(1.0f), objectPosition);
        model = glm::rotate(model, glm::radians(objectRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else {
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    }
}

void processMovement() {
    if (presentationMode) return;
    bool moved = false;
    if (pressedKeys[GLFW_KEY_W]) { myCamera.move(gps::MOVE_FORWARD, cameraSpeed); moved = true; }
    if (pressedKeys[GLFW_KEY_S]) { myCamera.move(gps::MOVE_BACKWARD, cameraSpeed); moved = true; }
    if (pressedKeys[GLFW_KEY_A]) { myCamera.move(gps::MOVE_LEFT, cameraSpeed); moved = true; }
    if (pressedKeys[GLFW_KEY_D]) { myCamera.move(gps::MOVE_RIGHT, cameraSpeed); moved = true; }
    if (pressedKeys[GLFW_KEY_Q]) { angle -= 2.0f; moved = true; }
    if (pressedKeys[GLFW_KEY_E]) { angle += 2.0f; moved = true; }
    if (pressedKeys[GLFW_KEY_SPACE]) { myCamera.move(gps::MOVE_UP, cameraSpeed); moved = true; }
    if (pressedKeys[GLFW_KEY_LEFT_CONTROL]) { myCamera.move(gps::MOVE_DOWN, cameraSpeed); moved = true; }
}

void initOpenGLWindow() {
    myWindow.Create(1920, 1080, "3D Scene - Fixed Lights & Fog");
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
    glClearColor(fogColor.r, fogColor.g, fogColor.b, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void initModels() {
    teapot.LoadModel("models/Untitled.obj");
    vultur.LoadModel("models/vultur/Untitled.obj");
}

void initShaders() {
    myBasicShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
}

void initUniforms() {
    myBasicShader.useShaderProgram();

    model = glm::mat4(1.0f);
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    normalMatrix = glm::mat3(glm::inverseTranspose(model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    projection = glm::perspective(glm::radians(70.0f), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 1000.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // LIGHTS
    lightDir = glm::vec3(0.5f, 1.0f, 0.5f);
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    lightColor = glm::vec3(1.0f, 1.0f, 0.9f);
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    // POINT LIGHT
    constantLoc = glGetUniformLocation(myBasicShader.shaderProgram, "constant");
    linearLoc = glGetUniformLocation(myBasicShader.shaderProgram, "linear");
    quadraticLoc = glGetUniformLocation(myBasicShader.shaderProgram, "quadratic");
    glUniform1f(constantLoc, constant);
    glUniform1f(linearLoc, linear);
    glUniform1f(quadraticLoc, quadratic);

    pointLightPosition = glm::vec3(-13.36f, 11.18f, -7.78f);
    pointLightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLightPos");
    // TRIMITEM COORDONATELE WORLD SPACE, NU VIEW SPACE
    glUniform3fv(pointLightPosLoc, 1, glm::value_ptr(pointLightPosition));

    pointLightColor = glm::vec3(4.0f, 4.0f, 4.5f);
    pointLightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLightColor");
    glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor));

    // SECOND LIGHT
    secondLightDir = glm::vec3(0.0f, 0.5f, -1.0f);
    secondLightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "secondLightDir");
    glUniform3fv(secondLightDirLoc, 1, glm::value_ptr(secondLightDir));

    secondLightColor = glm::vec3(0.3f, 0.3f, 0.8f);
    secondLightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "secondLightColor");
    glUniform3fv(secondLightColorLoc, 1, glm::value_ptr(secondLightColor));

    // FOG
    fogEnabledLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogEnabled");
    fogDensityLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
    fogColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogColor");
    cameraPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "cameraPos");
}

void renderScene() {
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myBasicShader.useShaderProgram();

    // UPDATE UNIFORMS (CRITIC PENTRU CA TASTELE SA MEARGA)
    glUniform1i(fogEnabledLoc, fogEnabled ? 1 : 0);
    glUniform1f(fogDensityLoc, fogDensity);
    glUniform3fv(fogColorLoc, 1, glm::value_ptr(fogColor));

    // Trimitem Camera Pos pt Ceata si Specular
    glUniform3fv(cameraPosLoc, 1, glm::value_ptr(myCamera.getPosition()));

    // Trimitem Matrici
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    normalMatrix = glm::mat3(glm::inverseTranspose(model)); // Normal matrix in World Space
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // UPDATE LIGHTS (Tastele L si N)
    glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor));
    glUniform3fv(secondLightDirLoc, 1, glm::value_ptr(secondLightDir));

    teapot.Draw(myBasicShader);

    renderEagle(myBasicShader);

    if (rainEnabled) {
        glDisable(GL_CULL_FACE);
        renderRain();
        glEnable(GL_CULL_FACE);
    }
}

void cleanup() {
    if (rainVAO != 0) glDeleteVertexArrays(1, &rainVAO);
    if (rainVBO != 0) glDeleteBuffers(1, &rainVBO);
    myWindow.Delete();
    std::cout << "Application closed!" << std::endl;
}

int main(int argc, const char* argv[]) {
    srand(static_cast<unsigned int>(time(nullptr)));
    try { initOpenGLWindow(); }
    catch (const std::exception& e) { std::cerr << e.what() << std::endl; return 1; }
    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    setWindowCallbacks();

    lastFrame = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        updateSecondLightDirection();
        updatePresentationMode();
        updateObjectAnimation();
        updateEagleAnimation();
        updateRain();
        processMovement();

        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());
    }
    cleanup();
    return 0;
}