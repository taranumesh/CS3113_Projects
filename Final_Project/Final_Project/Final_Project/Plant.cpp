#include "Plant.h"
#include "Util.h"

Plant::Plant()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 0;
    water_level = 0.0;
    state = SEED;
    isActive = false;

    modelMatrix = glm::mat4(1.0f);
    
    GLuint plantTextureID = Util::LoadTexture("sunflower.png");
    textureID = plantTextureID;
    animRight = new int[5] {0, 1, 2, 3, 4};
    animIndices = animRight;
    animFrames = 5;
    animIndex = 0;
    animCols = 1;
    animRows = 5;
    isActive = false;
    
    water_level = 1.0;
    
    sound_effect = Mix_LoadWAV("dig.wav");
}

void Plant::Update(float deltaTime, Entity *objects, int objectCount, Map *map)
{
    if (isActive == false) return;
    
    switch (state) {
        case SEED:
            water_level -= 0.075*deltaTime;
            timer += 0.075*deltaTime;
            if (timer > 1.0) {
                state = SEEDLING;
                animIndex = 1;
            }
            if (water_level < 0) {
                state = DEAD;
                isActive = false;
            }
            break;
        case SEEDLING:
            water_level -= 0.075*deltaTime;
            timer += 0.075*deltaTime;
            if (timer > 2.0) {
                state = ADULT;
                animIndex = 2;
            }
            if (water_level < 0) {
                state = DEAD;
                isActive = false;
            }
            break;
        case ADULT:
            water_level -= 0.075*deltaTime;
            timer += 0.075*deltaTime;
            if (timer > 3.0) {
                state = FLOWERING;
                animIndex = 3;
            }
            if (water_level < 0) {
                state = DEAD;
                isActive = false;
            }
            break;
        case FLOWERING:
            water_level -= 0.075*deltaTime;
            timer += 0.075*deltaTime;
            if (timer > 4.0) {
                state = DEAD;
                animIndex = 4;
            }
            if (water_level < 0) {
                state = DEAD;
                isActive = false;
            }
            break;
        case DEAD:
            water_level -= 0.05*deltaTime;
            timer += 0.1*deltaTime;
            if (timer > 5.0) {
                animIndex = 0;
                isActive = false;
            }
            break;
    }
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Plant::PlacePlant(glm::vec3 plant_position) {
    position = plant_position;
    isActive = true;
    state = SEED;
    water_level = 1.0;
    timer = 0;
    Mix_PlayChannel(-1, sound_effect, 0);
}

void Plant::WaterPlant(void) {
    water_level = 1.0;
}

void Plant::HarvestPlant(void) {
    isActive = false;
    state = HARVESTED;
}


void Plant::Render_Water_Level(ShaderProgram *program) {
    if (isActive == false) return;
    if (water_level > 0) {
        glm::vec3 waterPosition = position;
        waterPosition.y += height * 0.75;
        waterPosition.x -= (1 - water_level)/2;
        glm::vec3 waterScale = glm::vec3(1*water_level, 0.3, 1.0);
        glm::mat4 waterModelMatrix = glm::mat4(1.0);
        
        waterModelMatrix = glm::translate(waterModelMatrix, waterPosition);
        waterModelMatrix = glm::scale(waterModelMatrix, waterScale);
        
        program->SetModelMatrix(waterModelMatrix);
        
        float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
        float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->positionAttribute);
        
        glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program->texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program->positionAttribute);
        glDisableVertexAttribArray(program->texCoordAttribute);
    }
}
