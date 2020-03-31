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
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

#define WALL_COUNT 24
#define ENEMY_COUNT 3

struct GameState {
    Entity *player;
    Entity *walls;
    Entity *enemies;
    Entity *bullets;
    bool win;
    bool died;
};

GLuint fontTextureID;

glm::vec3 wall_layout[WALL_COUNT] = {glm::vec3(-0.5,-3,0), glm::vec3(1.5,-3,0), glm::vec3(-3.5,-2,0), glm::vec3(-1.5,-2,0), glm::vec3(-0.5,-2,0), glm::vec3(1.5,-2,0), glm::vec3(3.5,-2,0), glm::vec3(3.5,-1,0), glm::vec3(-3.5,0,0), glm::vec3(-2.5,0,0), glm::vec3(-1.5,0,0), glm::vec3(0.5,0,0), glm::vec3(1.5,0,0), glm::vec3(3.5,0,0), glm::vec3(-2.5,1,0), glm::vec3(-1.5,1,0), glm::vec3(3.5,1,0), glm::vec3(-3.5,2,0), glm::vec3(-2.5,2,0), glm::vec3(-1.5,2,0), glm::vec3(-0.5,2,0), glm::vec3(1.5,2,0), glm::vec3(2.5,2,0), glm::vec3(3.5,2,0)};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

glm::vec3 playerDestination;

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
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 560, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 800, 560);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.5f, 3.5f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(3.5, -3, 0);
    state.player->movement = glm::vec3(0);
    state.player->speed = 2.0f;
    state.player->textureID = LoadTexture("player.png");
    
    state.player->height = 1.0f;
    state.player->width = 1.0f;
    
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};
    
    state.player->animIndices = NULL;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    playerDestination = state.player->position;
    
    
    // Initialize Walls
    state.walls = new Entity[WALL_COUNT];
    GLuint platformTextureID = LoadTexture("wall.png");
    for (int i = 0; i < WALL_COUNT; i++) {
        state.walls[i].textureID = platformTextureID;
        state.walls[i].position = wall_layout[i];
        state.walls[i].entityType = PLATFORM;
    }
    
    for (int i = 0; i < WALL_COUNT; i++) {
        state.walls[i].Update(0, NULL, NULL, 0);
    }
    
    // Initialize Enemies
    state.enemies = new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = LoadTexture("enemy.png");
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(4.5, 3, 0);
    state.enemies[0].speed = 3;
    state.enemies[0].aiType = RUNNER;
    state.enemies[0].aiState = IDLE;
    
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].position = glm::vec3(1.5, 1, 0);
    state.enemies[1].speed = 1;
    state.enemies[1].aiType = CIRCLER;
    state.enemies[1].aiState = IDLE;
    
    state.enemies[2].entityType = ENEMY;
    state.enemies[2].textureID = enemyTextureID;
    state.enemies[2].position = glm::vec3(-4.5, -3, 0);
    state.enemies[2].speed = 0.0;
    state.enemies[2].aiType = STATIONARY;
    state.enemies[2].aiState = IDLE;
    
    // Initialize Bullets
    state.bullets = new Entity[ENEMY_COUNT + 1];
    GLuint bulletTexureID = LoadTexture("bullet.png");
    for (int i = 0; i < ENEMY_COUNT + 1; i++) {
        state.bullets[i].isActive = false;
        state.bullets[i].speed = 4.0f;
        state.bullets[i].textureID = bulletTexureID;
        state.bullets[i].entityType = BULLET;
        state.bullets[i].height = 0.2;
        state.bullets[i].width = 0.2;
    }
    state.player->bullet = &state.bullets[ENEMY_COUNT];
    state.enemies[0].bullet = &state.bullets[0];
    state.enemies[1].bullet = &state.bullets[1];
    state.enemies[2].bullet = &state.bullets[2];
    
    // Initialize game state
    state.died = false;
    state.win = false;
    
    // Text
    fontTextureID = LoadTexture("font1.png");
}

void ProcessInput() {
    
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
                        if (state.player->movement == glm::vec3(0)) {
                            state.player->movement.x = -1.0f;
                            playerDestination = state.player->position - glm::vec3(1.0, 0, 0);
                        }
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        if (state.player->movement == glm::vec3(0)) {
                            state.player->movement.x = 1.0f;
                            playerDestination = state.player->position + glm::vec3(1.0, 0, 0);
                        }
                        break;
                    case SDLK_UP:
                        // Move the player up
                        if (state.player->movement == glm::vec3(0)) {
                            state.player->movement.y = 1.0f;
                            playerDestination = state.player->position + glm::vec3(0, 1.0, 0);
                        }
                        break;
                    case SDLK_DOWN:
                        // Move the player down
                        if (state.player->movement == glm::vec3(0)) {
                            state.player->movement.y = -1.0f;
                            playerDestination = state.player->position - glm::vec3(0, 1.0, 0);
                        }
                        break;
                    case SDLK_w:
                        state.player->shoot(glm::vec3(0,1,0));
                        break;
                    case SDLK_s:
                        state.player->shoot(glm::vec3(0,-1,0));
                        break;
                    case SDLK_d:
                        state.player->shoot(glm::vec3(1,0,0));
                        break;
                    case SDLK_a:
                        state.player->shoot(glm::vec3(-1,0,0));
                        break;
                    case SDLK_SPACE:
                        // Some sort of action
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    
    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }
    
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return; }
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->Update(FIXED_TIMESTEP, state.player, state.walls, WALL_COUNT);
        
        for (int i = 0; i < ENEMY_COUNT; i++) {
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.walls, WALL_COUNT);
        }
        
        for (int i = 0; i < ENEMY_COUNT + 1; i++) {
            state.bullets[i].Update(FIXED_TIMESTEP, state.player, state.walls, WALL_COUNT);
        }
        
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    if (glm::distance(state.player->position, playerDestination) < 0.05) {
        state.player->movement = glm::vec3(0);
        state.player->position = playerDestination;
    }
}

void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text,
              float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for(int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
        });
        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        });
    } // end of for loop
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (int i = 0; i < WALL_COUNT; i++) {
        state.walls[i].Render(&program);
    }
    int enemies_dead = 0;
    for (int i =0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Render(&program);
        if (state.enemies[i].CheckCollision(&state.bullets[ENEMY_COUNT])) {
            state.enemies[i].isActive = false;
        }
        if (!state.enemies[i].isActive) enemies_dead++;
    }
    
    state.win = enemies_dead == ENEMY_COUNT;
    
    for (int i =0; i < ENEMY_COUNT+1; i++) {
        state.bullets[i].Render(&program);
        if (state.bullets[i].collidedTop | state.bullets[i].collidedBottom | state.bullets[i].collidedLeft | state.bullets[i].collidedRight) {
            state.bullets[i].isActive = false;
        }
        if (state.bullets[i].CheckCollision(state.player) && (i != ENEMY_COUNT)) {
            state.died = true;
        }
    }
    
    state.player->Render(&program);
    
    if (state.win) {
        DrawText(&program, fontTextureID, "SUCCESS!", 1.0, -0.2, glm::vec3(-2.0,0,0));
    } else if (state.died) {
        DrawText(&program, fontTextureID, "GAME OVER!", 1.0, -0.2, glm::vec3(-2.0,0,0));
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
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
