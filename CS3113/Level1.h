#include "Scene.h"

#ifndef LEVEL1_H
#define LEVEL1_H

constexpr int LEVEL1_WIDTH = 50,
              LEVEL1_HEIGHT = 50;

class Level1 : public Scene {
private:
    unsigned int mLevelData[LEVEL1_WIDTH * LEVEL1_HEIGHT];

    // Chests and key state
    static constexpr int MAX_CHESTS = 6;
    Entity *mChests[MAX_CHESTS];
    bool mChestOpened[MAX_CHESTS];
    int  mChestWithKeyIndex;
    int  mActiveChestCount;
    Entity *mKeyEntity; // Shows up when player opens the key chest
    bool mKeyFound;
    bool mKeyCollected;
    float mKeyCollectionTimer;

    // Wanderer NPC and flame
    Entity *mWanderer;
    Entity *mFlame;
    bool mFlameDropped;
    bool mFlameCollected;
    Vector2 mWandererOrigin;
    int mWandererDir = 1; // 1 right, -1 left

    // For swapping player animations
    Direction mLastFacing = DOWN;
    bool mLastAttack = false;

public:
    static constexpr float TILE_DIMENSION       = 75.0f,
                        ACCELERATION_OF_GRAVITY = 981.0f; // kept for future use

    Level1();
    Level1(Vector2 origin, const char *bgHexCode);
    ~Level1();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif

