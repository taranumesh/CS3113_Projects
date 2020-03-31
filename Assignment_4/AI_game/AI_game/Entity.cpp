#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 0;
    
    modelMatrix = glm::mat4(1.0f);
}

glm::vec3 Entity::playerInSight(Entity *player, Entity *objects, int objectCount) {
    if (fabs(player->position.x - position.x) < width/8) {
        for (int i = 0; i < objectCount; i++) {
            if (fabs(objects[i].position.x - position.x) < width) {
                float distance_between = fabs(glm::distance(player->position, position));
                float total_distance = fabs(glm::distance(player->position, objects[i].position)) + fabs(glm::distance(position, objects[i].position));
                if (fabs(distance_between - total_distance) < 0.02) {
                    goto next;
                }
            }
        }
        return glm::normalize(glm::vec3(0, player->position.y - position.y, 0));
        
    }
            next:
    if (fabs(player->position.y - position.y) < height/8) {
        for (int i = 0; i < objectCount; i++) {
            if (fabs(objects[i].position.y - position.y) < height) {
                float distance_between = fabs(glm::distance(player->position, position));
                float total_distance = fabs(glm::distance(player->position, objects[i].position)) + fabs(glm::distance(position, objects[i].position));
                if (fabs(distance_between - total_distance) < 0.02) {
                    goto end;
                }
            }
        }
        return glm::normalize(glm::vec3(player->position.x - position.x, 0, 0));
    }
end:
    return glm::vec3(0);
}

void Entity::shoot(glm::vec3 shot_direction) {
    if (!bullet->isActive) {
        bullet->position = position;
        bullet->movement = shot_direction;
        bullet->bulletStart = position;
        bullet->isActive = true;
    }
}

void Entity::AIRunner(Entity *player, glm::vec3 shot_direction) {
    switch (aiState) {
        case IDLE:
            aiState = MOVING;
            movement.x = -1.0;
            break;
        case MOVING:
//            if (playerVisible(*player)) {
//                aiState = ATTACKING;
//            } else
            if (collidedLeft) {
                movement.x = 1.0;
            } else if (collidedRight) {
                movement.x = -1.0;
            }
            if (shot_direction != glm::vec3(0)) {
                aiState = ATTACKING;
            }
            break;
        case ATTACKING:
            if (collidedLeft) {
                movement.x = 1.0;
            } else if (collidedRight) {
                movement.x = -1.0;
            }
            if (shot_direction == glm::vec3(0)) {
                aiState = MOVING;
            } else {
                shoot(shot_direction);
            }
            break;
    }
}

void Entity::AICircler(Entity *player, glm::vec3 shot_direction) {
    switch (aiState) {
        case IDLE:
            aiState = MOVING;
            movement.x = -1.0;
            break;
        case MOVING:
            //            if (playerVisible(*player)) {
            //                aiState = ATTACKING;
            //            } else
            if (collidedLeft) {
                movement.y = -1.0;
                movement.x = 0.0;
            } else if (collidedBottom) {
                movement.y = 0.0;
                movement.x = 1.0;
            } else if (collidedRight) {
                movement.y = 1.0;
                movement.x = 0.0;
            } else if (collidedTop) {
                movement.y = 0.0;
                movement.x = -1.0;
            }
            if (shot_direction != glm::vec3(0)) {
                aiState = ATTACKING;
            }
            break;
        case ATTACKING:
            if (collidedLeft) {
                movement.y = -1.0;
                movement.x = 0.0;
            } else if (collidedBottom) {
                movement.y = 0.0;
                movement.x = 1.0;
            } else if (collidedRight) {
                movement.y = 1.0;
                movement.x = 0.0;
            } else if (collidedTop) {
                movement.y = 0.0;
                movement.x = -1.0;
            }
            if (shot_direction == glm::vec3(0)) {
                aiState = MOVING;
            } else {
                shoot(shot_direction);
            }
            break;
    }
}

void Entity::AIStationary(Entity *player, glm::vec3 shot_direction) {
    switch (aiState) {
        case IDLE:
            if (shot_direction != glm::vec3(0)) {
                aiState = ATTACKING;
            }
            break;
        case MOVING:
            break;
        case ATTACKING:
            if (shot_direction == glm::vec3(0)) {
                aiState = MOVING;
            } else {
                shoot(shot_direction);
            }
            break;
    }
}

void Entity::AI(Entity *player, glm::vec3 shot_direction) {
    switch(aiType) {
        case RUNNER:
            AIRunner(player, shot_direction);
            break;
        case CIRCLER:
            AICircler(player, shot_direction);
            break;
        case STATIONARY:
            AIStationary(player, shot_direction);
            break;
    }
}



void Entity::Update(float deltaTime, Entity *player, Entity *platforms, int platformCount)
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
                    animIndex = 0;
                }
            }
        } else {
            animIndex = 0;
        }
    }
    
    velocity.x = movement.x * speed;
    velocity.y = movement.y * speed;
    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(platforms, platformCount);// Fix if needed
    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(platforms, platformCount);// Fix if needed
    CheckCollisionGameEdge();
    
    if (entityType == ENEMY) {
        AI(player, playerInSight(player, platforms, platformCount));
    }
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

bool Entity::CheckCollision(Entity *other) {
    if (isActive == false || other->isActive == false) return false;
    float x_dist = fabs(position.x - other->position.x) - ((width + other->width)/2.0);
    float y_dist = fabs(position.y - other->position.y) - ((height + other->height)/2.0);
    if (x_dist < 0 && y_dist < 0) return true;
    return false;
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                movement.y = 0;
                collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                movement.y = 0;
                collidedBottom = true;
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                movement.x = 0;
                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                movement.x = 0;
                collidedLeft = true;
            }
        }
    }
}

void Entity::CheckCollisionGameEdge(void) {
    if (position.x + (width/2) > 5.0) {
        position.x = 5 - (width/2);
        velocity.x = 0;
        movement.x = 0;
        collidedRight = true;
    } else if (position.x - (width/2) < -5) {
        position.x = -5 + (width/2);
        velocity.x = 0;
        movement.x = 0;
        collidedLeft = true;
    } else if (position.y + (height/2) > 3.5) {
        position.y = 3.5 - (height/2);
        velocity.y = 0;
        movement.y = 0;
        collidedTop = true;
    } else if (position.y - (height/2) < -3.5) {
        position.y = -3.5 + (height/2);
        velocity.y = 0;
        movement.y = 0;
        collidedBottom = true;
    }
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    if (isActive == false) return;
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

