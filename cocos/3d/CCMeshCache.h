
#ifndef __CCMESH_CACHE_H__
#define __CCMESH_CACHE_H__

#include <string>
#include "base/ccTypes.h"
#include "base/CCMap.h"

NS_CC_BEGIN

class Sprite3D;
class Mesh;
class EventListenerCustom;
class EventCustom;

class MeshCache
{
public:
    virtual ~MeshCache();
    
    static MeshCache* getInstance();
    static void purgeMeshCache();
    
    bool addMesh(const std::string& fileName, Mesh* mesh);
    Mesh* getMesh(const std::string& fileName);
    void removeMesh(const std::string& fileName);
    void removeMesh(const Mesh* mesh);
    
// #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//     void listenBackToForeground(EventCustom* event);
// #endif
    
protected:
    MeshCache();
    
    static MeshCache* _cacheInstance;
    Map<std::string, Mesh*> _cachedMeshes; //cached meshes
    
// #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//     EventListenerCustom* _backToForegroundlistener;
// #endif
};


NS_CC_END

#endif // __CCMESH_CACHE_H_
