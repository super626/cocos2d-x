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

#include "3d/CCParticleSystem3D.h"
#include "base/CCDirector.h"
#include "base/CCPlatformMacros.h"
#include "base/ccMacros.h"
#include "platform/CCFileUtils.h"
#include "renderer/CCTextureCache.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCGLProgramState.h"
#include "renderer/CCGLProgramCache.h"
#include "deprecated/CCString.h" // For StringUtils::format
#include "3d/CCCamera3D.h"
#include "base/CCConfiguration.h"
#include "3d/CCParticleAffector.h" 
#include "tinyxml2.h"
NS_CC_BEGIN
    ParticleSystem3D * ParticleSystem3D::create(const std::string& particleFile)
{
    ParticleSystem3D *ret = new (std::nothrow) ParticleSystem3D();
    if (ret && ret->load(particleFile))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return ret;
}
ParticleSystem3D * ParticleSystem3D::create()
{
    ParticleSystem3D *ret = new (std::nothrow) ParticleSystem3D();
    if (ret)
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return ret;
}
ParticleSystem3D::ParticleSystem3D():
    _particles(nullptr)
    ,_particleIdx(0)
    ,_particleCount(0)
    ,_emitCounter(0)
    ,_quads(nullptr)
    ,_texture(nullptr)
{
    _started = false;
    _state = STOP;
    _cameraUp=	Vec3(0,1,0);
    _cameraRight=Vec3(1,0,0);
    _emitterConfig.emitDuration=-1;
    _emitterConfig.emitPos = Vec3(0,0,0);
    _blend = BlendFunc::ADDITIVE;
}
ParticleSystem3D::~ParticleSystem3D()
{
    CC_SAFE_FREE(_quads);
}

void ParticleSystem3D::update(float dt) 
{  
    if (_particles)
    {
        if(_emitterConfig.billboardType	==BILLBOARD)
        {
            const Mat4& cameraViewMatrix = Camera3D::getActiveCamera()->getViewMatrix().getInversed();
            cameraViewMatrix.getRightVector(&_cameraRight);
            cameraViewMatrix.getUpVector(&_cameraUp);
        }
        if( _state == RUNNING)
        {
            emitterParticle(dt);
            updateParticle(dt);
        }
    }
    if (_state == RUNNING)
    {
        _timeRunning += dt;
        if (_emitterConfig.emitDuration != -1 && _emitterConfig.emitDuration < _timeRunning)
        {
            stop();
        }
    }
    for(int i=0; i < _children.size(); i++ )
    {
        auto node = _children.at(i);
        if ( node )
            node->update(dt);
    }
}
bool ParticleSystem3D::isFull()
{
    return (_particleCount == _emitterConfig.totalParticles);
}
void ParticleSystem3D::onEnter()
{
    Node::onEnter();
    // update after action in run!
    this->scheduleUpdateWithPriority(1);
}
bool ParticleSystem3D::isStarted() const
{
    return _started;
}
//add particle
bool  ParticleSystem3D::addParticle()
{
    if (this->isFull())
    {
        return false;
    }
    Particle3D * particle = &_particles[ _particleCount ];
    if(particle)
    {
        initParticleLife(particle);
        initParticleColor(particle);
        initParticlePos(particle);
        initParticleDirection(particle);
        initParticleScale(particle);
        initParticleUV(particle);
        initParticleRotation(particle);
        for(std::vector<ParticleAffector*>::iterator iter =_parAffectors.begin(); iter!=_parAffectors.end(); ++iter)
        {
            (*iter)->initAffector(particle);
        }
    }
    ++_particleCount;
    return true;
}
void ParticleSystem3D::start()
{
    _started = true;
    if (_state != RUNNING)
    {
        _state = RUNNING;
        _timeRunning = 0.0f;
    }
    for(int i=0; i < _children.size(); i++ )
    {
        auto node = (ParticleSystem3D*)_children.at(i);
        if ( node)
            node->start();
    }
}
void ParticleSystem3D::pause()
{
    if (_state == RUNNING)
    {
        _state = PAUSE;
    }
    for(int i=0; i < _children.size(); i++ )
    {
        auto node = (ParticleSystem3D*)_children.at(i);
        if ( node)
            node->pause();
    }
}
void ParticleSystem3D::resume()
{
    if (_state == PAUSE)
    {
        _state = RUNNING;
    }
    for(int i=0; i < _children.size(); i++ )
    {
        auto node = (ParticleSystem3D*)_children.at(i);
        if ( node)
            node->resume();
    }
}

void ParticleSystem3D::stop()
{
    _started = false;
    if (_state != STOP)
    {
        ParticleState old = _state;
        _state = STOP;
    }
    for(int i=0; i < _children.size(); i++ )
    {
        auto node = (ParticleSystem3D*)_children.at(i);
        if ( node)
            node->stop();
    }
}
void ParticleSystem3D::setTexture(const std::string& texFile)
{
    auto tex = Director::getInstance()->getTextureCache()->addImage(texFile);
    if( tex && _texture != tex ) {
        CC_SAFE_RETAIN(tex);
        CC_SAFE_RELEASE_NULL(_texture);
        _texture = tex;
    }
}
void ParticleSystem3D::setTexture(Texture2D* texture)
{
    if(_texture != texture) {
        CC_SAFE_RETAIN(texture);
        CC_SAFE_RELEASE_NULL(_texture);
        _texture = texture;
    }
}
void ParticleSystem3D::setBlendFunc(const BlendFunc &blendFunc)
{
    if(_blend.src != blendFunc.src || _blend.dst != blendFunc.dst)
    {
        _blend = blendFunc;
    }
}
const BlendFunc& ParticleSystem3D::getBlendFunc() const
{
    return _blend;
}

void ParticleSystem3D::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    if(_particleIdx > 0)
    {
        _quadCommand.init(_globalZOrder, _texture->getName(), getGLProgramState(), _blend, _quads, _particleIdx, transform);
        // Director::getInstance()->getRenderer()->addCommand(&_quadCommand);
        renderer->addCommand(&_quadCommand);
    }
    for(int i=0; i < _children.size(); i++ )
    {
        auto node = _children.at(i);
        if ( node)
            node->draw(renderer,transform,flags);
    }
}
void ParticleSystem3D::setTotalParticles(int tp)
{
    _emitterConfig.totalParticles = tp;

    CC_SAFE_FREE(_particles);
    CC_SAFE_FREE(_quads);
    _particles = (Particle3D*)calloc(tp, sizeof(Particle3D));
    _quads = (V3F_C4B_T2F_Quad*)malloc(tp * sizeof(V3F_C4B_T2F_Quad));
    setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP));
}
void ParticleSystem3D::updateBillboardParticle(Particle3D* particle,const Vec3& newPosition)
{ 
    V3F_C4B_T2F_Quad *quad=&(_quads[_particleIdx]);
    Color4B color = Color4B(particle->color.r*255,particle->color.g*255,particle->color.b*255,particle->color.a*255);
    Vec3 halfsizeright(0.5f*_emitterConfig.billboardSize.x*particle->scale*_cameraRight);
    Vec3 halfsizeup(0.5f*_emitterConfig.billboardSize.y*particle->scale*_cameraUp);
    quad->bl.colors = color;
    quad->br.colors = color;
    quad->tl.colors = color;
    quad->tr.colors = color;
    Vec3 vOffset[4];
    vOffset[0] =-halfsizeright-halfsizeup;
    vOffset[1] =halfsizeright-halfsizeup;
    vOffset[2] =-halfsizeright+halfsizeup;
    vOffset[3] =halfsizeright+halfsizeup;
    if (particle->rotation!=0.0f) 
    {
        Vec3 axis=vOffset[3] - vOffset[0];
        axis.cross((vOffset[2] - vOffset[1]));
        Mat4 rotMat;
        Mat4::createRotation(axis, CC_DEGREES_TO_RADIANS(particle->rotation), &rotMat);
        rotMat.transformVector(&vOffset[0]);
        rotMat.transformVector(&vOffset[1]);
        rotMat.transformVector(&vOffset[2]);
        rotMat.transformVector(&vOffset[3]);
    }
    quad->bl.vertices =newPosition+vOffset[0];
    quad->br.vertices =newPosition+vOffset[1];
    quad->tl.vertices =newPosition+vOffset[2];
    quad->tr.vertices =newPosition+vOffset[3];
    quad->bl.texCoords.u = particle->uvRect.x;
    quad->bl.texCoords.v = particle->uvRect.w;
    // bottom-right vertex:
    quad->br.texCoords.u = particle->uvRect.z;
    quad->br.texCoords.v = particle->uvRect.w;
    // top-left vertex:
    quad->tl.texCoords.u = particle->uvRect.x;
    quad->tl.texCoords.v = particle->uvRect.y;
    // top-right vertex:
    quad->tr.texCoords.u = particle->uvRect.z;
    quad->tr.texCoords.v = particle->uvRect.y;
}
void   ParticleSystem3D::initParticlePos(Particle3D* particle)
{
    if(_emitterConfig.emitType == (int)EMIT_BOX)
    {
        particle->position.x =_emitterConfig.boxSize.x*0.5*CCRANDOM_MINUS1_1();
        particle->position.y =_emitterConfig.boxSize.y*0.5*CCRANDOM_MINUS1_1();
        particle->position.z =_emitterConfig.boxSize.z*0.5*CCRANDOM_MINUS1_1();
    }
    else if (_emitterConfig.emitType == (int)EMIT_SPHERE)
    {
        Vec3 r = Vec3::UNIT_Y * _emitterConfig.sphereRadius;
        Mat4 rot,rotX,rotY,rotZ;
        Mat4::createRotationX(CC_DEGREES_TO_RADIANS(generateScalar(0, 360)), &rotX);
        Mat4::createRotationY(CC_DEGREES_TO_RADIANS(generateScalar(0, 360)), &rotY);
        Mat4::createRotationZ(CC_DEGREES_TO_RADIANS(generateScalar(0, 360)), &rotZ);
        rot =rotX*rotY*rotZ;
        particle->position = rot * r;
    }
    else
    {
        particle->position=Vec3::ZERO;
    }
    particle->position+=_emitterConfig.emitPos;
}
void   ParticleSystem3D::initParticleLife(Particle3D* particle)
{
    particle->timeToLive = _emitterConfig.life + _emitterConfig.lifeVar * CCRANDOM_MINUS1_1();
    particle->timeToLive = MAX(0, particle->timeToLive);
}
void    ParticleSystem3D::initParticleDirection(Particle3D* particle)
{
    if (_emitterConfig.dirType == DIR_SPHERE)
    {  
        if (_emitterConfig.emitType == (int)EMIT_POINT)
        {
            Mat4 rot,rotX,rotY,rotZ;
            Mat4::createRotationX(CC_DEGREES_TO_RADIANS(generateScalar(0, 360)), &rotX);
            Mat4::createRotationY(CC_DEGREES_TO_RADIANS(generateScalar(0, 360)), &rotY);
            Mat4::createRotationZ(CC_DEGREES_TO_RADIANS(generateScalar(0, 360)), &rotZ);
            rot =rotX*rotY*rotZ;
            particle->velocity = rot * Vec3::UNIT_Y;
        }
        else
        {
            particle->velocity = particle->position;
        }
    }
    else if (_emitterConfig.dirType ==DIR_PLANAR)
    {
        particle->velocity=_emitterConfig.velocityAxis;  
    }
    particle->velocity.normalize();
    float speed =_emitterConfig.startSpeed + _emitterConfig.startSpeedVar * CCRANDOM_MINUS1_1();
    if (_emitterConfig.dirType != DIR_SPHERE)
    {
    }
    else
    {
        particle->velocity*=speed;
    }
}
void  ParticleSystem3D:: initParticleColor(Particle3D* particle)
{
    particle->color.r = clampf(_emitterConfig.startColor.x + _emitterConfig.startColorVar.x * CCRANDOM_MINUS1_1(), 0, 1);
    particle->color.g = clampf(_emitterConfig.startColor.y + _emitterConfig.startColorVar.y * CCRANDOM_MINUS1_1(), 0, 1);
    particle->color.b = clampf(_emitterConfig.startColor.z + _emitterConfig.startColorVar.z * CCRANDOM_MINUS1_1(), 0, 1);
    particle->color.a = clampf(_emitterConfig.startColor.w + _emitterConfig.startColorVar.w * CCRANDOM_MINUS1_1(), 0, 1);

}
float   ParticleSystem3D::generateScalar(float min, float max)
{
    return min + (max - min) * CCRANDOM_0_1();
}
void    ParticleSystem3D::initParticleScale(Particle3D* particle)
{
    particle->scale = _emitterConfig.startScale + _emitterConfig.startScaleVar * CCRANDOM_MINUS1_1();
}
void  ParticleSystem3D::initParticleUV(Particle3D* particle)
{
    particle->uvRect = _emitterConfig.uvRect;
}
void   ParticleSystem3D::initParticleRotation(Particle3D* particle)
{
    particle->rotation = _emitterConfig.startRotation + _emitterConfig.startRotationVar * CCRANDOM_MINUS1_1();
}
void  ParticleSystem3D::emitterParticle(float dt)
{
    if (_timeRunning >= _emitterConfig.emitDelay)
    {
        if (_emitterConfig.emitRate)
        {
            float rate = 1.0f / _emitterConfig.emitRate;
            if (_particleCount < _emitterConfig.totalParticles)
            {
                _emitCounter += dt;
            }
            while (_particleCount <_emitterConfig.totalParticles && _emitCounter > rate) 
            {
                this->addParticle();
                _emitCounter -= rate;
            }
        }
    }
}
void   ParticleSystem3D::updateParticle(float dt)
{
    _particleIdx = 0;
    while (_particleIdx < _particleCount)
    {
        Particle3D *p = &_particles[_particleIdx];
        p->timeToLive -= dt;
        if (p->timeToLive > 0.0f)
        {

            for(std::vector<ParticleAffector*>::iterator iter =_parAffectors.begin(); iter!=_parAffectors.end(); ++iter)
            {
                (*iter)->updateAffector(p,dt);
            }
            p->position += p->velocity * dt;
            Vec3    newPos=p->position;
            updateBillboardParticle(p,newPos);
            ++_particleIdx;
        }
        else 
        {
            // life < 0
            if( _particleIdx != _particleCount-1 )
            {
                _particles[_particleIdx] = _particles[_particleCount-1];
            }
            --_particleCount;
        }
    }
}
void ParticleSystem3D::addAffector(ParticleAffector* affector)
{
    _parAffectors.push_back(affector);

}
void ParticleSystem3D::removeAffector(ParticleAffector* affector)
{
    for (size_t i = 0; i < _parAffectors.size(); i++)
    {
        if (_parAffectors[i] == affector)
        {
            delete affector;
            _parAffectors.erase(_parAffectors.begin() + i);
            break;
        }
    }
}
bool ParticleSystem3D::load(const std::string& fileName)
{
    tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
    ssize_t nSize; 
    const char* pXmlBuffer = (const char*)FileUtils::getInstance()->getFileData(fileName.c_str(), "rb", &nSize);
    if(NULL == pXmlBuffer)  
    {  
        CCLOG("can not read xml file"); 
        delete pDoc;
        return false;  
    }  
    pDoc->Parse(pXmlBuffer, nSize);  
    delete[] pXmlBuffer;
    tinyxml2::XMLElement *rootElement = pDoc->RootElement();
    if(!rootElement)
    {
        delete pDoc;
        return false;
    }
    tinyxml2::XMLElement *particleElement =rootElement->FirstChildElement("particleSystem");
    while(particleElement)  
    {  
        ParticleSystem3D* particleSystem = ParticleSystem3D::create();
        if(particleSystem)
        {
            particleSystem->load(particleElement);
            addChild(particleSystem);
        }
        particleElement= particleElement->NextSiblingElement("particleSystem");
    }
    delete pDoc;
    return true;
}
bool ParticleSystem3D::save(const std::string& szFile)
{ 
    tinyxml2::XMLDocument* xmlDoc = new tinyxml2::XMLDocument();
    if (NULL==xmlDoc) {

        return false;
    }
    //xmlÉùÃ÷
    tinyxml2::XMLDeclaration *pDel = xmlDoc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
    if (NULL==pDel) {

        delete xmlDoc;
        return false;
    }
    xmlDoc->LinkEndChild(pDel);
    for(int i=0; i < _children.size(); i++ )
    {
        auto node = (ParticleSystem3D*)_children.at(i);
        if ( node)
            node->save(xmlDoc);
    }
    xmlDoc->SaveFile(szFile.c_str());
    delete xmlDoc;
    return true;
}
bool ParticleSystem3D::load(tinyxml2::XMLElement* element)
{
    const tinyxml2:: XMLAttribute*	curAttribute=element->FirstAttribute();
    while (curAttribute) 
    {  
        if(0==strcmp(curAttribute->Name(),"totalParticles"))
        {
            curAttribute->QueryIntValue(&_emitterConfig.totalParticles);
        }
        else if(0==strcmp(curAttribute->Name(),"emitDelay"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.emitDelay);
        }
        else if(0==strcmp(curAttribute->Name(),"emitPosX"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.emitPos.x);
        }
        else if(0==strcmp(curAttribute->Name(),"emitPosY"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.emitPos.y);
        }
        else if(0==strcmp(curAttribute->Name(),"emitPosZ"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.emitPos.z);
        }
        else if(0==strcmp(curAttribute->Name(),"emitDuration"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.emitDuration);
        }
        else if(0==strcmp(curAttribute->Name(),"emitRate"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.emitRate);
        }
        else if(0==strcmp(curAttribute->Name(),"emitType"))
        {
            curAttribute->QueryIntValue(&_emitterConfig.emitType);
        }
        else if(0==strcmp(curAttribute->Name(),"boxSizeX"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.boxSize.x);
        }
        else if(0==strcmp(curAttribute->Name(),"boxSizeY"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.boxSize.y);
        }
        else if(0==strcmp(curAttribute->Name(),"boxSizeZ"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.boxSize.z);
        }
        else if(0==strcmp(curAttribute->Name(),"sphereRadius"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.sphereRadius);
        }
        else if(0==strcmp(curAttribute->Name(),"life"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.life);
        }
        else if(0==strcmp(curAttribute->Name(),"lifeVar"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.lifeVar);
        }
        else if(0==strcmp(curAttribute->Name(),"startColorR"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.startColor.x);
        }
        else if(0==strcmp(curAttribute->Name(),"startColorG"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.startColor.y);
        }
        else if(0==strcmp(curAttribute->Name(),"startColorB"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.startColor.z);
        }
        else if(0==strcmp(curAttribute->Name(),"startColorA"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.startColor.w);
        }
        else if(0==strcmp(curAttribute->Name(),"startColorVarR"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.startColorVar.x);
        }
        else if(0==strcmp(curAttribute->Name(),"startColorVarG"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.startColorVar.y);
        }
        else if(0==strcmp(curAttribute->Name(),"startColorVarB"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.startColorVar.z);
        }
        else if(0==strcmp(curAttribute->Name(),"startColorVarA"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.startColorVar.w);
        }
        else if(0==strcmp(curAttribute->Name(),"dirType"))
        {
            curAttribute->QueryIntValue(&_emitterConfig.dirType);
        }
        else if(0==strcmp(curAttribute->Name(),"velocityAxisX"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.velocityAxis.x);
        }
        else if(0==strcmp(curAttribute->Name(),"velocityAxisY"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.velocityAxis.y);
        }
        else if(0==strcmp(curAttribute->Name(),"velocityAxisZ"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.velocityAxis.z);
        }
        else if(0==strcmp(curAttribute->Name(),"startSpeed"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.startSpeed);
        } 
        else if(0==strcmp(curAttribute->Name(),"startSpeedVar"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.startSpeedVar);
        } 
        else if(0==strcmp(curAttribute->Name(),"startScale"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.startScale);
        }
        else if(0==strcmp(curAttribute->Name(),"startScaleVar"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.startScaleVar);
        }
        else if(0==strcmp(curAttribute->Name(),"startRotation"))
        {
            curAttribute->QueryIntValue(&_emitterConfig.startRotation);
        }
        else if(0==strcmp(curAttribute->Name(),"startRotationVar"))
        {
            curAttribute->QueryIntValue(&_emitterConfig.startRotationVar);
        }
        else if(0==strcmp(curAttribute->Name(),"uvRectLeft"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.uvRect.x);
        }
        else if(0==strcmp(curAttribute->Name(),"uvRectTop"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.uvRect.y);
        }
        else if(0==strcmp(curAttribute->Name(),"uvRectRight"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.uvRect.z);
        }
        else if(0==strcmp(curAttribute->Name(),"uvRectBottom"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.uvRect.w);
        }
        else if(0==strcmp(curAttribute->Name(),"renderType"))
        {
            curAttribute->QueryIntValue(&_emitterConfig.renderType);
        }
        else if(0==strcmp(curAttribute->Name(),"billboardSizeX"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.billboardSize.x);
        }
        else if(0==strcmp(curAttribute->Name(),"billboardSizeY"))
        {
            curAttribute->QueryFloatValue(&_emitterConfig.billboardSize.y);
        }
        else if(0==strcmp(curAttribute->Name(),"billboardType"))
        {
            curAttribute->QueryIntValue(&_emitterConfig.billboardType);
        }
        else if(0==strcmp(curAttribute->Name(),"textureName"))
        {   
            strcpy(_emitterConfig.textureName,curAttribute->Value());
        }
        else if(0==strcmp(curAttribute->Name(),"blendType"))
        {
            curAttribute->QueryIntValue(&_emitterConfig.blendType);
        }
        curAttribute = curAttribute->Next();  
    } 
    tinyxml2::XMLElement *affectorElement = element->FirstChildElement();
    while(affectorElement)  
    {  
        ParticleAffector*  particleAffector=NULL;
        if(0==strcmp(affectorElement->Name(),"colorAffector"))
        {
            particleAffector = new 	ColorAffector(this);
        }
        else if(0==strcmp(affectorElement->Name(),"RotAffector"))
        {
            particleAffector = new 	RotAffector(this);
        }
        else if(0==strcmp(affectorElement->Name(),"scaleAffector"))
        {
            particleAffector = new 	ScaleAffector(this);
        }
        if(particleAffector)
        {
            particleAffector->load(affectorElement);
            addAffector(particleAffector);
        }
        affectorElement= affectorElement->NextSiblingElement();
    }
    setTotalParticles(_emitterConfig.totalParticles);
    setTexture(_emitterConfig.textureName);
    return true;
}
bool ParticleSystem3D::save(tinyxml2::XMLDocument* xmlDoc)
{
    tinyxml2::XMLElement *pElement = xmlDoc->NewElement("particleSystem");
    pElement->SetAttribute("totalParticles",_emitterConfig.totalParticles);
    pElement->SetAttribute("emitPosX",_emitterConfig.emitPos.x);
    pElement->SetAttribute("emitPosY",_emitterConfig.emitPos.y);
    pElement->SetAttribute("emitPosZ",_emitterConfig.emitPos.z);
    pElement->SetAttribute("emitDelay",_emitterConfig.emitDelay);
    pElement->SetAttribute("emitDuration",_emitterConfig.emitDuration);
    pElement->SetAttribute("emitRate",_emitterConfig.emitRate);
    pElement->SetAttribute("emitType",_emitterConfig.emitType);
    pElement->SetAttribute("boxSizeX",_emitterConfig.boxSize.x);
    pElement->SetAttribute("boxSizeY",_emitterConfig.boxSize.y);
    pElement->SetAttribute("boxSizeZ",_emitterConfig.boxSize.z);
    pElement->SetAttribute("sphereRadius",_emitterConfig.sphereRadius);
    pElement->SetAttribute("life",_emitterConfig.life);
    pElement->SetAttribute("lifeVar",_emitterConfig.lifeVar);
    pElement->SetAttribute("startColorR",_emitterConfig.startColor.x);
    pElement->SetAttribute("startColorG",_emitterConfig.startColor.y);
    pElement->SetAttribute("startColorB",_emitterConfig.startColor.z);
    pElement->SetAttribute("startColorA",_emitterConfig.startColor.w);
    pElement->SetAttribute("startColorVarR",_emitterConfig.startColorVar.x);
    pElement->SetAttribute("startColorVarG",_emitterConfig.startColorVar.y);
    pElement->SetAttribute("startColorVarB",_emitterConfig.startColorVar.z);
    pElement->SetAttribute("startColorVarA",_emitterConfig.startColorVar.w);
    pElement->SetAttribute("dirType",_emitterConfig.dirType);
    pElement->SetAttribute("velocityAxisX",_emitterConfig.velocityAxis.x);
    pElement->SetAttribute("velocityAxisY",_emitterConfig.velocityAxis.y);
    pElement->SetAttribute("velocityAxisZ",_emitterConfig.velocityAxis.z);
    pElement->SetAttribute("startSpeed",_emitterConfig.startSpeed);
    pElement->SetAttribute("startSpeedVar",_emitterConfig.startSpeedVar);
    pElement->SetAttribute("startScale",_emitterConfig.startScale);
    pElement->SetAttribute("startScaleVar",_emitterConfig.startScaleVar);
    pElement->SetAttribute("startRotation",_emitterConfig.startRotation);
    pElement->SetAttribute("startRotationVar",_emitterConfig.startRotationVar);
    pElement->SetAttribute("uvRectLeft",_emitterConfig.uvRect.x);
    pElement->SetAttribute("uvRectTop",_emitterConfig.uvRect.y);
    pElement->SetAttribute("uvRectRight",_emitterConfig.uvRect.z);
    pElement->SetAttribute("uvRectBottom",_emitterConfig.uvRect.w);
    pElement->SetAttribute("renderType",_emitterConfig.renderType);
    pElement->SetAttribute("billboardSizeX",_emitterConfig.billboardSize.x);
    pElement->SetAttribute("billboardSizeY",_emitterConfig.billboardSize.y);
    pElement->SetAttribute("billboardType",_emitterConfig.billboardType);
    pElement->SetAttribute("textureName",_emitterConfig.textureName);
    pElement->SetAttribute("blendType",_emitterConfig.blendType);
    xmlDoc->LinkEndChild(pElement);
    for (size_t i = 0; i < _parAffectors.size(); i++)
    {
        _parAffectors[i]->save(pElement,xmlDoc);
    }
    return true;
}
NS_CC_END
