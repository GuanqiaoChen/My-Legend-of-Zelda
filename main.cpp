/**
* Author: Guanqiao Chen
* Assignment: Adventure
* Date due: 12/5/2025, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/MenuScene.h"
#include "CS3113/Level1.h"
#include "CS3113/Level2.h"
#include "CS3113/Level3.h"
#include "CS3113/ShaderProgram.h"
#include <cstdlib>
#include <ctime>

// Global Constants
constexpr int SCREEN_WIDTH     = 2000,
              SCREEN_HEIGHT    = 1200,
              FPS              = 120,
              NUMBER_OF_LEVELS = 3;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
MenuScene *gMenuScene = nullptr;
Level1 *gLevel1 = nullptr;
Level2 *gLevel2 = nullptr;
Level3 *gLevel3 = nullptr;

int gCurrentLevel = 0; 
int gLives = 10;
bool gGameWon = false;
bool gGameLost = false;
bool gPlayerCanAttack = false;
bool gPlayerAttacking = false;
float gAttackTimer = 0.0f;
Direction gFacing = DOWN;

Sound gWalkSound;
Sound gJumpSound;
Sound gDeathSound;
Sound gAttackSound;
ShaderProgram gShader;
Vector2 gLightPosition = {0.0f, 0.0f};

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{   
    gCurrentScene = scene;
    gCurrentScene->initialise();
}

void initialise()
{
    // Seed random number generator
    srand(static_cast<unsigned int>(time(nullptr)));
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Adventure Quest");
    InitAudioDevice();

    // Load sound effects
    gWalkSound = LoadSound("assets/game/player_walk_sound.wav");
    gJumpSound = LoadSound("assets/game/player_jump_sound.wav");
    gDeathSound = LoadSound("assets/game/player_death_sound.wav");
    gAttackSound = LoadSound("assets/game/player_attack_sound.wav");

    gShader.load("shaders/vertex.glsl", "shaders/fragment.glsl");

    // Create scenes
    gMenuScene = new MenuScene(ORIGIN, "#1a1a2e");
    gLevel1 = new Level1(ORIGIN, "#000000");
    gLevel2 = new Level2(ORIGIN, "#000000");
    gLevel3 = new Level3(ORIGIN, "#000000");

    switchToScene(gMenuScene);
    gLives = 10;
    gPlayerCanAttack = false;
    gPlayerAttacking = false;
    gAttackTimer = 0.0f;
    gFacing = DOWN;
    gGameWon = false;
    gGameLost = false;

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (gCurrentLevel == 0) return; // Menu handles its own input

    Entity *player = gCurrentScene->getState().xochitl;
    player->resetMovement();

    // Top-down style controls: WASD to move in 4 directions.
    bool moved = false;
    if (IsKeyDown(KEY_A)) { player->moveLeft();  gFacing = LEFT;  moved = true; }
    if (IsKeyDown(KEY_D)) { player->moveRight(); gFacing = RIGHT; moved = true; }
    if (IsKeyDown(KEY_W)) { player->moveUp();    gFacing = UP;    moved = true; }
    if (IsKeyDown(KEY_S)) { player->moveDown();  gFacing = DOWN;  moved = true; }

    if (gPlayerCanAttack && IsKeyPressed(KEY_J) && !gPlayerAttacking)
    {
        gPlayerAttacking = true;
        gAttackTimer = 0.2f;
        PlaySound(gAttackSound);
    }

    if (GetLength(player->getMovement()) > 1.0f) 
        player->normaliseMovement();

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }

    // Update light position in screen space
    if (gCurrentLevel != 0 && gCurrentScene && gCurrentScene->getState().xochitl)
    {
        gLightPosition = GetWorldToScreen2D(
            gCurrentScene->getState().xochitl->getPosition(),
            gCurrentScene->getState().camera
        );
    }

    if (gCurrentLevel != 0)
    {
        Entity* player = gCurrentScene->getState().xochitl;
        const float EPS = 1e-2f;
        
        if (player != nullptr) 
        {
            bool moving = GetLength(player->getMovement()) > EPS;
        
            if (moving) {
                if (!IsSoundPlaying(gWalkSound)) PlaySound(gWalkSound);
            } else {
                if (IsSoundPlaying(gWalkSound))  StopSound(gWalkSound);
            }
        }
    }

    // Handle scene transitions
    int nextSceneID = gCurrentScene->getState().nextSceneID;
    
    if (nextSceneID != 0)
    {
        if (IsSoundPlaying(gWalkSound)) StopSound(gWalkSound);

        if (nextSceneID == -2) // win
        {
            gGameWon = true;
            gGameLost = false;
            gCurrentLevel = 0;
            switchToScene(gMenuScene);
            return;
        }

        if (nextSceneID == 1 && gCurrentLevel == 0) // Start game from menu
        {
            gCurrentLevel = 1;
            gLives = 10;
            gPlayerCanAttack = false;
            gPlayerAttacking = false;
            gAttackTimer = 0.0f;
            gGameWon = false;
            gGameLost = false;
            switchToScene(gLevel1);
        }
        else if (nextSceneID > 0) // Go to next level
        {
            gCurrentLevel = nextSceneID;
            switch (nextSceneID)
            {
                case 1:
                    switchToScene(gLevel1);
                    break;
                case 2:
                    switchToScene(gLevel2);
                    break;
                case 3:
                    switchToScene(gLevel3);
                    break;
            }
        }
    }

}

void render()
{
    BeginDrawing();
    
    if (gCurrentLevel == 0)
    {
        // Menu rendering
        gCurrentScene->render();

        if (gGameWon)
        {
            const char* message = "You Win!";
            int fontSize = 50;
            int width = MeasureText(message, fontSize);
            DrawText(message, SCREEN_WIDTH / 2 - width / 2, SCREEN_HEIGHT / 2 - 100, fontSize, GREEN);
        }
        if (gGameLost)
        {
            const char* message = "You Lose!";
            int fontSize = 50;
            int width = MeasureText(message, fontSize);
            DrawText(message, SCREEN_WIDTH / 2 - width / 2, SCREEN_HEIGHT / 2 - 100, fontSize, RED);
        }
    }
    else
    {
        // Game rendering
        BeginMode2D(gCurrentScene->getState().camera);
        gShader.begin();
        gShader.setVector2("lightPosition", gLightPosition);
        gCurrentScene->render();
        gShader.end();
        EndMode2D();

        // Lives UI
        const char* livesText = TextFormat("Lives: %d", gLives);
        DrawText(livesText, 20, 20, 30, WHITE);
    }
    
    EndDrawing();
}

void shutdown() 
{
    delete gMenuScene;
    delete gLevel1;
    delete gLevel2;
    delete gLevel3;

    UnloadSound(gWalkSound);
    UnloadSound(gJumpSound);
    UnloadSound(gDeathSound);
    UnloadSound(gAttackSound);
    gShader.unload();

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}
