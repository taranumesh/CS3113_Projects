#ifndef Level1_h
#define Level1_h

#include "Scene.h"

class Level1 : public Scene {
public:
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
    void WaterPlant(void) override;
    void HarvestPlant(void) override;
    void PlacePlant(void) override;
    void NextScene(void) override;
};

#endif
