/****************************************************************************
 Copyright (c) 2015 Chukong Technologies Inc.
 
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

#include "renderer/CCShadowMap.h"
#include "base/CCDirector.h"
#include "2d/CCLight.h"
#include "2d/CCScene.h"

NS_CC_BEGIN

ShadowMap::ShadowMap()
: _frameBuffer(nullptr)
, _shaodowmapTexture(0)
, _shadowLight(nullptr)
{
    
}

ShadowMap::~ShadowMap()
{
    if (_frameBuffer)
    {
        CC_SAFE_RELEASE(_frameBuffer);
        glDeleteTextures(1, &_shaodowmapTexture);
    }
}

unsigned int ShadowMap::getShadowMapWidth() const
{
    if (_frameBuffer)
        return _frameBuffer->getWidth();
    
    return 0;
}

unsigned int ShadowMap::getShadowMapHeight() const
{
    if (_frameBuffer)
        return _frameBuffer->getHeight();
    
    return 0;
}

ShadowMap* ShadowMap::create(int width, int height)
{
    auto shadowmap = new(std::nothrow) ShadowMap();
    if (!shadowmap->init(width, height))
    {
        CC_SAFE_DELETE(shadowmap);
    }
    CC_SAFE_RETAIN(shadowmap);
    
    return shadowmap;
}

bool ShadowMap::init(int width, int height)
{
    if (_frameBuffer)
    {
        CC_SAFE_RELEASE(_frameBuffer);
//        glDeleteTextures(1, &_shaodowmapTexture);
    }
    
    _frameBuffer = experimental::FrameBuffer::create(1, width, height);
    _frameBuffer->retain();
    
    auto rt = experimental::RenderTarget::create(width, height);
    auto rtDS = experimental::RenderTargetDepthStencil::create(width, height);
    _frameBuffer->attachRenderTarget(rt);
    _frameBuffer->attachDepthStencilTarget(rtDS);
    
//    glGenTextures(1, &_shaodowmapTexture);
//    glBindTexture(GL_TEXTURE_2D, _shaodowmapTexture);
//    
//    // Create the depth texture.
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
//    
//    // Set the textures parameters
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    
//    _frameBuffer->applyFBO();
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shaodowmapTexture, 0);
//    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        CCLOG("ERROR: Frame buffer not set up correctly");
        _frameBuffer->applyDefaultFBO();
        return false;
        
    }
    _frameBuffer->applyDefaultFBO();
    
    return true;
}

GLuint ShadowMap::getTextureName() const
{
    return _frameBuffer->getDepthStencilTarget()->getBuffer();
}

void ShadowMap::bind()
{
    
    glGetIntegerv( GL_VIEWPORT , _viewport );
    glViewport(1, 1, _frameBuffer->getWidth() - 2, _frameBuffer->getHeight() - 2);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    
    glClear(GL_DEPTH_BUFFER_BIT);
    
    Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    getLightViewProjectionMat();
    Director::getInstance()->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, _lightViewProjectionMat);
    
    _frameBuffer->applyFBO();
}

void ShadowMap::unBind()
{
    glViewport(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    _frameBuffer->applyDefaultFBO();
    
}

const Mat4& ShadowMap::getLightViewProjectionMat()
{
    auto light = Director::getInstance()->getRunningScene()->getShadowLight();
    auto world = light->getNodeToWorldTransform();
    static int count = sizeof(float) * 16;
    if (light != _shadowLight)
    {
        _shadowLight = light;
        //get projection matrix
        _lightWorldMat = world;
        _lightViewMat = _lightWorldMat.getInversed();
        auto type = light->getLightType();
        if (type == LightType::SPOT)
        {
            auto spot = (SpotLight*)light;
            Mat4::createPerspective(CC_RADIANS_TO_DEGREES(spot->getOuterAngle() * 2.f), (float)_frameBuffer->getWidth() /(float)_frameBuffer->getHeight(), 0.1f, spot->getRange() + 1.f, &_projectionMat);
        }
        else if (type == LightType::DIRECTIONAL)
        {
            //            auto dirlight = (DirectionLight*)light;
        }
        else if (type == LightType::POINT)
        {
            
        }
        
        Mat4::multiply(_projectionMat, _lightViewMat, &_lightViewProjectionMat);
    }
    else if (memcmp(world.m, _lightWorldMat.m, count) != 0)
    {
        _lightWorldMat = light->getNodeToWorldTransform();
        _lightViewMat = _lightWorldMat.getInversed();
        Mat4::multiply(_projectionMat, _lightViewMat, &_lightViewProjectionMat);
    }
    
    return _lightViewProjectionMat;
}

NS_CC_END
