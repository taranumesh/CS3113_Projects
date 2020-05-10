#ifndef Level2_h
#define Level2_h

#include "Scene.h"

class Level2 : public Scene {
public:
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
};

#endif
