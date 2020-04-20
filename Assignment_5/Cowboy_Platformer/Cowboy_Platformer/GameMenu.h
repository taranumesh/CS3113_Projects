#include "Scene.h"

class GameMenu : public Scene {
private:
    GLuint titleTextureID;
    void RenderTitle(ShaderProgram *program, glm::vec3 position, glm::vec3 scale);
public:
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
    void ResetLevel() override;
    
};
