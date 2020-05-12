#include "Level1.h"

#define ENEMY_COUNT 2
#define SEED_COUNT 2
#define LEVEL1_WIDTH 11
#define LEVEL1_HEIGHT 8
unsigned int level1_data[] =
{
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 1, 1, 1, 1, 1, 1, 2, 1, 1, 3,
    3, 1, 1, 0, 1, 1, 1, 2, 1, 1, 3,
    3, 1, 1, 1, 1, 1, 1, 2, 1, 1, 3,
    3, 1, 1, 2, 2, 2, 2, 2, 1, 1, 3,
    3, 1, 1, 2, 1, 0, 1, 1, 1, 1, 3,
    3, 1, 1, 2, 1, 1, 1, 1, 1, 1, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

void Level1::Initialize() {
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tile_set.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 2, 2);
    
    // Initialize Game Objects
    state.player = new Player();
    
    state.plants = new Plant[SEED_COUNT];
    
    state.enemies = new Enemy[ENEMY_COUNT];
    state.enemies[0].startPosition = glm::vec3(0.0, -5.0, 0.0);
    state.enemies[0].target = &state.plants[0];
    state.enemies[1].startPosition = glm::vec3(10.5, -2.0, 0.0);
    state.enemies[1].target = &state.plants[1];
    
    state.win = false;
    state.lose = false;
    
    state.harvested = 0;
    state.seeds_left = SEED_COUNT;
    state.harvest_goal = SEED_COUNT;
    
    state.score = 100;
}

void Level1::Update(float deltaTime) {
    for (int i=0; i<ENEMY_COUNT; i++) {
        int random = std::rand() % 100;
        if (random == 0) {
            if (!state.enemies[i].isActive && state.enemies[i].target->isActive) {
                state.enemies[i].isActive = true;
            }
        }
        state.enemies[i].Update(deltaTime, state.player, 1, state.map);
    }
    
    for (int i=0; i<SEED_COUNT; i++) {
        state.plants[i].Update(deltaTime, state.enemies, ENEMY_COUNT, state.map);
    }
    
    state.player->Update(deltaTime, state.enemies, ENEMY_COUNT, state.map);
    
    if (state.harvested == state.harvest_goal) {
        state.win = true;
    }
    
    int dead = 0;
    for (int i=0; i<SEED_COUNT; i++) {
        Plant* current_plant = &state.plants[i];
        if (current_plant->state == DEAD) {
            dead++;
        }
    }
    
    if ((SEED_COUNT - dead) < state.harvest_goal) {
        state.lose = true;
    }
    
    if (!state.win && !state.lose) {
        if (state.score >= 0) {
            state.score -= deltaTime * 0.75;
        } else {
            state.score = 0.0;
        }
    }
}

void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    GLuint font_texture = Util::LoadTexture("font1.png");
    if (state.win) {
        std::string you_scored = "You scored " + std::to_string((int)state.score) + "!";
        Util::DrawText(program, font_texture, "Grow-tastic!", 1.5, -0.75, glm::vec3(0.9, -2.5, 0.0));
        Util::DrawText(program, font_texture, you_scored, 0.6, -0.3, glm::vec3(3.2, -3.5, 0.0));
        Util::DrawText(program, font_texture, "Press enter to continue.", 0.6, -0.3, glm::vec3(1.65, -4.5, 0.0));
    } else if (state.lose) {
        Util::DrawText(program, font_texture, "Too many plants died", 0.9, -0.45, glm::vec3(0.75, -2.5, 0.0));
        Util::DrawText(program, font_texture, "Press enter to retry.", 0.6, -0.3, glm::vec3(2.3, -3.25, 0.0));
    } else {
        state.player->Render(program);
        for (int i=0; i<SEED_COUNT; i++) {
            state.plants[i].Render(program);
            state.plants[i].Render_Water_Level(program);
        }
        for (int i=0; i<ENEMY_COUNT; i++) {
            state.enemies[i].Render(program);
        }
    }
    // Game info banner
    std::string score = "Score:" + std::to_string((int)state.score);
    std::string seeds_left = "Seeds:" + std::to_string(state.seeds_left);
    std::string harvested = "Harvested:" + std::to_string(state.harvested);
    std::string harvest_goal = "Harvest Left:" + std::to_string(state.harvest_goal - state.harvested);
    
    Util::DrawText(program, font_texture, score, 0.5, -0.3, glm::vec3(0.25, -0.25, 0.0));
    Util::DrawText(program, font_texture, seeds_left, 0.5, -0.3, glm::vec3(2.5, -0.25, 0.0));
    Util::DrawText(program, font_texture, harvested, 0.5, -0.3, glm::vec3(4.5, -0.25, 0.0));
    Util::DrawText(program, font_texture, harvest_goal, 0.5, -0.3, glm::vec3(7.0, -0.25, 0.0));
    
}

void Level1::PlacePlant() {
    if (state.seeds_left > 0) {
        int plant_index = SEED_COUNT - state.seeds_left;
        glm::vec3 plant_position;
        glm::vec3 position_in_front = state.player->position;
        switch (state.player->facing) {
            case RIGHT:
                position_in_front.x += state.player->width;
                if (state.map->IsPlantable(position_in_front, &plant_position)) {
                    state.plants[plant_index].PlacePlant(plant_position);
                    state.seeds_left -= 1;
                }
                break;
            case LEFT:
                position_in_front.x -= state.player->width;
                if (state.map->IsPlantable(position_in_front, &plant_position)) {
                    state.plants[plant_index].PlacePlant(plant_position);
                    state.seeds_left -= 1;
                }
                break;
            case UP:
                position_in_front.y += state.player->height;
                if (state.map->IsPlantable(position_in_front, &plant_position)) {
                    state.plants[plant_index].PlacePlant(plant_position);
                    state.seeds_left -= 1;
                }
                break;
            case DOWN:
                position_in_front.y -= state.player->height;
                if (state.map->IsPlantable(position_in_front, &plant_position)) {
                    state.plants[plant_index].PlacePlant(plant_position);
                    state.seeds_left -= 1;
                }
                break;
        }
    }
}

void Level1::WaterPlant() {
    float dist;
    for (int i=0; i<SEED_COUNT; i++) {
        if (!state.plants[i].isActive) continue;
        dist = glm::distance(state.plants[i].position, state.player->position);
        if (dist < 1) {
            state.plants[i].WaterPlant();
            return;
        }
    }
}

void Level1::HarvestPlant() {
    float dist;
    for (int i=0; i<SEED_COUNT; i++) {
        dist = glm::distance(state.plants[i].position, state.player->position);
        if ((dist < 1 ) && (state.plants[i].state == FLOWERING)) {
            state.plants[i].HarvestPlant();
            state.harvested += 1;
            return;
        }
    }
}

void Level1::NextScene() {
    state.nextScene = 2;
}
