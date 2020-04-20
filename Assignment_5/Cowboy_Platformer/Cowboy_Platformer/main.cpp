#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Map.h"
#include "Util.h"
#include "Scene.h"
#include "GameMenu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene *currentScene;
Scene *sceneList[4];

GLuint fontTextureID;

int previousLevelsScore = 0;

Mix_Music *music;
Mix_Chunk *win;

void SwitchToScene(Scene *scene, int lives) {
    if (scene == sceneList[0]) {
        previousLevelsScore = 0;
    } else {
        previousLevelsScore += currentScene->state.score;
    }
    currentScene = scene;
    currentScene->Initialize();
    currentScene->state.lives = lives;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Initialize the levels and start at the first one
    sceneList[0] = new GameMenu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0], 3);
    
    fontTextureID = Util::LoadTexture("font1.png");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("yeehaw_music.wav");
    Mix_PlayMusic(music, -1);
    win = Mix_LoadWAV("yeehaw.wav");
}

void ProcessInput() {
    
    currentScene->state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                        
                    case SDLK_SPACE:
                        // Some sort of action
                        if (currentScene != sceneList[0]) {
                            if (currentScene->state.player->collidedBottom) currentScene->state.player->jump = true;
                        } else {
                            SwitchToScene(sceneList[1], 3);
                        }
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_LEFT]) {
        if (currentScene != sceneList[0]) {
            currentScene->state.player->movement.x = -1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animLeft;
        }
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        if (currentScene != sceneList[0]) {
            currentScene->state.player->movement.x = 1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animRight;
        }
    }
    
    
    if (glm::length(currentScene->state.player->movement) > 1.0f) {
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
    }
    
}

#define FIXED_TIMESTEP 0.0166666f
void Update() {
    currentScene->Update(FIXED_TIMESTEP);
    
    viewMatrix = glm::mat4(1.0f);
    if ((currentScene->state.player->position.x > 5) && (currentScene->state.player->position.x < 18)) {
        viewMatrix = glm::translate(viewMatrix,
                                    glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    } else if (currentScene->state.player->position.x <= 5) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    } else if (currentScene->state.player->position.x >= 18) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-18, 3.75, 0));
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetViewMatrix(viewMatrix);
    
    currentScene->Render(&program);
    
    if (currentScene != sceneList[0]) {
        glm::vec3 header_position;
        if ((currentScene->state.player->position.x > 5) && (currentScene->state.player->position.x < 18)) {
            header_position = glm::vec3(currentScene->state.player->position.x-4, -0.5, 0);
        } else if (currentScene->state.player->position.x <= 5) {
            header_position = glm::vec3(1.0, -0.5, 0);
        } else if (currentScene->state.player->position.x >= 18) {
            header_position = glm::vec3(14, -0.5, 0);
        }
        // Game Header
        std::string score = "Score:" + std::to_string((previousLevelsScore + currentScene->state.score));
        std::string lives = " Lives:" + std::to_string(currentScene->state.lives);
        Util::DrawText(&program, fontTextureID, score, 1.0f, -0.5f, header_position);
        header_position.x += 4.5;
        Util::DrawText(&program, fontTextureID, lives, 1.0f, -0.5f, header_position);
        
        // Check Lose/Win
        if (currentScene->state.lose) {
            header_position.y -= 2.5;
            header_position.x -= 2.0;
            Util::DrawText(&program, fontTextureID, "YOU LOSE", 1.0f, -0.5f, header_position);
        } else if (currentScene->state.win){
            header_position.y -= 2.5;
            header_position.x -= 2.0;
            Util::DrawText(&program, fontTextureID, "YOU WIN!", 1.0f, -0.5f, header_position);
            Mix_PlayChannel(-1, win, 0);
        }
    }
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        if (currentScene->state.nextScene >= 0) SwitchToScene(sceneList[currentScene->state.nextScene], currentScene->state.lives);
        Update();
        Render();
        if (currentScene->state.lose || currentScene->state.win) {
            SDL_Delay(2000);
            SwitchToScene(sceneList[0], 3);
        }
    }
    
    Shutdown();
    return 0;
}
