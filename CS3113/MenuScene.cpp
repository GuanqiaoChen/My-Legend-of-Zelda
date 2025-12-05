#include "MenuScene.h"

constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 600;

MenuScene::MenuScene() : Scene { {0.0f}, nullptr } {}
MenuScene::MenuScene(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

MenuScene::~MenuScene() { shutdown(); }

void MenuScene::initialise()
{
    mGameState.nextSceneID = 0;
}

void MenuScene::update(float deltaTime)
{
    if (IsKeyPressed(KEY_ENTER))
    {
        mGameState.nextSceneID = 1; // Go to first level
    }
}

void MenuScene::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    
    int screenWidth = GetScreenWidth();
    
    const char* gameTitle = "Adventure";
    const char* instruction = "Press Enter to Start";
    
    int titleFontSize = 60;
    int instructionFontSize = 30;
    int tutorialFontSize = 20;
    int tutorialTitleFontSize = 28;
    
    int titleWidth = MeasureText(gameTitle, titleFontSize);
    int instructionWidth = MeasureText(instruction, instructionFontSize);
    
    // Draw title
    DrawText(gameTitle, 
             screenWidth / 2 - titleWidth / 2, 
             100, 
             titleFontSize, 
             WHITE);
    
    // Draw instruction
    DrawText(instruction, 
             screenWidth / 2 - instructionWidth / 2, 
             180, 
             instructionFontSize, 
             WHITE);
    
    // Draw tutorial section
    const char* tutorialTitle = "Tutorial";
    int tutorialTitleWidth = MeasureText(tutorialTitle, tutorialTitleFontSize);
    DrawText(tutorialTitle, 
             screenWidth / 2 - tutorialTitleWidth / 2, 
             250, 
             tutorialTitleFontSize, 
             YELLOW);
    
    // Tutorial content
    int tutorialY = 300;
    int tutorialLineSpacing = 35;
    Color tutorialColor = LIGHTGRAY;
    
    DrawText("Controls:", screenWidth / 2 - 200, tutorialY, tutorialFontSize, WHITE);
    tutorialY += tutorialLineSpacing;
    
    DrawText("W/A/S/D - Move Up/Left/Down/Right", screenWidth / 2 - 200, tutorialY, tutorialFontSize, tutorialColor);
    tutorialY += tutorialLineSpacing;
    
    DrawText("J - Attack (when unlocked)", screenWidth / 2 - 200, tutorialY, tutorialFontSize, tutorialColor);
    tutorialY += tutorialLineSpacing;
    
    DrawText("E - Interact (NPCs, Chests)", screenWidth / 2 - 200, tutorialY, tutorialFontSize, tutorialColor);
    tutorialY += tutorialLineSpacing;
    
    DrawText("Q - Quit Game", screenWidth / 2 - 200, tutorialY, tutorialFontSize, tutorialColor);
    tutorialY += tutorialLineSpacing * 2;
    
    DrawText("Gameplay:", screenWidth / 2 - 200, tutorialY, tutorialFontSize, WHITE);
    tutorialY += tutorialLineSpacing;
    
    DrawText("- You start with 10 lives", screenWidth / 2 - 200, tutorialY, tutorialFontSize, tutorialColor);
    tutorialY += tutorialLineSpacing;
    
    DrawText("- Explore levels and collect items", screenWidth / 2 - 200, tutorialY, tutorialFontSize, tutorialColor);
    tutorialY += tutorialLineSpacing;
    
    DrawText("- Interact with NPCs to unlock abilities", screenWidth / 2 - 200, tutorialY, tutorialFontSize, tutorialColor);
    tutorialY += tutorialLineSpacing;
    
    DrawText("- Open chests to find keys and progress", screenWidth / 2 - 200, tutorialY, tutorialFontSize, tutorialColor);
    tutorialY += tutorialLineSpacing;
    
    DrawText("- Complete all 3 levels to win!", screenWidth / 2 - 200, tutorialY, tutorialFontSize, tutorialColor);
}

void MenuScene::shutdown()
{
    // Nothing to clean up
}

