#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
glm::mat4 modelMatrices_c[6];
glm::mat4 modelMatrices_f[5];

float cloud_x_reset = -7.0f;

float clouds_x[6] = {-6.0f, -3.3f, 0.0f, 1.7f, 4.5f, 6.0f};
float clouds_y[6] = {3.0f, 1.5f, 2.0f, 2.7f, 2.2f, 2.5};

float flowers_x[5] = {-4.0f, -2.0f, 0.0f, 2.0f, 4.0f};
float flowers_y[5] = {-1.0, -2.0f, -1.0f, -2.0f, -1.0f};
float flowers_rot[5] = {0.0f, 60.0f, 300.0f, 120.0f, 250.0f};
float flowers_scale[5] = {1.0, 1.4, 1.7, 1.2, 1.9};
float flowers_scale_mul[5] = {1.01, 0.99, 0.99, 1.01, 1.01};

GLuint playerTextureID_cloud;
GLuint playerTextureID_flower;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Texture", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
//    program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.3f, 0.65f, 0.85f, 1.0f);
    
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    playerTextureID_cloud = LoadTexture("cloud.png");
    playerTextureID_flower = LoadTexture("flower.png");
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

void updateClouds(float deltaTime) {
    for (size_t i=0; i < 6; ++i) {
        if (clouds_x[i] > 7.0f) clouds_x[i] = cloud_x_reset;
        else clouds_x[i] += 0.5*deltaTime;
        modelMatrix = glm::mat4(1.0);
        modelMatrices_c[i] = glm::translate(modelMatrix, glm::vec3(clouds_x[i], clouds_y[i], 0.0f));
        modelMatrices_c[i] = glm::scale(modelMatrices_c[i], glm::vec3(2.0f, 2.0f, 0.0));
    }
}

void updateFlowers(float deltaTime) {
    float scaler;
    for (size_t i=0; i < 5; ++i) {
        if (flowers_rot[i] > 360) flowers_rot[i] -= 360;
        flowers_rot[i] += deltaTime*90;
        if (flowers_scale[i] > 2.0f) flowers_scale_mul[i] = 0.99;
        else if (flowers_scale[i] < 1.0f) flowers_scale_mul[i] = 1.01;
        scaler = flowers_scale_mul[i] * flowers_scale[i];
        flowers_scale[i] = scaler;
        modelMatrix = glm::mat4(1.0);
        modelMatrices_f[i] = glm::translate(modelMatrix, glm::vec3(flowers_x[i], flowers_y[i], 0.0f));
        modelMatrices_f[i] = glm::scale(modelMatrices_f[i], glm::vec3(scaler, scaler, 0.0));
        modelMatrices_f[i] = glm::rotate(modelMatrices_f[i], glm::radians(flowers_rot[i]),glm::vec3(0.0f, 0.0f, 1.0f));
    }
}

float lastTicks = 0.0f;
void Update() {
    float ticks = (float)SDL_GetTicks()/1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    updateClouds(deltaTime);
    updateFlowers(deltaTime);
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    for (size_t i=0; i < 6; ++i) {
        program.SetModelMatrix(modelMatrices_c[i]);
        float vertices[]  = { -0.75, -0.5, 0.75, -0.5, 0.75, 0.5, -0.75, -0.5, 0.75, 0.5, -0.75, 0.5 };
        float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glBindTexture(GL_TEXTURE_2D, playerTextureID_cloud);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    for (size_t i=0; i < 5; ++i) {
        program.SetModelMatrix(modelMatrices_f[i]);
        float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
        float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glBindTexture(GL_TEXTURE_2D, playerTextureID_flower);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
