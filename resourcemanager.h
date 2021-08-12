#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <map>
#include <vector>
#include <string>
#include "gltypes.h"
/**
  Super banal Resource Manager!
  This class is a singleton
 */
class ResourceManager
{
public:
    bool searchMap(std::string &name, GLuint &VAO, int &indicesCount);

    void insertInMap(std::string &name, GLuint &VAO, int &indicesCount);

    static ResourceManager& getInstance();

    std::map<std::string, int> mObjFiles;
    std::map<int, int> mIndicesCount;

private:
    ResourceManager();
    void operator=(ResourceManager&);           //Assignment operator
    ResourceManager(const ResourceManager&);    //Copy constructor
};

#endif // RESOURCEMANAGER_H
