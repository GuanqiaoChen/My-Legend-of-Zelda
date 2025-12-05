#include "Entity.h"

Entity::Entity() : mPosition {0.0f, 0.0f}, mMovement {0.0f, 0.0f}, 
                   mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f},
                   mScale {DEFAULT_SIZE, DEFAULT_SIZE},
                   mColliderDimensions {DEFAULT_SIZE, DEFAULT_SIZE}, 
                   mTexture {0}, mTextureLeft {0}, mTextureRight {0},
                   mHasSeparateTextures {false}, mTextureType {SINGLE}, mAngle {0.0f},
                   mSpriteSheetDimensions {}, mDirection {RIGHT}, 
                   mAnimationAtlas {{}}, mAnimationIndices {}, mFrameSpeed {DEFAULT_FRAME_SPEED},
                   mEntityType {NONE} { }

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
    EntityType entityType) : mPosition {position}, mVelocity {0.0f, 0.0f}, 
    mAcceleration {0.0f, 0.0f}, mScale {scale}, mMovement {0.0f, 0.0f}, 
    mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)}, 
    mTextureLeft {0}, mTextureRight {0}, mHasSeparateTextures {false},
    mTextureType {SINGLE}, mDirection {RIGHT}, mAnimationAtlas {{}}, 
    mAnimationIndices {}, mFrameSpeed {DEFAULT_FRAME_SPEED}, mSpeed {DEFAULT_SPEED}, 
    mAngle {0.0f}, mEntityType {entityType} { }

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
        TextureType textureType, Vector2 spriteSheetDimensions, std::map<Direction, 
        std::vector<int>> animationAtlas, EntityType entityType) : 
        mPosition {position}, mVelocity {0.0f, 0.0f}, 
        mAcceleration {0.0f, 0.0f}, mMovement { 0.0f, 0.0f }, mScale {scale},
        mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)}, 
        mTextureLeft {0}, mTextureRight {0}, mHasSeparateTextures {false},
        mTextureType {ATLAS}, mSpriteSheetDimensions {spriteSheetDimensions},
        mAnimationAtlas {animationAtlas}, mDirection {RIGHT},
        mAnimationIndices {animationAtlas.at(RIGHT)}, 
        mFrameSpeed {DEFAULT_FRAME_SPEED}, mAngle { 0.0f }, 
        mSpeed { DEFAULT_SPEED }, mEntityType {entityType} { }

Entity::~Entity() 
{ 
    UnloadTexture(mTexture); 
    if (mHasSeparateTextures)
    {
        UnloadTexture(mTextureLeft);
        UnloadTexture(mTextureRight);
    }
};

void Entity::checkCollisionY(Entity *collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = &collidableEntities[i];
        
        if (isColliding(collidableEntity))
        {
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(yDistance - (mColliderDimensions.y / 2.0f) - 
                              (collidableEntity->mColliderDimensions.y / 2.0f));

            if (mVelocity.y > 0) 
            {
                mPosition.y -= yOverlap;
                mVelocity.y  = 0;
                mIsCollidingBottom = true;
            } else if (mVelocity.y < 0) 
            {
                mPosition.y += yOverlap;
                mVelocity.y  = 0;
                mIsCollidingTop = true;

                if (collidableEntity->mEntityType == BLOCK)
                    collidableEntity->deactivate();
            }
        }
    }
}

void Entity::checkCollisionX(Entity *collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = &collidableEntities[i];
        
        if (isColliding(collidableEntity))
        {            
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(yDistance - (mColliderDimensions.y / 2.0f) - (collidableEntity->mColliderDimensions.y / 2.0f));

            if (yOverlap < Y_COLLISION_THRESHOLD) continue;

            float xDistance = fabs(mPosition.x - collidableEntity->mPosition.x);
            float xOverlap  = fabs(xDistance - (mColliderDimensions.x / 2.0f) - (collidableEntity->mColliderDimensions.x / 2.0f));

            if (mVelocity.x > 0) {
                mPosition.x     -= xOverlap;
                mVelocity.x      = 0;

                mIsCollidingRight = true;
            } else if (mVelocity.x < 0) {
                mPosition.x    += xOverlap;
                mVelocity.x     = 0;
 
                mIsCollidingLeft = true;
            }
        }
    }
}

void Entity::checkCollisionY(Map *map)
{
    if (map == nullptr) return;

    Vector2 topCentreProbe    = { mPosition.x, mPosition.y - (mColliderDimensions.y / 2.0f) };
    Vector2 topLeftProbe      = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y - (mColliderDimensions.y / 2.0f) };
    Vector2 topRightProbe     = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y - (mColliderDimensions.y / 2.0f) };

    Vector2 bottomCentreProbe = { mPosition.x, mPosition.y + (mColliderDimensions.y / 2.0f) };
    Vector2 bottomLeftProbe   = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y + (mColliderDimensions.y / 2.0f) };
    Vector2 bottomRightProbe  = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y + (mColliderDimensions.y / 2.0f) };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    if ((map->isSolidTileAt(topCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(topLeftProbe, &xOverlap, &yOverlap)   ||
         map->isSolidTileAt(topRightProbe, &xOverlap, &yOverlap)) && mVelocity.y < 0.0f)
    {
        mPosition.y += yOverlap;   
        mVelocity.y  = 0.0f;
        mIsCollidingTop = true;
    }

    if ((map->isSolidTileAt(bottomCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(bottomLeftProbe, &xOverlap, &yOverlap)   ||
         map->isSolidTileAt(bottomRightProbe, &xOverlap, &yOverlap)) && mVelocity.y > 0.0f)
    {
        mPosition.y -= yOverlap;   
        mVelocity.y  = 0.0f;
        mIsCollidingBottom = true;
    } 
}

void Entity::checkCollisionX(Map *map)
{
    if (map == nullptr) return;

    Vector2 leftCentreProbe   = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y };

    Vector2 rightCentreProbe  = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    if (map->isSolidTileAt(rightCentreProbe, &xOverlap, &yOverlap) 
         && mVelocity.x > 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x -= xOverlap * 1.01f;  
        mVelocity.x  = 0.0f;
        mIsCollidingRight = true;
    }

    if (map->isSolidTileAt(leftCentreProbe, &xOverlap, &yOverlap) 
         && mVelocity.x < 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x += xOverlap * 1.01;   
        mVelocity.x  = 0.0f;
        mIsCollidingLeft = true;
    }
}

bool Entity::isColliding(Entity *other) const 
{
    if (!other->isActive() || other == this) return false;

    float xDistance = fabs(mPosition.x - other->getPosition().x) - 
        ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->getPosition().y) - 
        ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

bool Entity::checkCollisionWithAI(Entity *ai) const
{
    if (mEntityType != PLAYER || ai->getEntityType() != NPC) return false;
    return isColliding(ai);
}

void Entity::setAnimationAtlas(std::map<Direction, std::vector<int>> animationAtlas)
{
    mAnimationAtlas = animationAtlas;
    // Initialize animation indices based on current direction
    if (mAnimationAtlas.find(mDirection) != mAnimationAtlas.end())
    {
        mAnimationIndices = mAnimationAtlas.at(mDirection);
        mCurrentFrameIndex = 0;
    }
}

void Entity::animate(float deltaTime)
{
    // Skip normal animation if we're forcing it elsewhere
    if (mForceAnimation) {
        return;
    }
    
    if (mAnimationAtlas.find(mDirection) != mAnimationAtlas.end())
    {
        mAnimationIndices = mAnimationAtlas.at(mDirection);
    }

    const float EPS = 1e-2f;
    bool movingHoriz = fabsf(mVelocity.x) > EPS;
    bool movingVert  = fabsf(mVelocity.y) > EPS;

    bool shouldAnimate = movingHoriz || (mAIType == FLYER && movingVert);

    if (!shouldAnimate) {
        // Idle frame
        mCurrentFrameIndex = 0;
        mAnimationTime = 0.0f;
        return;
    }

    mAnimationTime += deltaTime;
    float framesPerSecond = 1.0f / mFrameSpeed;

    if (mAnimationTime >= framesPerSecond)
    {
        mAnimationTime = 0.0f;

        mCurrentFrameIndex++;
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}

void Entity::forceAnimate(float deltaTime)
{
    mForceAnimation = true; // Skip normal animation
    
    if (mAnimationAtlas.find(mDirection) != mAnimationAtlas.end())
    {
        mAnimationIndices = mAnimationAtlas.at(mDirection);
    }
    
    if (mAnimationIndices.empty()) {
        mForceAnimation = false;
        return;
    }

    mAnimationTime += deltaTime;
    float framesPerSecond = 1.0f / mFrameSpeed;

    if (mAnimationTime >= framesPerSecond)
    {
        mAnimationTime = 0.0f;

        mCurrentFrameIndex++;
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}

void Entity::AIWander() 
{ 
    moveLeft();
}

void Entity::AIFollow(Entity *target)
{
    switch (mAIState)
    {
    case IDLE:
        if (Vector2Distance(mPosition, target->getPosition()) < 250.0f) 
            mAIState = WALKING;
        break;

    case WALKING:
        if (mPosition.x > target->getPosition().x) moveLeft();
        else                                       moveRight();
        if (mPosition.y > target->getPosition().y) moveUp();
        else                                       moveDown();
    
    default:
        break;
    }
}

void Entity::AIFly(Entity *target)
{
    if (mPosition.x > target->getPosition().x) moveLeft();
    else moveRight();
    
    if (mPosition.y > target->getPosition().y) moveUp();
    else moveDown();
}

void Entity::AIActivate(Entity *target)
{
    switch (mAIType)
    {
    case WANDERER:
        AIWander();
        break;

    case FOLLOWER:
        AIFollow(target);
        break;
    
    case FLYER:
        AIFly(target);
        break;
    
    default:
        break;
    }
}

void Entity::update(float deltaTime, Entity *player, Map *map, 
    Entity *collidableEntities, int collisionCheckCount)
{
    if (mEntityStatus == INACTIVE) return;
    
    if (mEntityType == NPC) AIActivate(player);

    resetColliderFlags();

    // Movement on both axes for top-down gameplay
    // Non-flying entities still use gravity below
    mVelocity.x = mMovement.x * mSpeed;
    mVelocity.y = mMovement.y * mSpeed;

    // Apply gravity to non-flying entities
    if (mAIType != FLYER)
    {
        mVelocity.x += mAcceleration.x * deltaTime;
        mVelocity.y += mAcceleration.y * deltaTime;
    }

    if (mIsJumping)
    {
        mIsJumping = false;
        
        mVelocity.y -= mJumpingPower;
    }

    mPosition.y += mVelocity.y * deltaTime;
    if (mAIType != FLYER)
    {
        checkCollisionY(collidableEntities, collisionCheckCount);
        checkCollisionY(map);
    }

    mPosition.x += mVelocity.x * deltaTime;
    if (mAIType != FLYER)
    {
        checkCollisionX(collidableEntities, collisionCheckCount);
        checkCollisionX(map);
    }

    if (mTextureType == ATLAS && !mAnimationIndices.empty()) 
    {
        if (mAIType == FLYER || mIsCollidingBottom || GetLength(mMovement) != 0 || mEntityType == PLAYER)
            animate(deltaTime);
    }
}

void Entity::render()
{
    if(mEntityStatus == INACTIVE) return;

    Texture2D textureToUse = mTexture;
    
    if (mHasSeparateTextures)
    {
        if (mDirection == LEFT)
            textureToUse = mTextureLeft;
        else
            textureToUse = mTextureRight;
    }

    if (textureToUse.id == 0) return;

    Rectangle textureArea;

    switch (mTextureType)
    {
        case SINGLE:
            textureArea = {
                // top-left corner
                0.0f, 0.0f,

                // bottom-right corner
                static_cast<float>(textureToUse.width),
                static_cast<float>(textureToUse.height)
            };
            break;
        case ATLAS:
            // Make sure we have valid animation frames
            if (mAnimationIndices.empty() || mCurrentFrameIndex >= mAnimationIndices.size())
            {
                textureArea = getUVRectangle(
                    &textureToUse, 
                    0, 
                    mSpriteSheetDimensions.x, 
                    mSpriteSheetDimensions.y
                );
            }
            else
            {
                textureArea = getUVRectangle(
                    &textureToUse, 
                    mAnimationIndices[mCurrentFrameIndex], 
                    mSpriteSheetDimensions.x, 
                    mSpriteSheetDimensions.y
                );
            }
            break;
        
        default: break;
    }

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        mPosition.x,
        mPosition.y,
        static_cast<float>(mScale.x),
        static_cast<float>(mScale.y)
    };

    // Texture origin at center
    Vector2 originOffset = {
        static_cast<float>(mScale.x) / 2.0f,
        static_cast<float>(mScale.y) / 2.0f
    };

    // Draw the sprite
    DrawTexturePro(
        textureToUse, 
        textureArea, destinationArea, originOffset,
        mAngle, WHITE
    );

    // displayCollider();
}

void Entity::displayCollider() 
{
    // Draw collision box for debugging
    Rectangle colliderBox = {
        mPosition.x - mColliderDimensions.x / 2.0f,  
        mPosition.y - mColliderDimensions.y / 2.0f,  
        mColliderDimensions.x,                        
        mColliderDimensions.y                        
    };

    DrawRectangleLines(
        colliderBox.x,      // Top-left X
        colliderBox.y,      // Top-left Y
        colliderBox.width,  // Width
        colliderBox.height, // Height
        GREEN               // Color
    );
}