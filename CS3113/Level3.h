#include "Scene.h"

#ifndef LEVEL3_H
#define LEVEL3_H

constexpr int LEVEL3_WIDTH = 50,
              LEVEL3_HEIGHT = 50;

class Level3 : public Scene {
private:
    unsigned int mLevelData[LEVEL3_WIDTH * LEVEL3_HEIGHT];
    static constexpr int MAX_DEMONS = 50;
    Entity *mDemons[MAX_DEMONS];
    int mDemonHP[MAX_DEMONS];

    // Key/chest for win
    Entity *mChest;
    bool mChestOpened;
    Entity *mKeyEntity;
    bool mKeyCollected;
    float mKeyCollectionTimer;

    // Animation state tracking
    Direction mLastFacing = DOWN;
    bool mLastAttack = false;
    
    // Invincibility timer after respawn
    float mInvincibilityTimer = 0.0f;

public:
    static constexpr float TILE_DIMENSION       = 75.0f,
                        ACCELERATION_OF_GRAVITY = 981.0f;

    Level3();
    Level3(Vector2 origin, const char *bgHexCode);
    ~Level3();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif

