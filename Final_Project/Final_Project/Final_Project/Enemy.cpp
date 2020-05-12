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
    
    textureID = Util::LoadTexture("bunny.png");
    animRight = new int[2] {0, 1};
    animUp = new int[2] {2, 3};
    animLeft = new int[2] {4, 5};
    animDown = new int[2] {6, 7};
    animIndices = animRight;
    animFrames = 2;
    animIndex = 0;
    animCols = 4;
    animRows = 2;
    isActive = false;
    
    state = INACTIVE;
    
    sound_effect = Mix_LoadWAV("munch.wav");
}

void Enemy::Update(float deltaTime, Entity *objects, int objectCount, Map *map)
{
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    if (isActive == false) return;
    
    if (animIndices != NULL) {
        animTime += deltaTime;
        
        if (animTime >= 0.25f)
        {
            animTime = 0.0f;
            animIndex++;
            if (animIndex >= animFrames)
            {
                animIndex = 0;
            }
        }
    }
    
    switch (state) {
        case INACTIVE:
            state = WALKING;
            position = startPosition;
            movement = glm::normalize((target->position - position));
            if (movement.x > 0) {
                animIndices = animRight;
            } else {
                animIndices = animLeft;
            }
            break;
        case WALKING:
            velocity =  movement * speed;
            position.y += velocity.y * deltaTime;
            position.x += velocity.x * deltaTime;
             if (glm::distance(target->position, position) < 1.0) {
                state = EATING;
                if (movement.x > 0) {
                    animIndices = animUp;
                } else {
                    animIndices = animDown;
                }
                eating_timer = 1.0;
                Mix_PlayChannel(-1, sound_effect, 0);
            }
        break;
        case EATING:
            eating_timer -= deltaTime*0.25;
            if (eating_timer < 0) {
                target->state = DEAD;
                target->isActive = false;
            }
            break;
        case FLEEING:
            velocity =  movement * speed;
            position.y += velocity.y * deltaTime;
            position.x += velocity.x * deltaTime;
            break;
            
    }
    
    if ((glm::distance(objects->position, position) < 1.5) || (!target->isActive && state != FLEEING)) {
        state = FLEEING;
        movement = glm::normalize((startPosition - position));
        if (movement.x > 0) {
            animIndices = animRight;
        } else {
            animIndices = animLeft;
        }
    }
    if ((position.x > 12) || (position.x < -1.5) || (position.y > 1.5) || (position.y < -9)) {
        isActive = false;
        state = INACTIVE;
    }
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}
