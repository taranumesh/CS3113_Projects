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

enum EntityType { PLAYER, PLATFORM, ENEMY, BULLET };

enum AIType { CIRCLER, RUNNER, STATIONARY };
enum AIState { IDLE, MOVING, ATTACKING };

class Entity {
public:
    EntityType entityType;
    AIType aiType;
    AIState aiState;
    
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 velocity;
    float speed;
    
    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
    int *animRight = NULL;
    int *animLeft = NULL;
    int *animUp = NULL;
    int *animDown = NULL;
    
    int *animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;
    
    bool isActive = true;
    
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    
    float width = 1;
    float height = 1;
    
    Entity *bullet = NULL;
    glm::vec3 bulletStart;
    
    Entity();
    bool CheckCollision(Entity *other);
    void CheckCollisionsY(Entity *objects, int objectCount);
    void CheckCollisionsX(Entity *objects, int objectCount);
    void CheckCollisionGameEdge(void);
    void Update(float deltaTime, Entity *player, Entity *platforms, int platformCount);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    
    void AI(Entity *player, glm::vec3 shot_direction);
    void AIRunner(Entity *player, glm::vec3 shot_direction);
    void AICircler(Entity *player, glm::vec3 shot_direction);
    void AIStationary(Entity *player, glm::vec3 shot_direction);
    void shoot(glm::vec3 shot_direction);
    glm::vec3 playerInSight(Entity *player, Entity *objects, int objectCount);
};
