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

#include "3d/CCParticleAffector.h"
#include "base/CCDirector.h"
#include "base/CCPlatformMacros.h"
#include "base/ccMacros.h"
#include "platform/CCFileUtils.h"
#include "renderer/CCTextureCache.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCGLProgramState.h"
#include "renderer/CCGLProgramCache.h"
#include "3d/CCParticleSystem3D.h"
#include "deprecated/CCString.h" // For StringUtils::format
#include "3d/CCParticleSystem3D.h" 
#include "tinyxml2.h"
NS_CC_BEGIN
    ParticleAffector::ParticleAffector(ParticleSystem3D* system):
    _parSystem(system)
{
}
ParticleAffector::~ParticleAffector()
{

}
bool ParticleAffector::load(tinyxml2::XMLElement* element)
{
    return true;
}
bool ParticleAffector::save(tinyxml2::XMLElement* element,tinyxml2::XMLDocument* xmlDoc)
{
    return true;
}
ScaleAffector::ScaleAffector(ParticleSystem3D* system):
    ParticleAffector(system)
{
}
ScaleAffector::~ScaleAffector()
{

}
void ScaleAffector::initAffector(Particle3D* p)
{
    p->deltaScale = (_scaleAffectorConfig.endScale - p->scale) / p->timeToLive;
}
void ScaleAffector::updateAffector(Particle3D* p,float dt)
{
    p->scale+= p->deltaScale*dt;
}
void  ScaleAffector::setDefaultProperty(const ScaleAffectorConfig& config)
{
    _scaleAffectorConfig= config;
}
bool ScaleAffector::load(tinyxml2::XMLElement* element)
{
    const tinyxml2:: XMLAttribute*	curAttribute=element->FirstAttribute();
    while (curAttribute) 
    {  

        if(0==strcmp(curAttribute->Name(),"type"))
        {
            curAttribute->QueryIntValue(&_scaleAffectorConfig.type);
        }
        else if(0==strcmp(curAttribute->Name(),"endScale"))
        {
            curAttribute->QueryFloatValue(&_scaleAffectorConfig.endScale);
        }
        curAttribute = curAttribute->Next();   
    }
    return true;
}
bool ScaleAffector::save(tinyxml2::XMLElement* element,tinyxml2::XMLDocument* xmlDoc)
{
    tinyxml2::XMLElement *pAffectElement = xmlDoc->NewElement("scaleAffector");
    if(pAffectElement)
    {
        pAffectElement->SetAttribute("type",_scaleAffectorConfig.type);
        pAffectElement->SetAttribute("endScale",_scaleAffectorConfig.endScale);
        element->LinkEndChild(pAffectElement);
    }
    return true;
}
ColorAffector::ColorAffector(ParticleSystem3D* system):
    ParticleAffector(system)
{
}
ColorAffector::~ColorAffector()
{

}
bool ColorAffector::load(tinyxml2::XMLElement* element)
{
    const tinyxml2:: XMLAttribute*	curAttribute=element->FirstAttribute();
    while (curAttribute) 
    {  
        if(0==strcmp(curAttribute->Name(),"type"))
        {
            curAttribute->QueryIntValue(&_colorAffectorConfig.type);
        }
        else if(0==strcmp(curAttribute->Name(),"endColorR"))
        {
            curAttribute->QueryFloatValue(&_colorAffectorConfig.endColor.x);
        }
        else if(0==strcmp(curAttribute->Name(),"endColorG"))
        {
            curAttribute->QueryFloatValue(&_colorAffectorConfig.endColor.y);
        }
        else if(0==strcmp(curAttribute->Name(),"endColorB"))
        {
            curAttribute->QueryFloatValue(&_colorAffectorConfig.endColor.z);
        }
        else if(0==strcmp(curAttribute->Name(),"endColorA"))
        {
            curAttribute->QueryFloatValue(&_colorAffectorConfig.endColor.w);
        }
        curAttribute = curAttribute->Next();  
    }
    return true;
}
bool ColorAffector::save(tinyxml2::XMLElement* element,tinyxml2::XMLDocument* xmlDoc)
{
    tinyxml2::XMLElement *pAffectElement = xmlDoc->NewElement("colorAffector");
    if(pAffectElement)
    {
        pAffectElement->SetAttribute("type",_colorAffectorConfig.type);
        pAffectElement->SetAttribute("endColorR",_colorAffectorConfig.endColor.x);
        pAffectElement->SetAttribute("endColorG",_colorAffectorConfig.endColor.y);
        pAffectElement->SetAttribute("endColorB",_colorAffectorConfig.endColor.z);
        pAffectElement->SetAttribute("endColorA",_colorAffectorConfig.endColor.w);
        element->LinkEndChild(pAffectElement);
    }
    return true;
}
void ColorAffector::initAffector(Particle3D* p)
{
    p->deltaColor.r = (_colorAffectorConfig.endColor.x - p->color.r) / p->timeToLive;
    p->deltaColor.g = (_colorAffectorConfig.endColor.y - p->color.g) / p->timeToLive;
    p->deltaColor.b = (_colorAffectorConfig.endColor.z - p->color.b) / p->timeToLive;
    p->deltaColor.a = (_colorAffectorConfig.endColor.w - p->color.a) / p->timeToLive;
}
void ColorAffector::updateAffector(Particle3D* p,float dt)
{

    p->color.r += (p->deltaColor.r * dt);
    p->color.g += (p->deltaColor.g * dt);
    p->color.b += (p->deltaColor.b * dt);
    p->color.a += (p->deltaColor.a * dt);
}
void  ColorAffector::setDefaultProperty(const ColorAffectorConfig& config)
{
    _colorAffectorConfig=config;
}
RotAffector::RotAffector(ParticleSystem3D* system):
    ParticleAffector(system)
{
}
RotAffector::~RotAffector()
{

}
void RotAffector::initAffector(Particle3D* p)
{
    p->deltaRotation = (_rotAffectorConfig.rotationEnd - p->rotation) / p->timeToLive;

}
void RotAffector::updateAffector(Particle3D* p,float dt)
{
    p->rotation+=p->deltaRotation*dt;
}
void  RotAffector::setDefaultProperty(const RotAffectorConfig& config)
{
    _rotAffectorConfig=config;
}
bool RotAffector::load(tinyxml2::XMLElement* element)
{
    const tinyxml2:: XMLAttribute*	curAttribute=element->FirstAttribute();
    while (curAttribute) 
    {  
        if(0==strcmp(curAttribute->Name(),"type"))
        {
            curAttribute->QueryIntValue(&_rotAffectorConfig.type);
        }
        else if(0==strcmp(curAttribute->Name(),"rotationEnd"))
        {
            curAttribute->QueryIntValue(&_rotAffectorConfig.rotationEnd);
        }
        curAttribute = curAttribute->Next();  
    }
    return true;
}
bool RotAffector::save(tinyxml2::XMLElement* element,tinyxml2::XMLDocument* xmlDoc)
{
    tinyxml2::XMLElement *pAffectElement = xmlDoc->NewElement("RotAffector");
    if(pAffectElement)
    {
        pAffectElement->SetAttribute("type",_rotAffectorConfig.type);
        pAffectElement->SetAttribute("rotationEnd",_rotAffectorConfig.rotationEnd);
        element->LinkEndChild(pAffectElement);
    }
    return true;
}
NS_CC_END
