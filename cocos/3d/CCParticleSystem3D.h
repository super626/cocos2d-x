/****************************************************************************
Copyright (c) 2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __CCPARTICLSYSTEM3D_H__
#define __CCPARTICLSYSTEM3D_H__
#include <vector>
#include "base/CCVector.h"
#include "base/ccTypes.h"
#include "base/CCProtocols.h"
#include "2d/CCNode.h"
#include "renderer/CCQuadCommand.h"
namespace tinyxml2
{
    class XMLElement;
    class XMLDocument; 
}
NS_CC_BEGIN
class GLProgramState;
class Texture2D;
class ParticleAffector;
struct ParEmitterConfig
{  
    int totalParticles;
    float emitDelay;    
    float emitDuration; 
    int   emitType;
    Vec3  emitPos;
    float emitRate;   
    Vec3  boxSize; 
    float sphereRadius;
    //life
    float life;
    float lifeVar; 
    //color
    Vec4  startColor;
    Vec4  startColorVar;
    //velocity
    int   dirType; 
    Vec3  velocityAxis;
    float startSpeed; 
    float startSpeedVar;
    //scale
    float startScale; 
    float startScaleVar;

    int startRotation;
    int startRotationVar;
    //uv
    Vec4  uvRect;
    //render
    int   renderType;
    Vec2  billboardSize; 
    int   billboardType;
    char  textureName[32];
    int   blendType;
};
enum  EMITTYPE
{
    EMIT_POINT,         
    EMIT_BOX,          
    EMIT_SPHERE,        
};
enum DIRECTION_TYPE
{
    DIR_PLANAR,
    DIR_SPHERE,
};
enum BILLBOARD_TYPE
{
    BILLBOARD,
    BILLBOARD_SELF,
    XY,
};
struct Particle3D
{
    int     index;          
    int     totalIndex;     
    Vec3    position;       
    Vec3    velocity;
    float   scale;
    float   deltaScale;
    Color4F deltaColor;
    Color4F color;
    float   timeToLive;
    Vec4    uvRect;	
    float   rotation;
    float   deltaRotation;
};
class ParticleSystem3D : public Node , public BlendProtocol
{
public:	
    static ParticleSystem3D * create(const std::string& particleFile);
    static ParticleSystem3D * create();
    ParticleSystem3D();
    virtual ~ParticleSystem3D();
    /**
    * particle state.
    */
    enum ParticleState
    {
        STOP,
        RUNNING,
        PAUSE,
    };
    void  setTotalParticles(int tp);
    virtual void  onEnter() override;
    /**
    * is particle system start ?
    */
    bool isStarted() const;
    /**
    * particle system control.
    */
    void start();
    void stop();
    void pause();
    void resume();
    virtual void update(float dt) ;
    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
    //set texture
    void setTexture(const std::string& texFile);
    void setTexture(Texture2D* texture);
    virtual void setBlendFunc(const BlendFunc &blendFunc) override;
    virtual const BlendFunc &getBlendFunc() const override;
    /**
    * add Affector to particle system to affect particles movement
    */
    void addAffector(ParticleAffector* affector);

    /**
    * remove particle system Affector
    */
    void removeAffector(ParticleAffector* affector);
    /**
    * load & save particle system .particle.
    */
    bool load(const std::string& fileName);
    bool save(const std::string& szFile);
private:
    void   updateBillboardParticle(Particle3D* particle,const Vec3& newPosition);
    void   emitterParticle(float dt);
    void   updateParticle(float dt);
    bool   addParticle();
    void   initParticleLife(Particle3D* particle);
    bool   isFull();
    void   initParticleDirection(Particle3D* particle);
    void   initParticlePos(Particle3D* particle);
    void   initParticleColor(Particle3D* particle);;
    void   initParticleScale(Particle3D* particle);
    void   initParticleUV(Particle3D* particle);
    void   initParticleRotation(Particle3D* particle);
    float  generateScalar(float min, float max);
    bool load(tinyxml2::XMLElement* element);
    bool save(tinyxml2::XMLDocument* xmlDoc);
protected:
    Particle3D*         _particles;
    ParEmitterConfig    _emitterConfig;
    int                 _particleIdx;
    int                 _particleCount;
    float               _emitCounter;
    bool                _started;
    ParticleState       _state;
    float               _timeRunning; // particle system running time
    Vec3                _cameraRight;
    Vec3                _cameraUp;
    V3F_C4B_T2F_Quad   *_quads;        // quads to be rendered
    QuadCommand        _quadCommand;
    Texture2D*         _texture;
    BlendFunc          _blend;
    std::vector<ParticleAffector*> _parAffectors;
};
NS_CC_END
#endif // __SPRITE3D_H_
