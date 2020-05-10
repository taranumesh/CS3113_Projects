//
//  Enemy.cpp
//  Final_Project
//
//  Created by Tara Umesh on 5/6/20.
//  Copyright © 2020 Tara Umesh. All rights reserved.
//

#include "Enemy.h"
#include "Util.h"

Enemy::Enemy()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 1;
    
    modelMatrix = glm::mat4(1.0f);
    
    textureID = Util::LoadTexture("farmer_player.png");
    isActive = false;
}

void Enemy::Update(float deltaTime, Entity *objects, int objectCount, Map *map)
{
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    if (isActive == false) return;
    
    //    if (PlayerType == ENEMY) {
    //        AI(player);
    //    }
    
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
    CheckCollisionsY(objects, objectCount);
    position.x += velocity.x * deltaTime;
    CheckCollisionsX(map);
    CheckCollisionsX(objects, objectCount);
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}
