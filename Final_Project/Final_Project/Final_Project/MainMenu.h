//
//  MainMenu.hpp
//  Final_Project
//
//  Created by Tara Umesh on 5/10/20.
//  Copyright © 2020 Tara Umesh. All rights reserved.
//

#ifndef MainMenu_h
#define MainMenu_h

#include "Scene.h"

class MainMenu : public Scene {
public:
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
    void WaterPlant(void) override;
    void HarvestPlant(void) override;
    void PlacePlant(void) override;
};


#endif
