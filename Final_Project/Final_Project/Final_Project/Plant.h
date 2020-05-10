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

enum plantState {
    SEED,
    SEEDLING,
    ADULT,
    FLOWERING,
    DEAD
};

class Plant: public Entity {
public:
    float timer;
    float water_level;
    enum plantState state;
    Plant();
    void Update(float deltaTime, Entity *objects, int objectCount, Map *map) override;
    void PlacePlant(glm::vec3 plant_position);
    void WaterPlant(void);
    void HarvestPlant(void);
    void Render_Water_Level(ShaderProgram *program);
};
