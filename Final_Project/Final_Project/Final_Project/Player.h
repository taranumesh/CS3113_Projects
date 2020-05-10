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
#include "Map.h"
#include "Entity.h"

enum direction {
    RIGHT,
    LEFT,
    UP,
    DOWN
};

class Player: public Entity {
public:
    enum direction facing;
    Player();
    void Update(float deltaTime, Entity *objects, int objectCount, Map *map) override;
};
