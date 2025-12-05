#include "Scene.h"

#ifndef LEVEL2_H
#define LEVEL2_H

constexpr int LEVEL2_WIDTH = 50,
              LEVEL2_HEIGHT = 50;

class Level2 : public Scene {
private:
    unsigned int mLevelData[LEVEL2_WIDTH * LEVEL2_HEIGHT];

    // Chests and key state for level2
    static constexpr int MAX_CHESTS = 6;
    Entity *mChests[MAX_CHESTS];
    bool mChestOpened[MAX_CHESTS];
    int  mChestWithKeyIndex;
    int  mActiveChestCount;
    Entity *mKeyEntity;
    bool mKeyFound;
    bool mKeyCollected;
    float mKeyCollectionTimer;

    // Skeletons
    static constexpr int MAX_SKELETONS = 160;
    Entity *mSkeletons[MAX_SKELETONS];
    int mSkeletonHP[MAX_SKELETONS];

    // Animation state tracking
    Direction mLastFacing = DOWN;
    bool mLastAttack = false;
    
    // Invincibility timer after respawn
    float mInvincibilityTimer = 0.0f;

public:
    static constexpr float TILE_DIMENSION       = 75.0f,
                        ACCELERATION_OF_GRAVITY = 981.0f;

    Level2();
    Level2(Vector2 origin, const char *bgHexCode);
    ~Level2();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif

