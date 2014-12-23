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

#include "3dparticle/CCParticleSystem3D.h"
#include "3dparticle/CCParticle3DRender.h"
#include "renderer/CCMeshCommand.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCTextureCache.h"
#include "renderer/CCGLProgramState.h"
#include "renderer/CCGLProgramCache.h"
#include "renderer/CCVertexIndexBuffer.h"
#include "base/CCDirector.h"
#include "3d/CCSprite3D.h"
#include "2d/CCCamera.h"

NS_CC_BEGIN

static bool compareParticle3D(Particle3D* left, Particle3D* right)
{
    return left->depthInView > right->depthInView;
}

Particle3DQuadRender::Particle3DQuadRender()
: _meshCommand(nullptr)
, _texture(nullptr)
, _glProgramState(nullptr)
, _indexBuffer(nullptr)
, _vertexBuffer(nullptr)
{
    
}
Particle3DQuadRender::~Particle3DQuadRender()
{
    CC_SAFE_DELETE(_meshCommand);
    CC_SAFE_RELEASE(_texture);
    CC_SAFE_RELEASE(_glProgramState);
    CC_SAFE_RELEASE(_vertexBuffer);
    CC_SAFE_RELEASE(_indexBuffer);
}

Particle3DQuadRender* Particle3DQuadRender::create(const std::string& texFile)
{
    auto ret = new Particle3DQuadRender();
    ret->autorelease();
    ret->initQuadRender(texFile);
    return ret;
}

void Particle3DQuadRender::render(Renderer* renderer, const Mat4 &transform, ParticleSystem3D* particleSystem)
{
    //batch and generate draw
    const ParticlePool &particlePool = particleSystem->getParticlePool();
    if (!_isVisible || particlePool.empty())
        return;
    
    if (_vertexBuffer == nullptr){
        GLsizei stride = sizeof(Particle3DQuadRender::posuvcolor);
        _vertexBuffer = VertexBuffer::create(stride, 4 * particleSystem->getParticleQuota());
        _vertexBuffer->retain();
    }

    if (_indexBuffer == nullptr){
        _indexBuffer = IndexBuffer::create(IndexBuffer::IndexType::INDEX_TYPE_SHORT_16, 6 * particleSystem->getParticleQuota());
        _indexBuffer->retain();
    }
    ParticlePool::PoolList activeParticleList = particlePool.getActiveParticleList();
    if (_posuvcolors.size() < activeParticleList.size() * 4)
    {
        _posuvcolors.resize(activeParticleList.size() * 4);
        _indexData.resize(activeParticleList.size() * 6);
    }

    auto camera = Camera::getVisitingCamera();
    auto cameraMat = camera->getNodeToWorldTransform();
    const Mat4 &viewMat = cameraMat.getInversed();
    

    for (auto iter : activeParticleList){
        iter->depthInView = -(viewMat.m[2] * iter->positionInWorld.x + viewMat.m[6] * iter->positionInWorld.y + viewMat.m[10] * iter->positionInWorld.z + viewMat.m[14]);
    }

    std::sort(activeParticleList.begin(), activeParticleList.end(), compareParticle3D);
    Mat4 pRotMat;
    Vec3 right(cameraMat.m[0], cameraMat.m[1], cameraMat.m[2]);
    Vec3 up(cameraMat.m[4], cameraMat.m[5], cameraMat.m[6]);
    Vec3 backward(cameraMat.m[8], cameraMat.m[9], cameraMat.m[10]);
    
    Vec3 position; //particle position
    int vertexindex = 0;
    int index = 0;
    for (auto iter : activeParticleList)
    {
        auto particle = iter;
        Vec3 halfwidth = particle->width * 0.5f * right;
        Vec3 halfheight = particle->height * 0.5f * up;
        //transform.transformPoint(particle->position, &position);
        position = particle->positionInWorld;
        Mat4::createRotation(backward, particle->zRotation, &pRotMat);
        _posuvcolors[vertexindex].position = (position + pRotMat * (- halfwidth - halfheight));
        _posuvcolors[vertexindex].color = particle->color;
        _posuvcolors[vertexindex].uv = Vec2(particle->lb_uv);

        _posuvcolors[vertexindex + 1].position = (position + pRotMat * (halfwidth - halfheight));
        _posuvcolors[vertexindex + 1].color = particle->color;
        _posuvcolors[vertexindex + 1].uv = Vec2(particle->rt_uv.x, particle->lb_uv.y);
        
        _posuvcolors[vertexindex + 2].position = (position + pRotMat * (- halfwidth + halfheight));
        _posuvcolors[vertexindex + 2].color = particle->color;
        _posuvcolors[vertexindex + 2].uv = Vec2(particle->lb_uv.x, particle->rt_uv.y);
        
        _posuvcolors[vertexindex + 3].position = (position + pRotMat * (halfwidth + halfheight));
        _posuvcolors[vertexindex + 3].color = particle->color;
        _posuvcolors[vertexindex + 3].uv = Vec2(particle->rt_uv);
        
        
        _indexData[index] = vertexindex;
        _indexData[index + 1] = vertexindex + 1;
        _indexData[index + 2] = vertexindex + 3;
        _indexData[index + 3] = vertexindex;
        _indexData[index + 4] = vertexindex + 3;
        _indexData[index + 5] = vertexindex + 2;
        
        index += 6;
        vertexindex += 4;
    }
    
    _posuvcolors.erase(_posuvcolors.begin() + vertexindex, _posuvcolors.end());
    _indexData.erase(_indexData.begin() + index, _indexData.end());
    
    _vertexBuffer->updateVertices(&_posuvcolors[0], vertexindex/* * sizeof(_posuvcolors[0])*/, 0);
    _indexBuffer->updateIndices(&_indexData[0], index/* * sizeof(unsigned short)*/, 0);
    
    GLuint texId = (_texture ? _texture->getName() : 0);
    float depthZ = -(viewMat.m[2] * transform.m[12] + viewMat.m[6] * transform.m[13] + viewMat.m[10] * transform.m[14] + viewMat.m[14]);
    _meshCommand->init(depthZ, texId, _glProgramState, particleSystem->getBlendFunc(), _vertexBuffer->getVBO(), _indexBuffer->getVBO(), GL_TRIANGLES, GL_UNSIGNED_SHORT, index, transform);
    renderer->addCommand(_meshCommand);
}

void Particle3DQuadRender::initQuadRender( const std::string& texFile )
{
    GLProgram* glProgram = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_3D_POSITION);
    if (!texFile.empty())
    {
        auto tex = Director::getInstance()->getTextureCache()->addImage(texFile);
        if (tex)
        {
            _texture = tex;
            glProgram = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_3D_PARTICLE_TEXTURE);
        }
    }
    auto glProgramState = GLProgramState::create(glProgram);
    glProgramState->retain();

    GLsizei stride = sizeof(Particle3DQuadRender::posuvcolor);

    glProgramState->setVertexAttribPointer(s_attributeNames[GLProgram::VERTEX_ATTRIB_POSITION], 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offsetof(posuvcolor, position));
    glProgramState->setVertexAttribPointer(s_attributeNames[GLProgram::VERTEX_ATTRIB_TEX_COORD], 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offsetof(posuvcolor, uv));
    glProgramState->setVertexAttribPointer(s_attributeNames[GLProgram::VERTEX_ATTRIB_COLOR], 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offsetof(posuvcolor, color));

    _glProgramState = glProgramState;
    //ret->_vertexBuffer = VertexBuffer::create(stride, 4 * 10000);
    //ret->_vertexBuffer->retain();
    //ret->_indexBuffer = IndexBuffer::create(IndexBuffer::IndexType::INDEX_TYPE_SHORT_16, 6 * 10000);
    //ret->_indexBuffer->retain();

    _meshCommand = new MeshCommand();
    _meshCommand->setTransparent(true);
    _meshCommand->setDepthTestEnabled(_depthTest);
    _meshCommand->setDepthWriteEnabled(_depthWrite);
    _meshCommand->setCullFace(GL_BACK);
    _meshCommand->setCullFaceEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////
Particle3DModelRender::Particle3DModelRender()
    : _spriteSize(Vec3::ONE)
{
    
}
Particle3DModelRender::~Particle3DModelRender()
{
    for (auto iter : _spriteList){
        iter->autorelease();
    }
}


Particle3DModelRender* Particle3DModelRender::create(const std::string& modelFile, const std::string &texFile)
{
    auto ret = new Particle3DModelRender();
    ret->_modelFile = modelFile;
    ret->_texFile = texFile;
    return ret;
}

void Particle3DModelRender::render(Renderer* renderer, const Mat4 &transform, ParticleSystem3D* particleSystem)
{
    if (!_isVisible)
        return;
    
    if (_spriteList.empty()){
        for (unsigned int i = 0; i < particleSystem->getParticleQuota(); ++i){
            Sprite3D *sprite = Sprite3D::create(_modelFile);
            sprite->setTexture(_texFile);
            sprite->retain();
            _spriteList.push_back(sprite);
        }
        if (!_spriteList.empty()){
            const AABB &aabb = _spriteList[0]->getAABB();
            Vec3 corners[8];
            aabb.getCorners(corners);
            _spriteSize = corners[3] - corners[6];
        }
    }


    const ParticlePool& particlePool = particleSystem->getParticlePool();
    ParticlePool::PoolList activeParticleList = particlePool.getActiveParticleList();
    Mat4 mat;
    Mat4 rotMat;
    Mat4 sclMat;
    Quaternion q;
    transform.decompose(nullptr, &q, nullptr);
    for (unsigned int i = 0; i < activeParticleList.size(); ++i)
    {
        auto particle = activeParticleList[i];
        q *= particle->orientation;
        Mat4::createRotation(q, &rotMat);
        sclMat.m[0] = particle->width / _spriteSize.x;
        sclMat.m[5]  = particle->height / _spriteSize.y; 
        sclMat.m[10] = particle->depth / _spriteSize.z;
        mat = rotMat * sclMat;
        mat.m[12] = particle->positionInWorld.x;
        mat.m[13] = particle->positionInWorld.y;
        mat.m[14] = particle->positionInWorld.z;
        _spriteList[i]->draw(renderer, mat, 0);
    }
}


void Particle3DRender::notifyStart( void )
{
    setVisible(true);
}

void Particle3DRender::notifyStop( void )
{
    setVisible(false);
}

void Particle3DRender::notifyRescaled( const Vec3& scale )
{
    _rendererScale = scale;
}

NS_CC_END
