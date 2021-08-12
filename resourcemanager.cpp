#include "resourcemanager.h"

#include <utility>

ResourceManager::ResourceManager()
{}

bool ResourceManager::searchMap(std::string &name, GLuint &VAO, int &indicesCount)
{

    auto result = mObjFiles.find(name);
    if (result != mObjFiles.end()) {
        VAO = result->second;
        indicesCount = mIndicesCount.find(VAO)->second;
        return true;
    }
    else
        return false;
}

void ResourceManager::insertInMap(std::string &name, GLuint &VAO, int &indicesCount)
{
    mObjFiles.emplace(name, VAO);
    mIndicesCount.emplace(VAO, indicesCount);
}

ResourceManager &ResourceManager::getInstance()
{
    static ResourceManager *mInstance = new ResourceManager();
    return *mInstance;
}
