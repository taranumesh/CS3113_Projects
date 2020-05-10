//
//  MainMenu.cpp
//  Final_Project
//
//  Created by Tara Umesh on 5/10/20.
//  Copyright © 2020 Tara Umesh. All rights reserved.
//

#include "MainMenu.h"

#define ENEMY_COUNT 1
#define SEED_COUNT 2
#define LEVEL1_WIDTH 11
#define LEVEL1_HEIGHT 8
unsigned int mainMenu_data[] =
{
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,
    3, 1, 1, 1, 1, 0, 1, 1, 1, 1, 3,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

void MainMenu::Initialize() {
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tile_set.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, mainMenu_data, mapTextureID, 1.0f, 2, 2);
    
    // Initialize Game Objects
    state.player = new Player();
    state.player->position = glm::vec3(5.0, -4.0, 0.0);
    state.enemies = new Enemy[ENEMY_COUNT];
    state.plants = new Plant[SEED_COUNT];
    state.plants[0].position = glm::vec3(5.0, -5.0, 0.0);
    state.plants[0].isActive = true;
    state.plants[0].animIndex = 3;
    state.win = false;
    state.lose = false;
    state.harvested = 0;
    state.seeds_left = SEED_COUNT;
    state.harvest_goal = SEED_COUNT;
}

void MainMenu::Update(float deltaTime) {
    state.player->modelMatrix = glm::translate(glm::mat4(1.0), state.player->position);
    state.plants->modelMatrix = glm::translate(glm::mat4(1.0), state.plants->position);
}

void MainMenu::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    for (int i=0; i<SEED_COUNT; i++) {
        state.plants[i].Render(program);
    }
    // Game info banner
    std::string title = "Sunflower Farm";
    std::string press_enter = "Press enter to start";
    GLuint font_texture = Util::LoadTexture("font1.png");
    Util::DrawText(program, font_texture, title, 1.25, -0.625, glm::vec3(0.875, -1.5, 0.0));
    Util::DrawText(program, font_texture, press_enter, 0.75, -0.35, glm::vec3(1.25, -3, 0.0));
}

void MainMenu::PlacePlant() {

}

void MainMenu::WaterPlant() {

}

void MainMenu::HarvestPlant() {
    
}

