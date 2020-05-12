#include "Player.h"
#include "Util.h"

Player::Player()
{
    position = glm::vec3(8, -3, 0);
    movement = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 1.0f;
    
    modelMatrix = glm::mat4(1.0f);

    textureID = Util::LoadTexture("farmer_player.png");
    
    height = 0.8f;
    width = 0.65f;
    
    animRight = new int[3] {9, 10, 11};
    animLeft = new int[3] {6, 7, 8};
    animUp = new int[3] {3, 4, 5};
    animDown = new int[3] {0, 1, 2};
    
    animIndices = animRight;
    animFrames = 3;
    animIndex = 0;
    animTime = 0;
    animCols = 3;
    animRows = 4;
    facing = RIGHT;
}

void Player::Update(float deltaTime, Entity *objects, int objectCount, Map *map)
{
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    if (isActive == false) return;
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;
            
            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 1;
                }
            }
        } else {
            animIndex = 0;
        }
    }
    
    velocity = movement * speed;
    
    position.y += velocity.y * deltaTime;
    CheckCollisionsY(map);
    position.x += velocity.x * deltaTime;
    CheckCollisionsX(map);

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

