#include "Level3.h"

#define ENEMY_COUNT 4
#define LEVEL3_WIDTH 24
#define LEVEL3_HEIGHT 8

unsigned int level3_data[] =
{
    5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 5, 0, 0, 1, 0, 5,
    5, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 5,
    5, 0, 0, 1, 0, 5, 5, 5, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 5,
    5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 5, 0, 0, 5, 5, 0, 0, 0, 0, 5,
    5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5,
    5, 0, 2, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 5, 3, 3, 5,
    5, 3, 3, 3, 0, 0, 5, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 5, 0, 0, 0, 3, 5,
    5, 3, 3, 3, 0, 0, 5, 0, 0, 3, 3, 3, 3, 3, 0, 0, 0, 3, 3, 3, 3, 3, 3, 5
};

void Level3::Initialize() {
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tiles.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 3, 2);
    
    // Initialize Game Objects
    state.lose = false;
    state.win = false;
    state.score = 0;
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(3, -3, 0);
    state.player->movement = glm::vec3(0);
    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("player.png");
    
    state.player->height = 1.0f;
    state.player->width = 0.5f;
    
    state.player->animRight = new int[2] {0, 2};
    state.player->animLeft = new int[2] {1, 3};
    
    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 2;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 2;
    state.player->animRows = 2;
    
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    
    state.player->jumpPower = 6.0f;
    
    state.enemies = new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("cow.png");
    
    for (int i=0; i<ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        
        state.enemies[i].speed = 1;
        state.enemies[i].acceleration = glm::vec3(0, -9.81f, 0);
        
        state.enemies[i].height = 0.6;
        
        state.enemies[i].aiType = WALKER;
        state.enemies[i].aiState = WALKING;
        
        state.enemies[i].animRight = new int[2] {0, 2};
        state.enemies[i].animLeft = new int[2] {1, 3};
        state.enemies[i].animIndices = state.enemies[0].animRight;
        state.enemies[i].animFrames = 2;
        state.enemies[i].animIndex = 0;
        state.enemies[i].animCols = 2;
        state.enemies[i].animRows = 2;
        state.enemies[i].movement = glm::vec3(-1, 0, 0);
    }
    state.enemies[0].position = glm::vec3(6, -1, 0);
    state.enemies[1].position = glm::vec3(12, -6, 0);
    state.enemies[2].position = glm::vec3(13, -6, 0);
    state.enemies[3].position = glm::vec3(21, -6, 0);
}
void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, ENEMY_COUNT, state.map);
    if (state.player->position.x >= 22) {
        state.win = true;
        state.nextScene = 0;
    }
    state.score = 0;
    for (int i=0; i<ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, ENEMY_COUNT, state.map);
        if (!state.enemies[i].isActive) {
            state.score+= 50;
        }
    }
    if (state.player->playerHit || state.player->position.y < -10) {
        state.lives -= 1;
        ResetLevel();
        state.player->playerHit = false;
    }
}
void Level3::Render(ShaderProgram *program) {
    state.map->Render(program);
    if (state.lives == 0) {
        state.lose = true;
    } else {
        state.player->Render(program);
        for (int i=0; i<ENEMY_COUNT; i++) {
            state.enemies[i].Render(program);
        }
    }
}

void Level3::ResetLevel() {
    state.score = 0;
    // Reset Player
    state.player->position = glm::vec3(3, -3, 0);
    state.player->velocity = glm::vec3(0, 0, 0);
    state.player->movement = glm::vec3(0);
    
    state.player->animIndices = state.player->animRight;
    state.player->animIndex = 0;
    
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    
    // Reset Enemies
    for (int i=0; i<ENEMY_COUNT; i++) {
        state.enemies[i].acceleration = glm::vec3(0, -9.81f, 0);
        
        state.enemies[i].aiType = WALKER;
        state.enemies[i].aiState = WALKING;
        
        state.enemies[i].animIndices = state.enemies[i].animRight;
        state.enemies[i].animIndex = 0;
        state.enemies[i].movement = glm::vec3(-1, 0, 0);
        state.enemies[i].isActive = true;
    }
    state.enemies[0].position = glm::vec3(6, -1, 0);
    state.enemies[1].position = glm::vec3(12, -6, 0);
    state.enemies[2].position = glm::vec3(13, -6, 0);
    state.enemies[3].position = glm::vec3(21, -6, 0);
}
