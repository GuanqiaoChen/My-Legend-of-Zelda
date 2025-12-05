#include "ShaderProgram.h"

ShaderProgram::ShaderProgram() : mIsLoaded(false)
{
    mShader.id = NOT_LOADED;
}

ShaderProgram::~ShaderProgram()
{
    unload();
}

bool ShaderProgram::load(const std::string &vertexPath, const std::string &fragmentPath)
{
    mShader = LoadShader(vertexPath.c_str(), fragmentPath.c_str());
    mIsLoaded = mShader.id != 0;
    return mIsLoaded;
}

void ShaderProgram::unload()
{
    if (mIsLoaded)
    {
        UnloadShader(mShader);
        mIsLoaded = false;
    }
}

void ShaderProgram::begin()
{
    if (mIsLoaded) BeginShaderMode(mShader);
}

void ShaderProgram::end()
{
    if (mIsLoaded) EndShaderMode();
}

void ShaderProgram::setVector2(const std::string &name, const Vector2 &value)
{
    if (!mIsLoaded) return;
    int loc = GetShaderLocation(mShader, name.c_str());
    SetShaderValue(mShader, loc, &value, SHADER_UNIFORM_VEC2);
}

void ShaderProgram::setFloat(const std::string &name, float value)
{
    if (!mIsLoaded) return;
    int loc = GetShaderLocation(mShader, name.c_str());
    SetShaderValue(mShader, loc, &value, SHADER_UNIFORM_FLOAT);
}

void ShaderProgram::setInt(const std::string &name, int value)
{
    if (!mIsLoaded) return;
    int loc = GetShaderLocation(mShader, name.c_str());
    SetShaderValue(mShader, loc, &value, SHADER_UNIFORM_INT);
}

