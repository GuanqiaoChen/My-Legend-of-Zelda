#include "MenuScene.h"

constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 600;

// External global variables
extern bool gGameWon;
extern bool gGameLost;

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
        if (gGameWon || gGameLost)
        {
            // Return to menu from win/lose screens
            gGameWon = false;
            gGameLost = false;
            // Not set nextSceneID, just reset flags to show menu
        }
        else
        {
            // Start game from normal menu
            mGameState.nextSceneID = 1; // Go to first level
        }
    }
}

void MenuScene::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Check if we should show win/lose screen instead of normal menu
    if (gGameWon)
    {
        // Win screen
        const char* winMessage = "You Win!";
        const char* instruction = "Press Enter to Return to Menu";
        
        int winFontSize = 80;
        int instructionFontSize = 35;
        
        int winWidth = MeasureText(winMessage, winFontSize);
        int instructionWidth = MeasureText(instruction, instructionFontSize);
        
        DrawText(winMessage, 
                 screenWidth / 2 - winWidth / 2, 
                 screenHeight / 2 - 100, 
                 winFontSize, 
                 GREEN);
        
        DrawText(instruction, 
                 screenWidth / 2 - instructionWidth / 2, 
                 screenHeight / 2 + 50, 
                 instructionFontSize, 
                 WHITE);
        
        return;
    }
    
    if (gGameLost)
    {
        // Lose screen
        const char* loseMessage = "You Lose!";
        const char* instruction = "Press Enter to Return to Menu";
        
        int loseFontSize = 80;
        int instructionFontSize = 35;
        
        int loseWidth = MeasureText(loseMessage, loseFontSize);
        int instructionWidth = MeasureText(instruction, instructionFontSize);
        
        DrawText(loseMessage, 
                 screenWidth / 2 - loseWidth / 2, 
                 screenHeight / 2 - 100, 
                 loseFontSize, 
                 RED);
        
        DrawText(instruction, 
                 screenWidth / 2 - instructionWidth / 2, 
                 screenHeight / 2 + 50, 
                 instructionFontSize, 
                 WHITE);
        
        return;
    }
    
    // Normal menu with tutorial
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

