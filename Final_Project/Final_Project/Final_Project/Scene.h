#ifndef Scene_h
#define Scene_h

#pragma once
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
#include "Util.h"
#include "Player.h"
#include "Enemy.h"
#include "Plant.h"
#include "Map.h"

struct GameState {
    Map *map;
    Player *player;
    Enemy *enemies;
    Plant *plants;
    
    int nextScene;
    
    int seeds_left;
    int harvested;
    int harvest_goal;
    
    bool lose;
    bool win;
};
class Scene {
public:
    GameState state;
    virtual void Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(ShaderProgram *program) = 0;
    virtual void WaterPlant(void) = 0;
    virtual void HarvestPlant(void) = 0;
    virtual void PlacePlant(void) = 0;
};

#endif
