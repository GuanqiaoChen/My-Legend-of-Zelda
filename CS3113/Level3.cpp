#include "Level3.h"
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

Level3::Level3() : Scene { {0.0f}, nullptr } {}
Level3::Level3(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

Level3::~Level3() { shutdown(); }

static unsigned int LEVEL3_DATA[LEVEL3_WIDTH * LEVEL3_HEIGHT];

void Level3::initialise()
{
    mGameState.nextSceneID = 0;
    mChestOpened = false;
    mKeyCollected = false;
    mKeyCollectionTimer = 0.0f;
    mLastFacing = DOWN;
    mLastAttack = false;
    mInvincibilityTimer = 1.0f; // Start with 1 second invincibility

    // Build Level 3 index matrix (open layout, no floor/room walls)
    // Initialize all tiles to 0 (empty)
    for (int r = 0; r < LEVEL3_HEIGHT; ++r)
    {
        for (int c = 0; c < LEVEL3_WIDTH; ++c)
            LEVEL3_DATA[r * LEVEL3_WIDTH + c] = 0;
    }
    
    // Corners and outer boundary
    LEVEL3_DATA[0] = 1;
    LEVEL3_DATA[LEVEL3_WIDTH - 1] = 6;
    LEVEL3_DATA[(LEVEL3_HEIGHT - 1) * LEVEL3_WIDTH] = 41;
    LEVEL3_DATA[(LEVEL3_HEIGHT - 1) * LEVEL3_WIDTH + (LEVEL3_WIDTH - 1)] = 46;
    // Top wall
    for (int c = 1; c < LEVEL3_WIDTH - 1; ++c) LEVEL3_DATA[c] = 2;
    // Bottom wall
    for (int c = 1; c < LEVEL3_WIDTH - 1; ++c) LEVEL3_DATA[(LEVEL3_HEIGHT - 1) * LEVEL3_WIDTH + c] = 42;
    // Left/right walls
    for (int r = 1; r < LEVEL3_HEIGHT - 1; ++r)
    {
        LEVEL3_DATA[r * LEVEL3_WIDTH] = 11;
        LEVEL3_DATA[r * LEVEL3_WIDTH + (LEVEL3_WIDTH - 1)] = 16;
    }
    
    // Doors on outer walls for visual entry/exit
    int midRow = LEVEL3_HEIGHT / 2;
    LEVEL3_DATA[midRow * LEVEL3_WIDTH] = 37;
    LEVEL3_DATA[midRow * LEVEL3_WIDTH + (LEVEL3_WIDTH - 1)] = 38;
    
    // Internal walls (partial, don't create separate rooms)
    // Horizontal partial walls
    for (int c = 6; c < 15; ++c) LEVEL3_DATA[10 * LEVEL3_WIDTH + c] = 2;
    for (int c = 8; c < 18; ++c) LEVEL3_DATA[12 * LEVEL3_WIDTH + c] = 2;
    for (int c = 35; c < 44; ++c) LEVEL3_DATA[10 * LEVEL3_WIDTH + c] = 2;
    for (int c = 32; c < 42; ++c) LEVEL3_DATA[12 * LEVEL3_WIDTH + c] = 2;
    for (int c = 12; c < 22; ++c) LEVEL3_DATA[16 * LEVEL3_WIDTH + c] = 2;
    for (int c = 28; c < 38; ++c) LEVEL3_DATA[16 * LEVEL3_WIDTH + c] = 2;
    for (int c = 15; c < 25; ++c) LEVEL3_DATA[38 * LEVEL3_WIDTH + c] = 42;
    for (int c = 25; c < 35; ++c) LEVEL3_DATA[38 * LEVEL3_WIDTH + c] = 42;
    for (int c = 12; c < 22; ++c) LEVEL3_DATA[34 * LEVEL3_WIDTH + c] = 42;
    for (int c = 28; c < 38; ++c) LEVEL3_DATA[34 * LEVEL3_WIDTH + c] = 42;
    for (int c = 6; c < 15; ++c) LEVEL3_DATA[40 * LEVEL3_WIDTH + c] = 42;
    for (int c = 35; c < 44; ++c) LEVEL3_DATA[40 * LEVEL3_WIDTH + c] = 42;
    // Vertical partial walls
    for (int r = 6; r < 15; ++r) LEVEL3_DATA[r * LEVEL3_WIDTH + 10] = 11;
    for (int r = 8; r < 18; ++r) LEVEL3_DATA[r * LEVEL3_WIDTH + 12] = 11;
    for (int r = 35; r < 44; ++r) LEVEL3_DATA[r * LEVEL3_WIDTH + 10] = 11;
    for (int r = 32; r < 42; ++r) LEVEL3_DATA[r * LEVEL3_WIDTH + 12] = 11;
    for (int r = 12; r < 22; ++r) LEVEL3_DATA[r * LEVEL3_WIDTH + 16] = 11;
    for (int r = 28; r < 38; ++r) LEVEL3_DATA[r * LEVEL3_WIDTH + 16] = 11;
    for (int r = 15; r < 25; ++r) LEVEL3_DATA[r * LEVEL3_WIDTH + 38] = 16;
    for (int r = 25; r < 35; ++r) LEVEL3_DATA[r * LEVEL3_WIDTH + 38] = 16;
    for (int r = 12; r < 22; ++r) LEVEL3_DATA[r * LEVEL3_WIDTH + 34] = 16;
    for (int r = 28; r < 38; ++r) LEVEL3_DATA[r * LEVEL3_WIDTH + 34] = 16;
    for (int r = 6; r < 15; ++r) LEVEL3_DATA[r * LEVEL3_WIDTH + 40] = 16;
    for (int r = 35; r < 44; ++r) LEVEL3_DATA[r * LEVEL3_WIDTH + 40] = 16;
    
    // Squared doors (67, 68)
    LEVEL3_DATA[8 * LEVEL3_WIDTH + 12] = 67;
    LEVEL3_DATA[8 * LEVEL3_WIDTH + 13] = 68;
    LEVEL3_DATA[10 * LEVEL3_WIDTH + 12] = 67;
    LEVEL3_DATA[10 * LEVEL3_WIDTH + 13] = 68;
    LEVEL3_DATA[14 * LEVEL3_WIDTH + 16] = 67;
    LEVEL3_DATA[14 * LEVEL3_WIDTH + 17] = 68;
    LEVEL3_DATA[36 * LEVEL3_WIDTH + 34] = 67;
    LEVEL3_DATA[36 * LEVEL3_WIDTH + 35] = 68;
    LEVEL3_DATA[40 * LEVEL3_WIDTH + 38] = 67;
    LEVEL3_DATA[40 * LEVEL3_WIDTH + 39] = 68;
    LEVEL3_DATA[25 * LEVEL3_WIDTH + 12] = 67;
    LEVEL3_DATA[25 * LEVEL3_WIDTH + 13] = 68;
    LEVEL3_DATA[25 * LEVEL3_WIDTH + 38] = 67;
    LEVEL3_DATA[25 * LEVEL3_WIDTH + 39] = 68;
    
    // Poles (47, 48, 57, 58)
    LEVEL3_DATA[4 * LEVEL3_WIDTH + 6] = 47;
    LEVEL3_DATA[4 * LEVEL3_WIDTH + 44] = 48;
    LEVEL3_DATA[6 * LEVEL3_WIDTH + 8] = 47;
    LEVEL3_DATA[6 * LEVEL3_WIDTH + 42] = 48;
    LEVEL3_DATA[44 * LEVEL3_WIDTH + 8] = 57;
    LEVEL3_DATA[44 * LEVEL3_WIDTH + 42] = 58;
    LEVEL3_DATA[46 * LEVEL3_WIDTH + 6] = 57;
    LEVEL3_DATA[46 * LEVEL3_WIDTH + 44] = 58;
    LEVEL3_DATA[12 * LEVEL3_WIDTH + 3] = 47;
    LEVEL3_DATA[12 * LEVEL3_WIDTH + 47] = 48;
    LEVEL3_DATA[38 * LEVEL3_WIDTH + 3] = 57;
    LEVEL3_DATA[38 * LEVEL3_WIDTH + 47] = 58;
    LEVEL3_DATA[8 * LEVEL3_WIDTH + 20] = 47;
    LEVEL3_DATA[8 * LEVEL3_WIDTH + 30] = 48;
    LEVEL3_DATA[42 * LEVEL3_WIDTH + 20] = 57;
    LEVEL3_DATA[42 * LEVEL3_WIDTH + 30] = 58;
    
    // Bones (69, 78)
    LEVEL3_DATA[11 * LEVEL3_WIDTH + 20] = 69;
    LEVEL3_DATA[11 * LEVEL3_WIDTH + 30] = 78;
    LEVEL3_DATA[14 * LEVEL3_WIDTH + 22] = 69;
    LEVEL3_DATA[14 * LEVEL3_WIDTH + 28] = 78;
    LEVEL3_DATA[18 * LEVEL3_WIDTH + 18] = 69;
    LEVEL3_DATA[18 * LEVEL3_WIDTH + 32] = 78;
    LEVEL3_DATA[32 * LEVEL3_WIDTH + 18] = 69;
    LEVEL3_DATA[32 * LEVEL3_WIDTH + 32] = 78;
    LEVEL3_DATA[36 * LEVEL3_WIDTH + 22] = 69;
    LEVEL3_DATA[36 * LEVEL3_WIDTH + 28] = 78;
    LEVEL3_DATA[39 * LEVEL3_WIDTH + 20] = 69;
    LEVEL3_DATA[39 * LEVEL3_WIDTH + 30] = 78;
    LEVEL3_DATA[25 * LEVEL3_WIDTH + 14] = 69;
    LEVEL3_DATA[25 * LEVEL3_WIDTH + 36] = 78;
    LEVEL3_DATA[25 * LEVEL3_WIDTH + 18] = 69;
    LEVEL3_DATA[25 * LEVEL3_WIDTH + 32] = 78;
    
    // Torches (91-96)
    LEVEL3_DATA[3 * LEVEL3_WIDTH + 12] = 91;
    LEVEL3_DATA[3 * LEVEL3_WIDTH + 38] = 92;
    LEVEL3_DATA[4 * LEVEL3_WIDTH + 15] = 91;
    LEVEL3_DATA[4 * LEVEL3_WIDTH + 35] = 92;
    LEVEL3_DATA[16 * LEVEL3_WIDTH + 3] = 93;
    LEVEL3_DATA[16 * LEVEL3_WIDTH + 47] = 94;
    LEVEL3_DATA[18 * LEVEL3_WIDTH + 4] = 93;
    LEVEL3_DATA[18 * LEVEL3_WIDTH + 46] = 94;
    LEVEL3_DATA[34 * LEVEL3_WIDTH + 3] = 93;
    LEVEL3_DATA[34 * LEVEL3_WIDTH + 47] = 94;
    LEVEL3_DATA[46 * LEVEL3_WIDTH + 15] = 95;
    LEVEL3_DATA[46 * LEVEL3_WIDTH + 35] = 96;
    LEVEL3_DATA[47 * LEVEL3_WIDTH + 12] = 95;
    LEVEL3_DATA[47 * LEVEL3_WIDTH + 38] = 96;
    
    // Magic potions (90, 98, 99)
    LEVEL3_DATA[14 * LEVEL3_WIDTH + 25] = 90;
    LEVEL3_DATA[16 * LEVEL3_WIDTH + 25] = 90;
    LEVEL3_DATA[25 * LEVEL3_WIDTH + 14] = 98;
    LEVEL3_DATA[25 * LEVEL3_WIDTH + 16] = 98;
    LEVEL3_DATA[25 * LEVEL3_WIDTH + 34] = 99;
    LEVEL3_DATA[25 * LEVEL3_WIDTH + 36] = 99;
    LEVEL3_DATA[34 * LEVEL3_WIDTH + 25] = 90;
    LEVEL3_DATA[36 * LEVEL3_WIDTH + 25] = 90;
    LEVEL3_DATA[18 * LEVEL3_WIDTH + 30] = 98;
    LEVEL3_DATA[20 * LEVEL3_WIDTH + 30] = 98;
    LEVEL3_DATA[30 * LEVEL3_WIDTH + 20] = 99;
    LEVEL3_DATA[32 * LEVEL3_WIDTH + 20] = 99;
    
    // Feather (65)
    LEVEL3_DATA[22 * LEVEL3_WIDTH + 20] = 65;
    LEVEL3_DATA[28 * LEVEL3_WIDTH + 20] = 65;
    LEVEL3_DATA[22 * LEVEL3_WIDTH + 30] = 65;
    LEVEL3_DATA[28 * LEVEL3_WIDTH + 30] = 65;
    LEVEL3_DATA[30 * LEVEL3_WIDTH + 30] = 65;
    LEVEL3_DATA[20 * LEVEL3_WIDTH + 25] = 65;
    LEVEL3_DATA[30 * LEVEL3_WIDTH + 25] = 65;
    
    // Flag (75)
    LEVEL3_DATA[2 * LEVEL3_WIDTH + 25] = 75;
    LEVEL3_DATA[3 * LEVEL3_WIDTH + 25] = 75;
    LEVEL3_DATA[47 * LEVEL3_WIDTH + 25] = 75;
    LEVEL3_DATA[48 * LEVEL3_WIDTH + 25] = 75;
    LEVEL3_DATA[25 * LEVEL3_WIDTH + 2] = 75;
    LEVEL3_DATA[25 * LEVEL3_WIDTH + 48] = 75;
    
    // Tables (81-86)
    LEVEL3_DATA[18 * LEVEL3_WIDTH + 18] = 81;
    LEVEL3_DATA[18 * LEVEL3_WIDTH + 32] = 82;
    LEVEL3_DATA[20 * LEVEL3_WIDTH + 18] = 81;
    LEVEL3_DATA[20 * LEVEL3_WIDTH + 32] = 82;
    LEVEL3_DATA[30 * LEVEL3_WIDTH + 18] = 83;
    LEVEL3_DATA[30 * LEVEL3_WIDTH + 32] = 84;
    LEVEL3_DATA[32 * LEVEL3_WIDTH + 18] = 83;
    LEVEL3_DATA[32 * LEVEL3_WIDTH + 32] = 84;
    LEVEL3_DATA[13 * LEVEL3_WIDTH + 25] = 85;
    LEVEL3_DATA[15 * LEVEL3_WIDTH + 25] = 85;
    LEVEL3_DATA[35 * LEVEL3_WIDTH + 25] = 86;
    LEVEL3_DATA[37 * LEVEL3_WIDTH + 25] = 86;
    
    for (int i = 0; i < LEVEL3_WIDTH * LEVEL3_HEIGHT; ++i)
        mLevelData[i] = LEVEL3_DATA[i];

    mGameState.bgm = LoadMusicStream("assets/game/looped_background_music.wav");
    SetMusicVolume(mGameState.bgm, 0.5f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/player_jump_sound.wav");

    mGameState.map = new Map(
        LEVEL3_WIDTH, LEVEL3_HEIGHT,
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
            gx = 2 + (rand() % (LEVEL3_WIDTH - 3 - 2 + 1));
            gy = 2 + (rand() % (LEVEL3_HEIGHT - 3 - 2 + 1));
            if (LEVEL3_DATA[gy * LEVEL3_WIDTH + gx] == 0) {
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

    // Demons (followers, 2 HP, flying)
    std::map<Direction, std::vector<int>> demonAtlas = {
        {LEFT,  {0,1,2,3}},
        {RIGHT, {0,1,2,3}}
    };
    for (int i = 0; i < MAX_DEMONS; ++i)
    {
        auto tile = randomEmptyTile();
        float x = mGameState.map->getLeftBoundary() + tile.first * TILE_DIMENSION;
        float y = mGameState.map->getTopBoundary() + tile.second * TILE_DIMENSION;
        mDemons[i] = new Entity(
            {x, y},
            {90.0f, 90.0f},
        "assets/game/demon_fly_right.png",
        NPC
    );
        mDemons[i]->setLeftRightTextures(
        "assets/game/demon_fly_left.png",
        "assets/game/demon_fly_right.png",
        ATLAS,
        {1.0f, 4.0f}  
    );
        mDemons[i]->setAnimationAtlas(demonAtlas);
        mDemons[i]->setAIType(FLYER);
        mDemons[i]->setAIState(WALKING);
        // Random speed for each demon
        mDemons[i]->setSpeed(20.0f + (rand() % 41));
        mDemons[i]->setColliderDimensions({50.0f,50.0f});
        mDemons[i]->setAcceleration({0.0f, 0.0f});
        mDemonHP[i] = 1;
    }

    // Chest/key for win
    auto chestTile = randomEmptyTile();
    float chestX = mGameState.map->getLeftBoundary() + chestTile.first * TILE_DIMENSION;
    float chestY = mGameState.map->getTopBoundary() + chestTile.second * TILE_DIMENSION;
    mChest = new Entity({chestX, chestY}, {80.0f,80.0f}, "assets/game/chest.png", ATLAS, {1.0f,4.0f}, {{RIGHT,{0}}}, NPC);
    mChest->setColliderDimensions({50.0f,40.0f});

    mKeyEntity = new Entity({0,0}, {80.0f,80.0f}, "assets/game/keys.png", PLAYER);
    mKeyEntity->deactivate();

    mGameState.camera = { 0 };
    mGameState.camera.target = mGameState.xochitl->getPosition();
    mGameState.camera.offset = mOrigin;
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom = 1.0f;
}

void Level3::update(float deltaTime)
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

    // Update demons, handle collisions and attacks
    bool tookDamageThisFrame = false;
    for (int i = 0; i < MAX_DEMONS; ++i)
    {
        if (!mDemons[i]->isActive()) continue;

        mDemons[i]->setAIState(WALKING);
        mDemons[i]->setAIType(FLYER);
        mDemons[i]->update(
            deltaTime,
            mGameState.xochitl,
            mGameState.map,
            nullptr,
            0
        );
        // Clamp demon inside map
        Vector2 dp = mDemons[i]->getPosition();
        float dminX = mGameState.map->getLeftBoundary() + 40.0f;
        float dmaxX = mGameState.map->getRightBoundary() - 40.0f;
        float dminY = mGameState.map->getTopBoundary() + 40.0f;
        float dmaxY = mGameState.map->getBottomBoundary() - 40.0f;
        dp.x = fmaxf(dminX, fminf(dmaxX, dp.x));
        dp.y = fmaxf(dminY, fminf(dmaxY, dp.y));
        mDemons[i]->setPosition(dp);

        // Take damage on touch (unless invincible or already took damage this frame)
        if (!tookDamageThisFrame && mInvincibilityTimer <= 0.0f && mGameState.xochitl->checkCollisionWithAI(mDemons[i]))
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
            float dist = Vector2Distance(mGameState.xochitl->getPosition(), mDemons[i]->getPosition());
            if (dist < 90.0f && facingEnemy(mDemons[i]->getPosition()))
            {
                mDemonHP[i] -= 1;
                if (mDemonHP[i] <= 0) mDemons[i]->deactivate();
            }
        }
    }

    // Chest/key for win
    if (!mChestOpened)
    {
        float dist = Vector2Distance(mGameState.xochitl->getPosition(), mChest->getPosition());
        if (dist < 80.0f && IsKeyPressed(KEY_E))
        {
            mChestOpened = true;
            mChest->setAnimationAtlas({{RIGHT,{3}}});
            mKeyEntity->setPosition(mChest->getPosition());
            mKeyEntity->activate();
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
            mGameState.nextSceneID = -2; // win
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

void Level3::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    mGameState.xochitl->render();
    for (int i = 0; i < MAX_DEMONS; ++i)
        if (mDemons[i]->isActive()) mDemons[i]->render();
    mChest->render();
    if (mKeyEntity && mKeyEntity->isActive()) mKeyEntity->render();
}

void Level3::shutdown()
{
    delete mGameState.xochitl;
    delete mGameState.map;
    for (int i = 0; i < MAX_DEMONS; ++i) delete mDemons[i];
    delete mChest;
    delete mKeyEntity;

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
}

