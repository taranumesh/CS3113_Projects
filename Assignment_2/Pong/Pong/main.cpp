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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, modelMatrix;
glm::mat4 modelMatrixp1, modelMatrixp2, modelMatrixball, modelMatrixtree1, modelMatrixtree2, modelMatrixwin;

glm::vec3 tree_position1 = glm::vec3(-4.7, 0, 0);
glm::vec3 tree_position2 = glm::vec3(4.7, 0, 0);

glm::vec3 tree_scale = glm::vec3(1, 10, 0);

glm::vec3 player1_position = glm::vec3(4.0, 0, 0);
glm::vec3 player1_movement = glm::vec3(0, 0, 0);

glm::vec3 player2_position = glm::vec3(-4.0, 0, 0);
glm::vec3 player2_movement = glm::vec3(0, 0, 0);

glm::vec3 ball_position = glm::vec3(0, 0, 0);
glm::vec3 ball_movement = glm::vec3(1.0, 1.0, 0);

glm::vec3 player_scaling = glm::vec3(0.6, 2.0, 0.0);
glm::vec3 ball_scaling = glm::vec3(1.0, 1.0, 0.0);

glm::vec3 win_position = glm::vec3(0, 3, 0);
glm::vec3 win_scale = glm::vec3(2, 2, 0);

float player_height = 1.0 * player_scaling.y;
float player_width = 1.0 * player_scaling.x;


float player_speed = 4.0f;
float ball_speed = 2.0f;

int p1_score = 0;
int p2_score = 0;
bool reset = true;

GLuint player1TextureID, player2TextureID, ballTextureID, treeTextureID, winp1TextureID, winp2TextureID;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Texture", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
//    program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    
    modelMatrixtree1 = glm::translate(modelMatrix, tree_position1);
    modelMatrixtree1 = glm::scale(modelMatrixtree1, tree_scale);
    modelMatrixtree2 = glm::scale(glm::translate(modelMatrix, tree_position2), tree_scale);
    modelMatrixwin = glm::translate(modelMatrix, win_position);
    modelMatrixwin = glm::scale(modelMatrixwin, win_scale);
    
    glUseProgram(program.programID);
    
    glClearColor(0.3f, 0.65f, 0.85f, 1.0f);
    
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    player1TextureID = LoadTexture("slug1.png");
    player2TextureID = LoadTexture("slug2.png");
    ballTextureID = LoadTexture("shell.png");
    treeTextureID = LoadTexture("tree.png");
    winp1TextureID = LoadTexture("slug1won.png");
    winp2TextureID = LoadTexture("slug2won.png");
}

void ProcessInput() {
    player1_movement = glm::vec3(0, 0, 0);
    player2_movement = glm::vec3(0, 0, 0);
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type){
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_SPACE:
                        break;
                    case SDLK_RIGHT:
                        break;
                    case SDLK_LEFT:
                        break;
                }
        }
    }
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if ((keys[SDL_SCANCODE_UP]) && ((player1_position.y + (player_height/2)) < 3.75)) {
        player1_movement.y = 1.0f;
    }
    
    if ((keys[SDL_SCANCODE_DOWN]) && ((player1_position.y - (player_height/2)) > -3.75)){
        player1_movement.y = -1.0f;
    }
    
    if ((keys[SDL_SCANCODE_W]) && ((player2_position.y + (player_height/2)) < 3.75)) {
        player2_movement.y = 1.0;
    }
    
    if ((keys[SDL_SCANCODE_S]) && ((player2_position.y - (player_height/2)) > -3.75)) {
        player2_movement.y = -1.0;
    }
    
    if (keys[SDL_SCANCODE_SPACE]) {
        reset = false;
        p1_score = 0;
        p2_score = 0;
    }
    
}
float lastTicks = 0.0f;

void update_player1(float deltaTime) {
    player1_position += player1_movement * player_speed * deltaTime;
    modelMatrixp1 = glm::translate(modelMatrix, player1_position);
    modelMatrixp1 = glm::scale(modelMatrixp1, player_scaling);
}

void update_player2(float deltaTime) {
    player2_position += player2_movement * player_speed * deltaTime;
    modelMatrixp2 = glm::translate(modelMatrix, player2_position);
    modelMatrixp2 = glm::scale(modelMatrixp2, player_scaling);
}

void update_ball(float deltaTime) {
    if (!reset) {
        if (((ball_position.y + 0.5) > 3.75) || ((ball_position.y - 0.5) < -3.75)) ball_movement.y *= -1.0;
        float x_dist_p1 = fabs(ball_position.x - player1_position.x) - ((player_width + 0.5)/2);
        float y_dist_p1 = fabs(ball_position.y - player1_position.y) - ((player_height + 0.5)/2);
        float x_dist_p2 = fabs(ball_position.x - player2_position.x) - ((player_width + 0.5)/2);
        float y_dist_p2 = fabs(ball_position.y - player2_position.y) - ((player_height + 0.5)/2);
        if ( ((x_dist_p1 < 0) && (y_dist_p1 < 0)) || ((x_dist_p2 < 0) && (y_dist_p2 < 0)) ) {
            ball_movement.x *= -1.0;
        }
        ball_position += ball_movement * player_speed * deltaTime;
        if ((ball_position.x + 0.5) > 5.0) {
            p2_score += 1;
            ball_position = glm::vec3(0, 0, 0);
            ball_speed = 2.0;
            player_speed = 4.0;
            int random = rand() % 4;
            switch(random) {
                case 0:
                    ball_movement = glm::vec3(1.0, 1.0, 0);
                    break;
                case 1:
                    ball_movement = glm::vec3(1.0, -1.0, 0);
                    break;
                case 2:
                    ball_movement = glm::vec3(-1.0, 1.0, 0);
                    break;
                case 3:
                    ball_movement = glm::vec3(-1.0, -1.0, 0);
                    break;
            }
        } else if ((ball_position.x - 0.5) < -5.0) {
            p1_score += 1;
            ball_position = glm::vec3(0, 0, 0);
            ball_speed = 2.0;
            player_speed = 4.0;
            int random = rand() % 4;
            switch(random) {
                case 0:
                    ball_movement = glm::vec3(1.0, 1.0, 0);
                    break;
                case 1:
                    ball_movement = glm::vec3(1.0, -1.0, 0);
                    break;
                case 2:
                    ball_movement = glm::vec3(-1.0, 1.0, 0);
                    break;
                case 3:
                    ball_movement = glm::vec3(-1.0, -1.0, 0);
                    break;
            }
        }
    }
    modelMatrixball = glm::translate(modelMatrix, ball_position);
    modelMatrixball = glm::scale(modelMatrixball, ball_scaling);
}


void Update() {
    float ticks = (float)SDL_GetTicks()/1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    update_player1(deltaTime);
    update_player2(deltaTime);
    update_ball(deltaTime);
    ball_speed += 0.1*deltaTime;
    player_speed += 0.1*deltaTime;
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    program.SetModelMatrix(modelMatrixtree1);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, treeTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    program.SetModelMatrix(modelMatrixtree2);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, treeTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    program.SetModelMatrix(modelMatrixp1);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, player1TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    program.SetModelMatrix(modelMatrixp2);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, player2TextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    program.SetModelMatrix(modelMatrixball);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, ballTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    if (p1_score == 10) {
        program.SetModelMatrix(modelMatrixwin);
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glBindTexture(GL_TEXTURE_2D, winp1TextureID);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        reset = true;
    } else if (p2_score == 10) {
        program.SetModelMatrix(modelMatrixwin);
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glBindTexture(GL_TEXTURE_2D, winp2TextureID);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        reset = true;
    }
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
