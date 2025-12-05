#include "Entity.h"

#ifndef SCENE_H
#define SCENE_H

struct GameState
{
    Entity *xochitl;
    Map *map;

    Music bgm;
    Sound jumpSound;
    Sound attackSound;
    Sound deathSound;

    Camera2D camera;

    int nextSceneID;

    // Combat / progression flags
    bool canAttack = false;
    bool isAttacking = false;
    float attackTimer = 0.0f;

    // Lives and spawn point
    int lives = 10;
    Vector2 spawnPosition;
};


class Scene 
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char *mBGColourHexCode = "#000000";
    
public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    
    GameState   getState()           const { return mGameState; }
    Vector2     getOrigin()          const { return mOrigin;    }
    const char* getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif