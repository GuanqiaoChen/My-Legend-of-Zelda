#include "Level1.h"
#include <cstdlib>

extern bool gPlayerCanAttack;
extern bool gPlayerAttacking;
extern float gAttackTimer;
extern Direction gFacing;

Level1::Level1() : Scene { {0.0f}, nullptr } {}
Level1::Level1(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

Level1::~Level1() { shutdown(); }

static unsigned int LEVEL1_DATA[LEVEL1_WIDTH * LEVEL1_HEIGHT];

void Level1::initialise()
{
    mGameState.nextSceneID = 0;
    mKeyFound          = false;
    mKeyCollected      = false;
    mKeyCollectionTimer = 0.0f;
    mLastFacing = DOWN;
    mLastAttack = false;

    // Build Level 1 index matrix (open layout, no floor/room walls)
    // Initialize all tiles to 0 (empty)
    for (int r = 0; r < LEVEL1_HEIGHT; ++r)
    {
        for (int c = 0; c < LEVEL1_WIDTH; ++c)
        {
            LEVEL1_DATA[r * LEVEL1_WIDTH + c] = 0;
        }
    }
    
    // Corners and outer boundary
    LEVEL1_DATA[0] = 1;
    LEVEL1_DATA[LEVEL1_WIDTH - 1] = 6;
    LEVEL1_DATA[(LEVEL1_HEIGHT - 1) * LEVEL1_WIDTH] = 41;
    LEVEL1_DATA[(LEVEL1_HEIGHT - 1) * LEVEL1_WIDTH + (LEVEL1_WIDTH - 1)] = 46;
    // Top wall
    for (int c = 1; c < LEVEL1_WIDTH - 1; ++c) LEVEL1_DATA[c] = 2;
    // Bottom wall
    for (int c = 1; c < LEVEL1_WIDTH - 1; ++c) LEVEL1_DATA[(LEVEL1_HEIGHT - 1) * LEVEL1_WIDTH + c] = 42;
    // Left/right walls
    for (int r = 1; r < LEVEL1_HEIGHT - 1; ++r)
    {
        LEVEL1_DATA[r * LEVEL1_WIDTH] = 11;
        LEVEL1_DATA[r * LEVEL1_WIDTH + (LEVEL1_WIDTH - 1)] = 16;
    }
    
    // Doors on outer walls for visual entry/exit
    int midRow = LEVEL1_HEIGHT / 2;
    LEVEL1_DATA[midRow * LEVEL1_WIDTH] = 37; // left door
    LEVEL1_DATA[midRow * LEVEL1_WIDTH + (LEVEL1_WIDTH - 1)] = 38; // right door
    
    // Internal walls (partial, don't create separate rooms)
    // Horizontal partial walls
    for (int c = 8; c < 18; ++c) LEVEL1_DATA[12 * LEVEL1_WIDTH + c] = 2;
    for (int c = 10; c < 20; ++c) LEVEL1_DATA[15 * LEVEL1_WIDTH + c] = 2;
    for (int c = 32; c < 42; ++c) LEVEL1_DATA[12 * LEVEL1_WIDTH + c] = 2;
    for (int c = 30; c < 40; ++c) LEVEL1_DATA[35 * LEVEL1_WIDTH + c] = 42;
    for (int c = 20; c < 30; ++c) LEVEL1_DATA[22 * LEVEL1_WIDTH + c] = 2;
    for (int c = 20; c < 30; ++c) LEVEL1_DATA[28 * LEVEL1_WIDTH + c] = 42;
    // Vertical partial walls
    for (int r = 8; r < 18; ++r) LEVEL1_DATA[r * LEVEL1_WIDTH + 12] = 11;
    for (int r = 10; r < 20; ++r) LEVEL1_DATA[r * LEVEL1_WIDTH + 15] = 11;
    for (int r = 32; r < 42; ++r) LEVEL1_DATA[r * LEVEL1_WIDTH + 12] = 11;
    for (int r = 30; r < 40; ++r) LEVEL1_DATA[r * LEVEL1_WIDTH + 35] = 16;
    for (int r = 20; r < 30; ++r) LEVEL1_DATA[r * LEVEL1_WIDTH + 22] = 11;
    // for (int r = 20; r < 30; ++r) LEVEL1_DATA[r * LEVEL1_WIDTH + 28] = 16;
    
    // Squared doors (67, 68)
    LEVEL1_DATA[10 * LEVEL1_WIDTH + 15] = 67;
    LEVEL1_DATA[10 * LEVEL1_WIDTH + 16] = 68;
    LEVEL1_DATA[38 * LEVEL1_WIDTH + 35] = 67;
    LEVEL1_DATA[38 * LEVEL1_WIDTH + 36] = 68;
    LEVEL1_DATA[20 * LEVEL1_WIDTH + 22] = 67;
    LEVEL1_DATA[20 * LEVEL1_WIDTH + 23] = 68;
    LEVEL1_DATA[30 * LEVEL1_WIDTH + 28] = 67;
    LEVEL1_DATA[30 * LEVEL1_WIDTH + 29] = 68;
    
    // Poles (47, 48, 57, 58)
    LEVEL1_DATA[6 * LEVEL1_WIDTH + 8] = 47;
    LEVEL1_DATA[6 * LEVEL1_WIDTH + 42] = 48;
    LEVEL1_DATA[8 * LEVEL1_WIDTH + 12] = 47;
    LEVEL1_DATA[8 * LEVEL1_WIDTH + 38] = 48;
    LEVEL1_DATA[42 * LEVEL1_WIDTH + 12] = 57;
    LEVEL1_DATA[42 * LEVEL1_WIDTH + 38] = 58;
    LEVEL1_DATA[44 * LEVEL1_WIDTH + 8] = 57;
    LEVEL1_DATA[44 * LEVEL1_WIDTH + 42] = 58;
    LEVEL1_DATA[20 * LEVEL1_WIDTH + 8] = 47;
    LEVEL1_DATA[20 * LEVEL1_WIDTH + 42] = 48;
    LEVEL1_DATA[30 * LEVEL1_WIDTH + 8] = 57;
    LEVEL1_DATA[30 * LEVEL1_WIDTH + 42] = 58;
    
    // Bones (69, 78)
    LEVEL1_DATA[10 * LEVEL1_WIDTH + 25] = 69;
    LEVEL1_DATA[10 * LEVEL1_WIDTH + 26] = 78;
    LEVEL1_DATA[14 * LEVEL1_WIDTH + 18] = 69;
    LEVEL1_DATA[14 * LEVEL1_WIDTH + 32] = 78;
    LEVEL1_DATA[36 * LEVEL1_WIDTH + 18] = 69;
    LEVEL1_DATA[36 * LEVEL1_WIDTH + 32] = 78;
    LEVEL1_DATA[40 * LEVEL1_WIDTH + 25] = 69;
    LEVEL1_DATA[40 * LEVEL1_WIDTH + 26] = 78;
    LEVEL1_DATA[18 * LEVEL1_WIDTH + 20] = 69;
    LEVEL1_DATA[18 * LEVEL1_WIDTH + 30] = 78;
    LEVEL1_DATA[32 * LEVEL1_WIDTH + 20] = 69;
    LEVEL1_DATA[32 * LEVEL1_WIDTH + 30] = 78;
    
    // Torches (91-96)
    LEVEL1_DATA[4 * LEVEL1_WIDTH + 8] = 91;
    LEVEL1_DATA[4 * LEVEL1_WIDTH + 42] = 92;
    LEVEL1_DATA[5 * LEVEL1_WIDTH + 10] = 91;
    LEVEL1_DATA[5 * LEVEL1_WIDTH + 40] = 92;
    LEVEL1_DATA[22 * LEVEL1_WIDTH + 5] = 93;
    LEVEL1_DATA[22 * LEVEL1_WIDTH + 45] = 94;
    LEVEL1_DATA[28 * LEVEL1_WIDTH + 5] = 93;
    LEVEL1_DATA[28 * LEVEL1_WIDTH + 45] = 94;
    LEVEL1_DATA[45 * LEVEL1_WIDTH + 10] = 95;
    LEVEL1_DATA[45 * LEVEL1_WIDTH + 40] = 96;
    LEVEL1_DATA[46 * LEVEL1_WIDTH + 8] = 95;
    LEVEL1_DATA[46 * LEVEL1_WIDTH + 42] = 96;
    
    // Magic potions (90, 98, 99)
    LEVEL1_DATA[16 * LEVEL1_WIDTH + 25] = 90;
    LEVEL1_DATA[18 * LEVEL1_WIDTH + 25] = 90;
    LEVEL1_DATA[25 * LEVEL1_WIDTH + 16] = 98;
    LEVEL1_DATA[25 * LEVEL1_WIDTH + 18] = 98;
    LEVEL1_DATA[25 * LEVEL1_WIDTH + 32] = 99;
    LEVEL1_DATA[25 * LEVEL1_WIDTH + 34] = 99;
    LEVEL1_DATA[34 * LEVEL1_WIDTH + 25] = 90;
    LEVEL1_DATA[32 * LEVEL1_WIDTH + 25] = 90;
    
    // Feather (65)
    LEVEL1_DATA[30 * LEVEL1_WIDTH + 25] = 65;
    LEVEL1_DATA[20 * LEVEL1_WIDTH + 15] = 65;
    LEVEL1_DATA[20 * LEVEL1_WIDTH + 35] = 65;
    LEVEL1_DATA[30 * LEVEL1_WIDTH + 15] = 65;
    LEVEL1_DATA[30 * LEVEL1_WIDTH + 35] = 65;
    
    // Flag (75)
    LEVEL1_DATA[5 * LEVEL1_WIDTH + 25] = 75;
    LEVEL1_DATA[7 * LEVEL1_WIDTH + 15] = 75;
    LEVEL1_DATA[7 * LEVEL1_WIDTH + 35] = 75;
    LEVEL1_DATA[43 * LEVEL1_WIDTH + 25] = 75;
    
    // Tables (81-86)
    LEVEL1_DATA[26 * LEVEL1_WIDTH + 18] = 81;
    LEVEL1_DATA[26 * LEVEL1_WIDTH + 32] = 82;
    LEVEL1_DATA[28 * LEVEL1_WIDTH + 20] = 81;
    LEVEL1_DATA[28 * LEVEL1_WIDTH + 30] = 82;
    LEVEL1_DATA[32 * LEVEL1_WIDTH + 20] = 83;
    LEVEL1_DATA[32 * LEVEL1_WIDTH + 30] = 84;
    LEVEL1_DATA[24 * LEVEL1_WIDTH + 25] = 85;
    LEVEL1_DATA[34 * LEVEL1_WIDTH + 25] = 86;
    
    for (int i = 0; i < LEVEL1_WIDTH * LEVEL1_HEIGHT; ++i)
        mLevelData[i] = LEVEL1_DATA[i];

    mGameState.bgm = LoadMusicStream("assets/game/looped_background_music.wav");
    SetMusicVolume(mGameState.bgm, 0.5f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/player_jump_sound.wav");

    mGameState.map = new Map(
        LEVEL1_WIDTH, LEVEL1_HEIGHT,
        (unsigned int *) mLevelData,
        "assets/game/Dungeon_Tileset.png",
        TILE_DIMENSION,
        10, 10,  
        mOrigin
    );

    // Player starts facing down, textures swap based on direction/attack
    std::map<Direction, std::vector<int>> playerWalkAtlas = {
        {DOWN,  {0,1,2,3,4,5,6,7}},
        {UP,    {0,1,2,3,4,5,6,7}},
        {LEFT,  {0,1,2,3,4,5,6,7}},
        {RIGHT, {0,1,2,3,4,5,6,7}}
    };

    mGameState.xochitl = new Entity(
        {mOrigin.x, mOrigin.y},
        {200.0f, 200.0f},
        "assets/game/player_walk_down.png",
        ATLAS,
        {1.0f, 8.0f},
        playerWalkAtlas,
        PLAYER
    );
    mGameState.xochitl->setDirection(DOWN); 
    mGameState.xochitl->setFrameSpeed(Entity::DEFAULT_FRAME_SPEED);
    mGameState.xochitl->setColliderDimensions({50.0f, 50.0f});
    mGameState.xochitl->setAcceleration({0.0f, 0.0f});
    mGameState.xochitl->setSpeed(200);
    mGameState.spawnPosition = mGameState.xochitl->getPosition();

    // --- Setup chests (random positions) ---
    // Random number of chests between 3 and MAX_CHESTS
    mActiveChestCount = 3 + (rand() % (Level1::MAX_CHESTS - 3 + 1));

    // Pick a random chest to hold the key
    mChestWithKeyIndex = rand() % mActiveChestCount;

    auto randomEmptyTile = [&]() {
        int gx = 0, gy = 0;
        for (int tries = 0; tries < 500; ++tries)
        {
            gx = 2 + (rand() % (LEVEL1_WIDTH - 3 - 2 + 1));
            gy = 2 + (rand() % (LEVEL1_HEIGHT - 3 - 2 + 1));
            if (LEVEL1_DATA[gy * LEVEL1_WIDTH + gx] == 0) break;
        }
        return std::pair<int,int>(gx, gy);
    };

    for (int i = 0; i < Level1::MAX_CHESTS; ++i)
    {
        mChestOpened[i] = false;

        auto tile = randomEmptyTile();
        float x = mGameState.map->getLeftBoundary() + tile.first * TILE_DIMENSION;
        float y = mGameState.map->getTopBoundary() + tile.second * TILE_DIMENSION;
        mChests[i] = new Entity(
            { x, y },
            { 80.0f, 80.0f },
            "assets/game/chest.png",
            ATLAS,
            {1.0f, 4.0f},
            std::map<Direction, std::vector<int>>{{RIGHT, {0}}},
            NPC
        );
        mChests[i]->setFrameSpeed(10);
        mChests[i]->setColliderDimensions({50.0f, 40.0f});

        if (i >= mActiveChestCount)
        {
            // Hide unused chests
            mChests[i]->deactivate();
        }
    }

    // Key entity (initially inactive) – make it visually larger
    mKeyEntity = new Entity({0,0}, {80.0f,80.0f}, "assets/game/keys.png", PLAYER);
    mKeyEntity->deactivate();

    // Wanderer NPC that grants attack
    std::map<Direction, std::vector<int>> wandererAtlas = {
        {LEFT,  {0,1,2,3,4,5,6,7,8,9}},
        {RIGHT, {0,1,2,3,4,5,6,7,8,9}}
    };
    mWanderer = new Entity(
        {mOrigin.x - 800.0f, mOrigin.y + 800.0f},
        {150.0f, 150.0f},
        "assets/game/wanderer_walk.png",
        ATLAS,
        {1.0f, 10.0f},
        wandererAtlas,
        NPC
    );
    mWanderer->setAIType(WANDERER);
    mWanderer->setAIState(WALKING);
    mWanderer->setSpeed(80);
    mWanderer->setColliderDimensions({50.0f, 50.0f});
    mWanderer->setEntityType(NONE); // disable built-in AI, use manual wander
    mWandererOrigin = mWanderer->getPosition();

    // Flame (attack unlock), initially hidden
    mFlame = new Entity({0,0}, {80.0f,80.0f}, "assets/game/flamethrower.png", PLAYER);
    mFlame->deactivate();
    mFlameDropped = false;
    mFlameCollected = false;

    mGameState.camera = { 0 };
    mGameState.camera.target = mGameState.xochitl->getPosition();
    mGameState.camera.offset = mOrigin;
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom = 1.0f;
}

void Level1::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    // Update player
    mGameState.xochitl->update(
        deltaTime,
        nullptr,
        mGameState.map,
        nullptr,
        0
    );

    // Wanderer manual wander: bounce in a small horizontal range, stop near player
    float wanderDist = Vector2Distance(mGameState.xochitl->getPosition(), mWanderer->getPosition());
    if (wanderDist < 140.0f)
    {
        mWanderer->resetMovement();
    }
    else
    {
        float leftLimit = mWandererOrigin.x - 10.0f;
        float rightLimit = mWandererOrigin.x + 500.0f;
        Vector2 wp = mWanderer->getPosition();
        if (wp.x <= leftLimit) mWandererDir = 1;
        if (wp.x >= rightLimit) mWandererDir = -1;
        if (mWandererDir > 0) mWanderer->moveRight(); else mWanderer->moveLeft();
    }
    mWanderer->update(
        deltaTime,
        mGameState.xochitl,
        mGameState.map,
        nullptr,
        0
    );
    // Clamp wanderer inside map
    Vector2 wp = mWanderer->getPosition();
    float wminX = mGameState.map->getLeftBoundary() + 40.0f;
    float wmaxX = mGameState.map->getRightBoundary() - 40.0f;
    float wminY = mGameState.map->getTopBoundary() + 40.0f;
    float wmaxY = mGameState.map->getBottomBoundary() - 40.0f;
    wp.x = fmaxf(wminX, fminf(wmaxX, wp.x));
    wp.y = fmaxf(wminY, fminf(wmaxY, wp.y));
    mWanderer->setPosition(wp);

    // Interact with wanderer to drop flame
    if (!mFlameDropped)
    {
        float dist = Vector2Distance(mGameState.xochitl->getPosition(), mWanderer->getPosition());
        // Make sure player is facing the wanderer
        Vector2 dirToWanderer = Vector2Subtract(mWanderer->getPosition(), mGameState.xochitl->getPosition());
        bool facing = false;
        switch (gFacing)
        {
        case LEFT:  facing = dirToWanderer.x < 0 && fabs(dirToWanderer.y) < TILE_DIMENSION; break;
        case RIGHT: facing = dirToWanderer.x > 0 && fabs(dirToWanderer.y) < TILE_DIMENSION; break;
        case UP:    facing = dirToWanderer.y < 0 && fabs(dirToWanderer.x) < TILE_DIMENSION; break;
        case DOWN:  facing = dirToWanderer.y > 0 && fabs(dirToWanderer.x) < TILE_DIMENSION; break;
        default: break;
        }

        if (dist < 90.0f && facing && IsKeyPressed(KEY_E))
        {
            mFlameDropped = true;
            mFlame->setPosition({mWanderer->getPosition().x, mWanderer->getPosition().y + 40.0f});
            mFlame->activate();
        }
    }

    // Picking up the flame unlocks attack ability
    if (mFlame->isActive())
    {
        float d = Vector2Distance(mGameState.xochitl->getPosition(), mFlame->getPosition());
        if (d < 50.0f)
        {
            mFlame->deactivate();
            mFlameCollected = true;
            gPlayerCanAttack = true;
        }
    }

    // Open chests with E when nearby
    for (int i = 0; i < mActiveChestCount; ++i)
    {
        if (!mChestOpened[i])
        {
            float dist = Vector2Distance(mGameState.xochitl->getPosition(), mChests[i]->getPosition());
            if (dist < 80.0f && IsKeyPressed(KEY_E))
            {
                // open chest fully (use right‑most frame in atlas)
                mChestOpened[i] = true;
                mChests[i]->setAnimationAtlas({{RIGHT, {3}}});

                if (i == mChestWithKeyIndex)
                {
                    // spawn key on chest
                    mKeyEntity->setPosition(mChests[i]->getPosition());
                    mKeyEntity->activate();
                    mKeyFound = true;
                }
            }
        }
    }

    // Pick up key when touching it, then wait before level transition
    if (mKeyEntity->isActive() && !mKeyCollected)
    {
        float d = Vector2Distance(mGameState.xochitl->getPosition(), mKeyEntity->getPosition());
        if (d < 60.0f)
        {
            // Keep key visible briefly before transitioning
            mKeyCollected = true;
            mKeyCollectionTimer = 1.5f; // seconds before changing level
        }
    }

    // Hide key and move to next level after delay
    if (mKeyCollected)
    {
        mKeyCollectionTimer -= deltaTime;
        if (mKeyCollectionTimer <= 0.0f)
        {
            mKeyEntity->deactivate();
            mGameState.nextSceneID = 2;
        }
    }

    // Tick attack timer
    if (gPlayerAttacking)
    {
        gAttackTimer -= deltaTime;
        if (gAttackTimer <= 0.0f)
        {
            gPlayerAttacking = false;
        }
    }

    // Swap player texture based on facing and attack state
    bool attackState = gPlayerAttacking;
    if (attackState != mLastAttack || gFacing != mLastFacing)
    {
        const char* texturePath = "assets/game/player_walk_down.png";
        switch (gFacing)
        {
        case DOWN:  texturePath = attackState ? "assets/game/player_attack_down.png" : "assets/game/player_walk_down.png"; break;
        case UP:    texturePath = attackState ? "assets/game/player_attack_up.png"   : "assets/game/player_walk_up.png";   break;
        case LEFT:  texturePath = attackState ? "assets/game/player_attack_left.png" : "assets/game/player_walk_left.png"; break;
        case RIGHT: texturePath = attackState ? "assets/game/player_attack_right.png": "assets/game/player_walk_right.png";break;
        default: break;
        }
        mGameState.xochitl->setTexture(texturePath);
        mGameState.xochitl->setSpriteSheetDimensions({1.0f, 8.0f});
        std::map<Direction, std::vector<int>> atlas = {
            {gFacing, {0,1,2,3,4,5,6,7}}
        };
        mGameState.xochitl->setAnimationAtlas(atlas);
        mGameState.xochitl->setDirection(gFacing);
        mLastFacing = gFacing;
        mLastAttack = attackState;
    }
    
    // Keep attack animation playing even when idle
    if (gPlayerAttacking)
    {
        mGameState.xochitl->forceAnimate(deltaTime);
    }
    else
    {
        mGameState.xochitl->resetForceAnimation();
    }

    // Keep player within map boundaries
    Vector2 p = mGameState.xochitl->getPosition();
    float halfW = mGameState.xochitl->getColliderDimensions().x / 2.0f;
    float halfH = mGameState.xochitl->getColliderDimensions().y / 2.0f;
    float minX = mGameState.map->getLeftBoundary() + halfW;
    float maxX = mGameState.map->getRightBoundary() - halfW;
    float minY = mGameState.map->getTopBoundary() + halfH;
    float maxY = mGameState.map->getBottomBoundary() - halfH;
    p.x = fmaxf(minX, fminf(maxX, p.x));
    p.y = fmaxf(minY, fminf(maxY, p.y));
    mGameState.xochitl->setPosition(p);

    // Camera follows player (x and y)
    Vector2 currentPlayerPosition = { p.x, p.y };
    panCamera(&mGameState.camera, &currentPlayerPosition);
}

void Level1::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    mGameState.xochitl->render();
    mWanderer->render();
    if (mFlame && mFlame->isActive()) mFlame->render();
    for (int i = 0; i < mActiveChestCount; ++i)
        mChests[i]->render();

    if (mKeyEntity && mKeyEntity->isActive()) mKeyEntity->render();
}

void Level1::shutdown()
{
    delete mGameState.xochitl;
    delete mGameState.map;
    delete mWanderer;
    delete mFlame;
    for (int i = 0; i < MAX_CHESTS; ++i) {
        delete mChests[i];
    }
    delete mKeyEntity;

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
}

