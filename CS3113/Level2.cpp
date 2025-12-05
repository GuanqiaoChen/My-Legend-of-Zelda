#include "Level2.h"
#include "MenuScene.h"
#include <cstdlib>

extern MenuScene *gMenuScene;
extern void switchToScene(Scene *scene);

extern bool gPlayerAttacking;
extern float gAttackTimer;
extern Direction gFacing;
extern bool gGameLost;
extern int gLives;
extern int gCurrentLevel;
extern Sound gDeathSound;

Level2::Level2() : Scene { {0.0f}, nullptr } {}
Level2::Level2(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

Level2::~Level2() { shutdown(); }

static unsigned int LEVEL2_DATA[LEVEL2_WIDTH * LEVEL2_HEIGHT];

void Level2::initialise()
{
    mGameState.nextSceneID = 0;
    mKeyFound          = false;
    mKeyCollected      = false;
    mKeyCollectionTimer = 0.0f;
    mLastFacing = DOWN;
    mLastAttack = false;
    mInvincibilityTimer = 1.0f; // Start with 1 second invincibility

    // Build Level 2 index matrix (open layout, no floor/room walls)
    // Initialize all tiles to 0 (empty)
    for (int r = 0; r < LEVEL2_HEIGHT; ++r)
    {
        for (int c = 0; c < LEVEL2_WIDTH; ++c)
            LEVEL2_DATA[r * LEVEL2_WIDTH + c] = 0;
    }
    
    // Corners and outer boundary
    LEVEL2_DATA[0] = 1;
    LEVEL2_DATA[LEVEL2_WIDTH - 1] = 6;
    LEVEL2_DATA[(LEVEL2_HEIGHT - 1) * LEVEL2_WIDTH] = 41;
    LEVEL2_DATA[(LEVEL2_HEIGHT - 1) * LEVEL2_WIDTH + (LEVEL2_WIDTH - 1)] = 46;
    // Top wall
    for (int c = 1; c < LEVEL2_WIDTH - 1; ++c) LEVEL2_DATA[c] = 2;
    // Bottom wall
    for (int c = 1; c < LEVEL2_WIDTH - 1; ++c) LEVEL2_DATA[(LEVEL2_HEIGHT - 1) * LEVEL2_WIDTH + c] = 42;
    // Left/right walls
    for (int r = 1; r < LEVEL2_HEIGHT - 1; ++r)
    {
        LEVEL2_DATA[r * LEVEL2_WIDTH] = 11;
        LEVEL2_DATA[r * LEVEL2_WIDTH + (LEVEL2_WIDTH - 1)] = 16;
    }
    
    // Doors on outer walls for visual entry/exit
    int midRow = LEVEL2_HEIGHT / 2;
    LEVEL2_DATA[midRow * LEVEL2_WIDTH] = 37;
    LEVEL2_DATA[midRow * LEVEL2_WIDTH + (LEVEL2_WIDTH - 1)] = 38;
    
    // Internal walls (partial, don't create separate rooms)
    // Horizontal partial walls
    for (int c = 10; c < 20; ++c) LEVEL2_DATA[14 * LEVEL2_WIDTH + c] = 2;
    for (int c = 12; c < 25; ++c) LEVEL2_DATA[18 * LEVEL2_WIDTH + c] = 2;
    for (int c = 30; c < 40; ++c) LEVEL2_DATA[14 * LEVEL2_WIDTH + c] = 2;
    for (int c = 25; c < 38; ++c) LEVEL2_DATA[32 * LEVEL2_WIDTH + c] = 42;
    for (int c = 10; c < 20; ++c) LEVEL2_DATA[36 * LEVEL2_WIDTH + c] = 42;
    for (int c = 20; c < 30; ++c) LEVEL2_DATA[22 * LEVEL2_WIDTH + c] = 2;
    for (int c = 20; c < 30; ++c) LEVEL2_DATA[28 * LEVEL2_WIDTH + c] = 42;
    // Vertical partial walls
    for (int r = 10; r < 20; ++r) LEVEL2_DATA[r * LEVEL2_WIDTH + 14] = 11;
    for (int r = 12; r < 25; ++r) LEVEL2_DATA[r * LEVEL2_WIDTH + 18] = 11;
    for (int r = 30; r < 40; ++r) LEVEL2_DATA[r * LEVEL2_WIDTH + 14] = 11;
    for (int r = 25; r < 38; ++r) LEVEL2_DATA[r * LEVEL2_WIDTH + 32] = 16;
    for (int r = 10; r < 20; ++r) LEVEL2_DATA[r * LEVEL2_WIDTH + 36] = 16;
    for (int r = 20; r < 30; ++r) LEVEL2_DATA[r * LEVEL2_WIDTH + 22] = 11;
    // for (int r = 20; r < 30; ++r) LEVEL2_DATA[r * LEVEL2_WIDTH + 28] = 16;
    
    // Squared doors (67, 68)
    LEVEL2_DATA[12 * LEVEL2_WIDTH + 18] = 67;
    LEVEL2_DATA[12 * LEVEL2_WIDTH + 19] = 68;
    LEVEL2_DATA[16 * LEVEL2_WIDTH + 18] = 67;
    LEVEL2_DATA[16 * LEVEL2_WIDTH + 19] = 68;
    LEVEL2_DATA[34 * LEVEL2_WIDTH + 32] = 67;
    LEVEL2_DATA[34 * LEVEL2_WIDTH + 33] = 68;
    LEVEL2_DATA[38 * LEVEL2_WIDTH + 32] = 67;
    LEVEL2_DATA[38 * LEVEL2_WIDTH + 33] = 68;
    LEVEL2_DATA[20 * LEVEL2_WIDTH + 22] = 67;
    LEVEL2_DATA[20 * LEVEL2_WIDTH + 23] = 68;
    LEVEL2_DATA[30 * LEVEL2_WIDTH + 28] = 67;
    LEVEL2_DATA[30 * LEVEL2_WIDTH + 29] = 68;
    
    // Poles (47, 48, 57, 58)
    LEVEL2_DATA[8 * LEVEL2_WIDTH + 8] = 47;
    LEVEL2_DATA[8 * LEVEL2_WIDTH + 42] = 48;
    LEVEL2_DATA[10 * LEVEL2_WIDTH + 10] = 47;
    LEVEL2_DATA[10 * LEVEL2_WIDTH + 40] = 48;
    LEVEL2_DATA[40 * LEVEL2_WIDTH + 10] = 57;
    LEVEL2_DATA[40 * LEVEL2_WIDTH + 40] = 58;
    LEVEL2_DATA[42 * LEVEL2_WIDTH + 8] = 57;
    LEVEL2_DATA[42 * LEVEL2_WIDTH + 42] = 58;
    LEVEL2_DATA[15 * LEVEL2_WIDTH + 5] = 47;
    LEVEL2_DATA[15 * LEVEL2_WIDTH + 45] = 48;
    LEVEL2_DATA[35 * LEVEL2_WIDTH + 5] = 57;
    LEVEL2_DATA[35 * LEVEL2_WIDTH + 45] = 58;
    
    // Bones (69, 78)
    LEVEL2_DATA[12 * LEVEL2_WIDTH + 30] = 69;
    LEVEL2_DATA[12 * LEVEL2_WIDTH + 31] = 78;
    LEVEL2_DATA[16 * LEVEL2_WIDTH + 25] = 69;
    LEVEL2_DATA[16 * LEVEL2_WIDTH + 26] = 78;
    LEVEL2_DATA[34 * LEVEL2_WIDTH + 25] = 69;
    LEVEL2_DATA[34 * LEVEL2_WIDTH + 26] = 78;
    LEVEL2_DATA[38 * LEVEL2_WIDTH + 20] = 69;
    LEVEL2_DATA[38 * LEVEL2_WIDTH + 21] = 78;
    LEVEL2_DATA[25 * LEVEL2_WIDTH + 12] = 69;
    LEVEL2_DATA[25 * LEVEL2_WIDTH + 38] = 78;
    LEVEL2_DATA[20 * LEVEL2_WIDTH + 20] = 69;
    LEVEL2_DATA[20 * LEVEL2_WIDTH + 30] = 78;
    LEVEL2_DATA[30 * LEVEL2_WIDTH + 20] = 69;
    LEVEL2_DATA[30 * LEVEL2_WIDTH + 30] = 78;
    
    // Torches (91-96)
    LEVEL2_DATA[5 * LEVEL2_WIDTH + 10] = 91;
    LEVEL2_DATA[5 * LEVEL2_WIDTH + 40] = 92;
    LEVEL2_DATA[6 * LEVEL2_WIDTH + 12] = 91;
    LEVEL2_DATA[6 * LEVEL2_WIDTH + 38] = 92;
    LEVEL2_DATA[20 * LEVEL2_WIDTH + 5] = 93;
    LEVEL2_DATA[20 * LEVEL2_WIDTH + 45] = 94;
    LEVEL2_DATA[30 * LEVEL2_WIDTH + 5] = 93;
    LEVEL2_DATA[30 * LEVEL2_WIDTH + 45] = 94;
    LEVEL2_DATA[44 * LEVEL2_WIDTH + 12] = 95;
    LEVEL2_DATA[44 * LEVEL2_WIDTH + 38] = 96;
    LEVEL2_DATA[45 * LEVEL2_WIDTH + 10] = 95;
    LEVEL2_DATA[45 * LEVEL2_WIDTH + 40] = 96;
    
    // Magic potions (90, 98, 99)
    LEVEL2_DATA[20 * LEVEL2_WIDTH + 28] = 90;
    LEVEL2_DATA[22 * LEVEL2_WIDTH + 28] = 90;
    LEVEL2_DATA[28 * LEVEL2_WIDTH + 20] = 98;
    LEVEL2_DATA[28 * LEVEL2_WIDTH + 22] = 98;
    LEVEL2_DATA[28 * LEVEL2_WIDTH + 28] = 99;
    LEVEL2_DATA[28 * LEVEL2_WIDTH + 30] = 99;
    LEVEL2_DATA[30 * LEVEL2_WIDTH + 28] = 90;
    LEVEL2_DATA[35 * LEVEL2_WIDTH + 28] = 90;
    LEVEL2_DATA[35 * LEVEL2_WIDTH + 30] = 98;
    
    // Feather (65)
    LEVEL2_DATA[30 * LEVEL2_WIDTH + 15] = 65;
    LEVEL2_DATA[20 * LEVEL2_WIDTH + 35] = 65;
    LEVEL2_DATA[25 * LEVEL2_WIDTH + 18] = 65;
    LEVEL2_DATA[25 * LEVEL2_WIDTH + 32] = 65;
    LEVEL2_DATA[15 * LEVEL2_WIDTH + 25] = 65;
    LEVEL2_DATA[35 * LEVEL2_WIDTH + 25] = 65;
    
    // Flag (75)
    LEVEL2_DATA[7 * LEVEL2_WIDTH + 25] = 75;
    LEVEL2_DATA[8 * LEVEL2_WIDTH + 25] = 75;
    LEVEL2_DATA[42 * LEVEL2_WIDTH + 25] = 75;
    LEVEL2_DATA[43 * LEVEL2_WIDTH + 25] = 75;
    LEVEL2_DATA[25 * LEVEL2_WIDTH + 7] = 75;
    LEVEL2_DATA[25 * LEVEL2_WIDTH + 43] = 75;
    
    // Tables (81-86)
    LEVEL2_DATA[24 * LEVEL2_WIDTH + 15] = 81;
    LEVEL2_DATA[24 * LEVEL2_WIDTH + 35] = 82;
    LEVEL2_DATA[26 * LEVEL2_WIDTH + 15] = 81;
    LEVEL2_DATA[26 * LEVEL2_WIDTH + 35] = 82;
    LEVEL2_DATA[30 * LEVEL2_WIDTH + 15] = 83;
    LEVEL2_DATA[30 * LEVEL2_WIDTH + 35] = 84;
    LEVEL2_DATA[32 * LEVEL2_WIDTH + 15] = 83;
    LEVEL2_DATA[32 * LEVEL2_WIDTH + 35] = 84;
    LEVEL2_DATA[14 * LEVEL2_WIDTH + 25] = 85;
    LEVEL2_DATA[36 * LEVEL2_WIDTH + 25] = 86;
    LEVEL2_DATA[18 * LEVEL2_WIDTH + 20] = 85;
    LEVEL2_DATA[32 * LEVEL2_WIDTH + 30] = 86;
    
    for (int i = 0; i < LEVEL2_WIDTH * LEVEL2_HEIGHT; ++i)
        mLevelData[i] = LEVEL2_DATA[i];

    mGameState.bgm = LoadMusicStream("assets/game/looped_background_music.wav");
    SetMusicVolume(mGameState.bgm, 0.5f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/player_jump_sound.wav");

    mGameState.map = new Map(
        LEVEL2_WIDTH, LEVEL2_HEIGHT,
        (unsigned int *) mLevelData,
        "assets/game/Dungeon_Tileset.png",
        TILE_DIMENSION,
        10, 10,  
        mOrigin
    );

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
    
    // Get spawn position in tile coordinates
    int spawnTileX = static_cast<int>((mGameState.spawnPosition.x - mGameState.map->getLeftBoundary()) / TILE_DIMENSION);
    int spawnTileY = static_cast<int>((mGameState.spawnPosition.y - mGameState.map->getTopBoundary()) / TILE_DIMENSION);
    const int SPAWN_EXCLUSION_RADIUS = 3; // Keep enemies 3+ tiles away from spawn

    auto randomEmptyTile = [&]() {
        int gx = 0, gy = 0;
        for (int tries = 0; tries < 500; ++tries)
        {
            gx = 2 + (rand() % (LEVEL2_WIDTH - 3 - 2 + 1));
            gy = 2 + (rand() % (LEVEL2_HEIGHT - 3 - 2 + 1));
            if (LEVEL2_DATA[gy * LEVEL2_WIDTH + gx] == 0) {
                // Make sure it's far enough from spawn
                int dx = gx - spawnTileX;
                int dy = gy - spawnTileY;
                if (dx * dx + dy * dy > SPAWN_EXCLUSION_RADIUS * SPAWN_EXCLUSION_RADIUS) {
                    break; // Good spot, use it
                }
            }
        }
        return std::pair<int,int>(gx, gy);
    };

    // Setup chests with a random count
    mActiveChestCount = 3 + (rand() % (Level2::MAX_CHESTS - 3 + 1));
    mChestWithKeyIndex = rand() % mActiveChestCount;
    for (int i = 0; i < Level2::MAX_CHESTS; ++i)
    {
        mChestOpened[i] = false;
        auto tile = randomEmptyTile();
        float x = mGameState.map->getLeftBoundary() + tile.first * TILE_DIMENSION;
        float y = mGameState.map->getTopBoundary() + tile.second * TILE_DIMENSION;
        mChests[i] = new Entity({x, y}, {80.0f,80.0f}, "assets/game/chest.png", ATLAS, {1.0f,4.0f}, {{RIGHT,{0}}}, NPC);
        mChests[i]->setColliderDimensions({50.0f,40.0f});

        if (i >= mActiveChestCount)
        {
            mChests[i]->deactivate();
        }
    }
    mKeyEntity = new Entity({0,0}, {80.0f,80.0f}, "assets/game/keys.png", PLAYER);
    mKeyEntity->deactivate();

    // Skeleton followers
    std::map<Direction, std::vector<int>> skeletonAtlas = {
        {LEFT,  {0,1,2,3,4,5}},
        {RIGHT, {0,1,2,3,4,5}}
    };
    for (int i = 0; i < MAX_SKELETONS; ++i)
    {
        auto tile = randomEmptyTile();
        float x = mGameState.map->getLeftBoundary() + tile.first * TILE_DIMENSION;
        float y = mGameState.map->getTopBoundary() + tile.second * TILE_DIMENSION;
        mSkeletons[i] = new Entity(
            {x, y},
            {200.0f, 200.0f},
            "assets/game/skeleton_walk_right.png",
            NPC
        );
        mSkeletons[i]->setLeftRightTextures(
            "assets/game/skeleton_walk_left.png",
            "assets/game/skeleton_walk_right.png",
            ATLAS,
            {1.0f, 6.0f}
        );
        mSkeletons[i]->setAnimationAtlas(skeletonAtlas);
        mSkeletons[i]->setAIType(FOLLOWER);
        mSkeletons[i]->setAIState(WALKING);
        // Random speed for each skeleton
        mSkeletons[i]->setSpeed(30.0f + (rand() % 41));
        mSkeletons[i]->setColliderDimensions({50.0f,50.0f});
        mSkeletons[i]->setAcceleration({0.0f, 0.0f});
        mSkeletonHP[i] = 1;
    }

    mGameState.camera = { 0 };
    mGameState.camera.target = mGameState.xochitl->getPosition();
    mGameState.camera.offset = mOrigin;
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom = 1.0f;
}

void Level2::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);
    
    // Count down invincibility after taking damage
    if (mInvincibilityTimer > 0.0f)
    {
        mInvincibilityTimer -= deltaTime;
    }

    auto facingEnemy = [&](Vector2 enemyPos) {
        Vector2 p = mGameState.xochitl->getPosition();
        float dx = enemyPos.x - p.x;
        float dy = fabs(enemyPos.y - p.y);
        float lane = TILE_DIMENSION * 1.5f;
        switch (gFacing)
        {
        case LEFT:  return dx < 0 && dy < lane;
        case RIGHT: return dx > 0 && dy < lane;
        case UP:    return enemyPos.y < p.y && fabs(dx) < lane;
        case DOWN:  return enemyPos.y > p.y && fabs(dx) < lane;
        default:    return false;
        }
    };

    // Update player movement and collisions
    mGameState.xochitl->update(
        deltaTime,
        nullptr,
        mGameState.map,
        nullptr,
        0
    );
    // Clamp player inside map
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

    // Update skeletons, handle collisions and attacks
    bool tookDamageThisFrame = false;
    for (int i = 0; i < MAX_SKELETONS; ++i)
    {
        if (!mSkeletons[i]->isActive()) continue;

        mSkeletons[i]->setAIState(WALKING);
        mSkeletons[i]->setAIType(FOLLOWER);
        mSkeletons[i]->update(
            deltaTime,
            mGameState.xochitl,
            mGameState.map,
            nullptr,
            0
        );
        // Clamp skeleton inside map
        Vector2 sp = mSkeletons[i]->getPosition();
        float sminX = mGameState.map->getLeftBoundary() + 40.0f;
        float smaxX = mGameState.map->getRightBoundary() - 40.0f;
        float sminY = mGameState.map->getTopBoundary() + 40.0f;
        float smaxY = mGameState.map->getBottomBoundary() - 40.0f;
        sp.x = fmaxf(sminX, fminf(smaxX, sp.x));
        sp.y = fmaxf(sminY, fminf(smaxY, sp.y));
        mSkeletons[i]->setPosition(sp);

        // Take damage on touch (unless invincible or already took damage this frame)
        if (!tookDamageThisFrame && mInvincibilityTimer <= 0.0f && mGameState.xochitl->checkCollisionWithAI(mSkeletons[i]))
        {
            gLives--;
            PlaySound(gDeathSound);
            gPlayerAttacking = false;
            tookDamageThisFrame = true; // Avoid multiple hits in one frame
            if (gLives <= 0)
            {
                gGameLost = true;
                gCurrentLevel = 0;
                switchToScene(gMenuScene);
                return;
            }
            mGameState.xochitl->setPosition(mGameState.spawnPosition);
            mInvincibilityTimer = 1.0f; // Give 1 second of invincibility after respawn
        }

        // Apply attack damage once per attack
        if (gPlayerAttacking && gAttackTimer > 0.15f)
        {
            float dist = Vector2Distance(mGameState.xochitl->getPosition(), mSkeletons[i]->getPosition());
            if (dist < 90.0f && facingEnemy(mSkeletons[i]->getPosition()))
            {
                mSkeletonHP[i] -= 1;
                if (mSkeletonHP[i] <= 0) mSkeletons[i]->deactivate();
            }
        }
    }

    // Chest interaction
    for (int i = 0; i < mActiveChestCount; ++i)
    {
        if (!mChestOpened[i])
        {
            float dist = Vector2Distance(mGameState.xochitl->getPosition(), mChests[i]->getPosition());
            if (dist < 80.0f && IsKeyPressed(KEY_E))
            {
                mChestOpened[i] = true;
                // Fully open chest using right‑most frame
                mChests[i]->setAnimationAtlas({{RIGHT, {3}}});
                if (i == mChestWithKeyIndex)
                {
                    mKeyEntity->setPosition(mChests[i]->getPosition());
                    mKeyEntity->activate();
                    mKeyFound = true;
                }
            }
        }
    }

    if (mKeyEntity->isActive() && !mKeyCollected)
    {
        float d = Vector2Distance(mGameState.xochitl->getPosition(), mKeyEntity->getPosition());
        if (d < 60.0f)
        {
            mKeyCollected = true;
            mKeyCollectionTimer = 1.5f;
        }
    }

    if (mKeyCollected)
    {
        mKeyCollectionTimer -= deltaTime;
        if (mKeyCollectionTimer <= 0.0f)
        {
            mKeyEntity->deactivate();
            mGameState.nextSceneID = 3;
        }
    }

    // Tick down attack timer
    if (gPlayerAttacking)
    {
        gAttackTimer -= deltaTime;
        if (gAttackTimer <= 0.0f)
        {
            gPlayerAttacking = false;
        }
    }

    // Swap player animation based on facing/attack
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

    // Camera follows player (x and y)
    Vector2 currentPlayerPosition = { mGameState.xochitl->getPosition().x, mGameState.xochitl->getPosition().y };
    panCamera(&mGameState.camera, &currentPlayerPosition);
}

void Level2::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    mGameState.xochitl->render();
    for (int i = 0; i < MAX_SKELETONS; ++i)
        if (mSkeletons[i]->isActive()) mSkeletons[i]->render();
    for (int i = 0; i < mActiveChestCount; ++i)
        mChests[i]->render();
    if (mKeyEntity && mKeyEntity->isActive()) mKeyEntity->render();
}

void Level2::shutdown()
{
    delete mGameState.xochitl;
    delete mGameState.map;
    for (int i = 0; i < MAX_SKELETONS; ++i) delete mSkeletons[i];
    for (int i = 0; i < MAX_CHESTS; ++i) delete mChests[i];
    delete mKeyEntity;

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
}

