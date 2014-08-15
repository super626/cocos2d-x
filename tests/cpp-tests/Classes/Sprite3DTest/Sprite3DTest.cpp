/****************************************************************************
 Copyright (c) 2012 cocos2d-x.org
 Copyright (c) 2013-2014 Chukong Technologies Inc.
 
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

#include "Sprite3DTest.h"
#include "3d/CCAnimation3D.h"
#include "3d/CCAnimate3D.h"
#include "3d/CCMesh.h"
#include "3d/CCSubMesh.h"
#include "3d/CCAttachNode.h"

#include <algorithm>
#include "../testResource.h"

#include "3d/CCDrawNode3D.h"
#include "3d/CCAABB.h"
#include "3d/CCOBB.h"
#include "3d/CCRay.h"
#include "3d/CCCamera3D.h"

enum
{
    IDC_NEXT = 100,
    IDC_BACK,
    IDC_RESTART
};

static int sceneIdx = -1;


static std::function<Layer*()> createFunctions[] =
{
    CL(Sprite3DBasicTest),
    CL(Sprite3DHitTest),
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WP8) && (CC_TARGET_PLATFORM != CC_PLATFORM_WINRT)
    // 3DEffect use custom shader which is not supported on WP8/WinRT yet. 
    CL(Sprite3DEffectTest),
#endif
    CL(Sprite3DWithSkinTest),
    CL(Animate3DTest),
    CL(AttachmentTest),
    CL(Sprite3DWithSubMeshTest),
    CL(Sprite3DWithCollisonTest),
    CL(Sprite3DWithOBBPerfromanceTest),
    CL(Sprite3DWithAABBPerfromanceTest),
    CL(BillBoardTest)
};

#define MAX_LAYER    (sizeof(createFunctions) / sizeof(createFunctions[0]))

static Layer* nextSpriteTestAction()
{
    sceneIdx++;
    sceneIdx = sceneIdx % MAX_LAYER;
    
    auto layer = (createFunctions[sceneIdx])();
    return layer;
}

static Layer* backSpriteTestAction()
{
    sceneIdx--;
    int total = MAX_LAYER;
    if( sceneIdx < 0 )
        sceneIdx += total;
    
    auto layer = (createFunctions[sceneIdx])();
    return layer;
}

static Layer* restartSpriteTestAction()
{
    auto layer = (createFunctions[sceneIdx])();
    return layer;
}

//------------------------------------------------------------------
//
// SpriteTestDemo
//
//------------------------------------------------------------------

Sprite3DTestDemo::Sprite3DTestDemo(void)
: BaseTest()
{
}

Sprite3DTestDemo::~Sprite3DTestDemo(void)
{
}

std::string Sprite3DTestDemo::title() const
{
    return "No title";
}

std::string Sprite3DTestDemo::subtitle() const
{
    return "";
}

void Sprite3DTestDemo::onEnter()
{
    BaseTest::onEnter();
}

void Sprite3DTestDemo::restartCallback(Ref* sender)
{
    auto s = new Sprite3DTestScene();
    s->addChild(restartSpriteTestAction());
    
    Director::getInstance()->replaceScene(s);
    s->release();
}

void Sprite3DTestDemo::nextCallback(Ref* sender)
{
    auto s = new Sprite3DTestScene();
    s->addChild( nextSpriteTestAction() );
    Director::getInstance()->replaceScene(s);
    s->release();
}

void Sprite3DTestDemo::backCallback(Ref* sender)
{
    auto s = new Sprite3DTestScene();
    s->addChild( backSpriteTestAction() );
    Director::getInstance()->replaceScene(s);
    s->release();
}


//------------------------------------------------------------------
//
// Sprite3DBasicTest
//
//------------------------------------------------------------------

Sprite3DBasicTest::Sprite3DBasicTest()
{
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = CC_CALLBACK_2(Sprite3DBasicTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    auto s = Director::getInstance()->getWinSize();
    addNewSpriteWithCoords( Vec2(s.width/2, s.height/2) );
}

void Sprite3DBasicTest::addNewSpriteWithCoords(Vec2 p)
{
    //int idx = (int)(CCRANDOM_0_1() * 1400.0f / 100.0f);
    //int x = (idx%5) * 85;
    //int y = (idx/5) * 121;
    
//    //option 1: load a obj that contain the texture in it
//    auto sprite = Sprite3D::create("sprite3dTest/scene01.obj");
    
    //option 2: load obj and assign the texture
    auto sprite = Sprite3D::create("Sprite3DTest/boss1.obj");

    sprite->setScale(3.f);
    sprite->setTexture("Sprite3DTest/boss.png");
    
    //
    //sprite->setEffect(cocos2d::EFFECT_OUTLINE);
    
    //add to scene
    addChild( sprite );
    
    sprite->setPosition( Vec2( p.x, p.y) );
    
    ActionInterval* action;
    float random = CCRANDOM_0_1();
    
    if( random < 0.20 )
        action = ScaleBy::create(3, 2);
    else if(random < 0.40)
        action = RotateBy::create(3, 360);
    else if( random < 0.60)
        action = Blink::create(1, 3);
    else if( random < 0.8 )
        action = TintBy::create(2, 0, -255, -255);
    else
        action = FadeOut::create(2);
    auto action_back = action->reverse();
    auto seq = Sequence::create( action, action_back, nullptr );
    
    sprite->runAction( RepeatForever::create(seq) );
}

void Sprite3DBasicTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    for (auto touch: touches)
    {
        auto location = touch->getLocation();
        
        addNewSpriteWithCoords( location );
    }
}

std::string Sprite3DBasicTest::title() const
{
    return "Testing Sprite3D";
}

std::string Sprite3DBasicTest::subtitle() const
{
    return "Tap screen to add more sprites";
}

//------------------------------------------------------------------
//
// Sprite3DHitTest
//
//------------------------------------------------------------------


Sprite3DHitTest::Sprite3DHitTest()
{
    auto s = Director::getInstance()->getWinSize();
    
    auto sprite1 = Sprite3D::create("Sprite3DTest/boss1.obj");

    sprite1->setScale(4.f);
    sprite1->setTexture("Sprite3DTest/boss.png");
    sprite1->setPosition( Vec2(s.width/2, s.height/2) );
    sprite1->setContentSize(Size(20, 20));
    
    //add to scene
    addChild( sprite1 );
    sprite1->runAction(RepeatForever::create(RotateBy::create(3, 360)));
    
    auto sprite2 = Sprite3D::create("Sprite3DTest/boss1.obj");
    
    sprite2->setScale(4.f);
    sprite2->setTexture("Sprite3DTest/boss.png");
    sprite2->setPosition( Vec2(s.width/2, s.height/2) );
    sprite2->setContentSize(Size(20, 20));
    sprite2->setAnchorPoint(Vec2(0.5, 0.5));

    //add to scene
    addChild( sprite2 );
    sprite2->runAction(RepeatForever::create(RotateBy::create(3, -360)));
    
    
    // Make sprite1 touchable
    auto listener1 = EventListenerTouchOneByOne::create();
    listener1->setSwallowTouches(true);
    
    listener1->onTouchBegan = [](Touch* touch, Event* event){
        auto target = static_cast<Sprite3D*>(event->getCurrentTarget());

        Vec2 locationInNode = target->convertToNodeSpace(touch->getLocation());
        Size s = target->getContentSize();
        Rect rect = Rect(-s.width/2, -s.height/2, s.width, s.height);
        
        if (rect.containsPoint(locationInNode))
        {
            log("sprite3d began... x = %f, y = %f", locationInNode.x, locationInNode.y);
            target->setOpacity(100);
            return true;
        }
        return false;
    };
    
    listener1->onTouchMoved = [](Touch* touch, Event* event){
        auto target = static_cast<Sprite3D*>(event->getCurrentTarget());
        target->setPosition(target->getPosition() + touch->getDelta());
    };
    
    listener1->onTouchEnded = [=](Touch* touch, Event* event){
        auto target = static_cast<Sprite3D*>(event->getCurrentTarget());
        log("sprite3d onTouchesEnded.. ");
        target->setOpacity(255);
    };
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, sprite1);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1->clone(), sprite2);
    
}

std::string Sprite3DHitTest::title() const
{
    return "Testing Sprite3D Touch in 2D";
}

std::string Sprite3DHitTest::subtitle() const
{
    return "Tap Sprite3D and Drag";
}

void Sprite3DTestScene::runThisTest()
{
    auto layer = nextSpriteTestAction();
    addChild(layer);
    
    Director::getInstance()->replaceScene(this);
}

static int tuple_sort( const std::tuple<ssize_t,Effect3D*,CustomCommand> &tuple1, const std::tuple<ssize_t,Effect3D*,CustomCommand> &tuple2 )
{
    return std::get<0>(tuple1) < std::get<0>(tuple2);
}

EffectSprite3D* EffectSprite3D::createFromObjFileAndTexture(const std::string &objFilePath, const std::string &textureFilePath)
{
    auto sprite = new EffectSprite3D();
    if (sprite && sprite->initWithFile(objFilePath))
    {
        sprite->autorelease();
        sprite->setTexture(textureFilePath);
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

EffectSprite3D::EffectSprite3D()
: _defaultEffect(nullptr)
{
    
}

EffectSprite3D::~EffectSprite3D()
{
    for(auto effect : _effects)
    {
        CC_SAFE_RELEASE_NULL(std::get<1>(effect));
    }
    CC_SAFE_RELEASE(_defaultEffect);
}

void EffectSprite3D::setEffect3D(Effect3D *effect)
{
    if(_defaultEffect == effect) return;
    CC_SAFE_RETAIN(effect);
    CC_SAFE_RELEASE(_defaultEffect);
    _defaultEffect = effect;
}

void EffectSprite3D::addEffect(Effect3DOutline* effect, ssize_t order)
{
    if(nullptr == effect) return;
    effect->retain();
    effect->setTarget(this);
    
    _effects.push_back(std::make_tuple(order,effect,CustomCommand()));

    std::sort(std::begin(_effects), std::end(_effects), tuple_sort);
}

const std::string Effect3DOutline::_vertShaderFile = "Shaders3D/OutLine.vert";
const std::string Effect3DOutline::_fragShaderFile = "Shaders3D/OutLine.frag";
const std::string Effect3DOutline::_keyInGLProgramCache = "Effect3DLibrary_Outline";
GLProgram* Effect3DOutline::getOrCreateProgram()
{
    auto program = GLProgramCache::getInstance()->getGLProgram(_keyInGLProgramCache);
    if(program == nullptr)
    {
        program = GLProgram::createWithFilenames(_vertShaderFile, _fragShaderFile);
        GLProgramCache::getInstance()->addGLProgram(program, _keyInGLProgramCache);
    }
    return program;
}

Effect3DOutline* Effect3DOutline::create()
{
    Effect3DOutline* effect = new Effect3DOutline();
    if(effect && effect->init())
    {
        effect->autorelease();
        return effect;
    }
    else
    {
        CC_SAFE_DELETE(effect);
        return nullptr;
    }
}

bool Effect3DOutline::init()
{

    GLProgram* glprogram = GLProgram::createWithFilenames(_vertShaderFile, _fragShaderFile);
    if(nullptr == glprogram)
    {
        CC_SAFE_DELETE(glprogram);
        return false;
    }
    _glProgramState = GLProgramState::create(glprogram);
    if(nullptr == _glProgramState)
    {
        return false;
    }
    _glProgramState->retain();
    _glProgramState->setUniformVec3("OutLineColor", _outlineColor);
    _glProgramState->setUniformFloat("OutlineWidth", _outlineWidth);
    
    return true;
}

Effect3DOutline::Effect3DOutline()
: _outlineWidth(1.0f)
, _outlineColor(1, 1, 1)
, _sprite(nullptr)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    _backToForegroundListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED,
                                                          [this](EventCustom*)
                                                          {
                                                              auto glProgram = _glProgramState->getGLProgram();
                                                              glProgram->reset();
                                                              glProgram->initWithFilenames(_vertShaderFile, _fragShaderFile);
                                                              glProgram->link();
                                                              glProgram->updateUniforms();
                                                          }
                                                          );
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, -1);
#endif
}

Effect3DOutline::~Effect3DOutline()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

void Effect3DOutline::setOutlineColor(const Vec3& color)
{
    if(_outlineColor != color)
    {
        _outlineColor = color;
        _glProgramState->setUniformVec3("OutLineColor", _outlineColor);
    }
}

void Effect3DOutline::setOutlineWidth(float width)
{
    if(_outlineWidth != width)
    {
        _outlineWidth = width;
        _glProgramState->setUniformFloat("OutlineWidth", _outlineWidth);
    }
}

void Effect3DOutline::setTarget(EffectSprite3D *sprite)
{
    CCASSERT(nullptr != sprite && nullptr != sprite->getMesh(),"Error: Setting a null pointer or a null mesh EffectSprite3D to Effect3D");
    
    if(sprite != _sprite)
    {
        _sprite = sprite;
        
        auto mesh = sprite->getMesh();
        long offset = 0;
        for (auto i = 0; i < mesh->getMeshVertexAttribCount(); i++)
        {
            auto meshvertexattrib = mesh->getMeshVertexAttribute(i);
            
            _glProgramState->setVertexAttribPointer(s_attributeNames[meshvertexattrib.vertexAttrib],
                                                    meshvertexattrib.size,
                                                    meshvertexattrib.type,
                                                    GL_FALSE,
                                                    mesh->getVertexSizeInBytes(),
                                                    (void*)offset);
            offset += meshvertexattrib.attribSizeBytes;
        }
        
        Color4F color(_sprite->getDisplayedColor());
        color.a = _sprite->getDisplayedOpacity() / 255.0f;
        _glProgramState->setUniformVec4("u_color", Vec4(color.r, color.g, color.b, color.a));
    }
    
}

void Effect3DOutline::draw(const Mat4 &transform)
{
    //draw
    Color4F color(_sprite->getDisplayedColor());
    color.a = _sprite->getDisplayedOpacity() / 255.0f;
    _glProgramState->setUniformVec4("u_color", Vec4(color.r, color.g, color.b, color.a));
    if(_sprite && _sprite->getMesh())
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);
        
        auto mesh = _sprite->getMesh();
        glBindBuffer(GL_ARRAY_BUFFER, mesh->getVertexBuffer());
        _glProgramState->apply(transform);
        for (ssize_t i = 0; i < mesh->getSubMeshCount(); i++) {
            auto submesh = mesh->getSubMesh((int)i);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submesh->getIndexBuffer());
            glDrawElements((GLenum)submesh->getPrimitiveType(), (GLsizei)submesh->getIndexCount(), (GLenum)submesh->getIndexFormat(), 0);
            CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, submesh->getIndexCount());
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);
        glDisable(GL_CULL_FACE);
    }
}

void EffectSprite3D::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    for(auto &effect : _effects)
    {
        if(std::get<0>(effect) >=0)
            break;
        CustomCommand &cc = std::get<2>(effect);
        cc.func = CC_CALLBACK_0(Effect3D::draw,std::get<1>(effect),transform);
        renderer->addCommand(&cc);
        
    }
    
    if(!_defaultEffect)
    {
        Sprite3D::draw(renderer, transform, flags);
    }
    else
    {
        _command.init(_globalZOrder);
        _command.func = CC_CALLBACK_0(Effect3D::draw, _defaultEffect, transform);
        renderer->addCommand(&_command);
    }
    
    for(auto &effect : _effects)
    {
        if(std::get<0>(effect) <=0)
            continue;
        CustomCommand &cc = std::get<2>(effect);
        cc.func = CC_CALLBACK_0(Effect3D::draw,std::get<1>(effect),transform);
        renderer->addCommand(&cc);
        
    }
}

Sprite3DEffectTest::Sprite3DEffectTest()
{
    auto s = Director::getInstance()->getWinSize();
    addNewSpriteWithCoords( Vec2(s.width/2, s.height/2) );
    
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = CC_CALLBACK_2(Sprite3DEffectTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

std::string Sprite3DEffectTest::title() const
{
    return "Testing Sprite3D";
}
std::string Sprite3DEffectTest::subtitle() const
{
    return "Sprite3d with effects";
}

void Sprite3DEffectTest::addNewSpriteWithCoords(Vec2 p)
{
    //option 2: load obj and assign the texture
    auto sprite = EffectSprite3D::createFromObjFileAndTexture("Sprite3DTest/boss1.obj", "Sprite3DTest/boss.png");
    Effect3DOutline* effect = Effect3DOutline::create();
    effect->setOutlineColor(Vec3(1,0,0));
    effect->setOutlineWidth(0.01f);
    sprite->addEffect(effect, -1);
    Effect3DOutline* effect2 = Effect3DOutline::create();
    effect2->setOutlineWidth(0.02f);
    effect2->setOutlineColor(Vec3(1,1,0));
    sprite->addEffect(effect2, -2);
    //sprite->setEffect3D(effect);
    sprite->setScale(6.f);
    
    //add to scene
    addChild( sprite );
    
    sprite->setPosition( Vec2( p.x, p.y) );
    
    ActionInterval* action;
    float random = CCRANDOM_0_1();
    
    if( random < 0.20 )
        action = ScaleBy::create(3, 2);
    else if(random < 0.40)
        action = RotateBy::create(3, 360);
    else if( random < 0.60)
        action = Blink::create(1, 3);
    else if( random < 0.8 )
        action = TintBy::create(2, 0, -255, -255);
    else
        action = FadeOut::create(2);
    auto action_back = action->reverse();
    auto seq = Sequence::create( action, action_back, nullptr );
    
    sprite->runAction( RepeatForever::create(seq) );
}

void Sprite3DEffectTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    for (auto touch: touches)
    {
        auto location = touch->getLocation();
        
        addNewSpriteWithCoords( location );
    }
}

Sprite3DWithSkinTest::Sprite3DWithSkinTest()
{
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = CC_CALLBACK_2(Sprite3DWithSkinTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    auto s = Director::getInstance()->getWinSize();
    addNewSpriteWithCoords( Vec2(s.width/2, s.height/2) );
}
std::string Sprite3DWithSkinTest::title() const
{
    return "Testing Sprite3D";
}
std::string Sprite3DWithSkinTest::subtitle() const
{
    return "Tap screen to add more sprite3D";
}

void Sprite3DWithSkinTest::addNewSpriteWithCoords(Vec2 p)
{
    std::string fileName = "Sprite3DTest/orc.c3b";
    auto sprite = Sprite3D::create(fileName);
    sprite->setScale(3);
    sprite->setRotation3D(Vec3(0,180,0));
    addChild(sprite);
    sprite->setPosition( Vec2( p.x, p.y) );

    auto animation = Animation3D::create(fileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation);
        bool inverse = (std::rand() % 3 == 0);

        int rand2 = std::rand();
        float speed = 1.0f;
        if(rand2 % 3 == 1)
        {
            speed = animate->getSpeed() + CCRANDOM_0_1();
        }
        else if(rand2 % 3 == 2)
        {
            speed = animate->getSpeed() - 0.5 * CCRANDOM_0_1();
        }
        animate->setSpeed(inverse ? -speed : speed);

        sprite->runAction(RepeatForever::create(animate));
    }
}

void Sprite3DWithSkinTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    for (auto touch: touches)
    {
        auto location = touch->getLocation();
        
        addNewSpriteWithCoords( location );
    }
}

Animate3DTest::Animate3DTest()
: _hurt(nullptr)
, _swim(nullptr)
, _sprite(nullptr)
, _moveAction(nullptr)
, _transTime(0.1f)
, _elapseTransTime(0.f)
, _drawDebug(nullptr)
, _hasPick(false)
{
    addSprite3D();
    
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = CC_CALLBACK_2(Animate3DTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    scheduleUpdate();
}

Animate3DTest::~Animate3DTest()
{
    CC_SAFE_RELEASE(_moveAction);
    CC_SAFE_RELEASE(_hurt);
    CC_SAFE_RELEASE(_swim);
}

std::string Animate3DTest::title() const
{
    return "Testing Animate3D";
}

std::string Animate3DTest::subtitle() const
{
    return "Touch to beat the tortoise";
}

void Animate3DTest::update(float dt)
{
    if (_state == State::HURT_TO_SWIMMING)
    {
        _elapseTransTime += dt;
        float t = _elapseTransTime / _transTime;
        
        if (t >= 1.f)
        {
            t = 1.f;
            _sprite->stopAction(_hurt);
            _state = State::SWIMMING;
        }
        _swim->setWeight(t);
        _hurt->setWeight(1.f - t);
    }
    else if (_state == State::SWIMMING_TO_HURT)
    {
        _elapseTransTime += dt;
        float t = _elapseTransTime / _transTime;
        if (t >= 1.f)
        {
            t = 1.f;
            _state = State::HURT;
        }
        _swim->setWeight(1.f - t);
        _hurt->setWeight(t);
    }
    
    if (_drawDebug)
    {
        _drawDebug->clear();
        
        Mat4 mat = _sprite->getNodeToWorldTransform();
        mat.getRightVector(&_obb._xAxis);
        _obb._xAxis.normalize();
        
        mat.getUpVector(&_obb._yAxis);
        _obb._yAxis.normalize();
        
        mat.getForwardVector(&_obb._zAxis);
        _obb._zAxis.normalize();
        
        _obb._center = _sprite->getPosition3D();
        
        Vec3 corners[8] = {};
        _obb.getCorners(corners);
        _drawDebug->drawCube(corners, _hasPick?Color4F(1,0,0,1):Color4F(0,1,0,1));
    }
}

void Animate3DTest::addSprite3D()
{
    std::string fileName = "Sprite3DTest/tortoise.c3b";
    auto sprite = Sprite3D::create(fileName);
    sprite->setScale(0.1f);
    auto s = Director::getInstance()->getWinSize();
    sprite->setPosition(Vec2(s.width * 4.f / 5.f, s.height / 2.f));
    addChild(sprite);
    _sprite = sprite;
    auto animation = Animation3D::create(fileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation, 0.f, 1.933f);
        sprite->runAction(RepeatForever::create(animate));
        _swim = animate;
        _swim->retain();
        _hurt = Animate3D::create(animation, 1.933f, 2.8f);
        _hurt->retain();
        _state = State::SWIMMING;
    }
    
    _moveAction = MoveTo::create(4.f, Vec2(s.width / 5.f, s.height / 2.f));
    _moveAction->retain();
    auto seq = Sequence::create(_moveAction, CallFunc::create(CC_CALLBACK_0(Animate3DTest::reachEndCallBack, this)), nullptr);
    seq->setTag(100);
    sprite->runAction(seq);

    //Generate OBB by AABB
    Vec3 extents = Vec3(50, 25, 25);
    AABB aabb(-extents, extents);
    _obb = OBB(aabb);
    
    _drawDebug = DrawNode3D::create();
    addChild(_drawDebug);
}

void Animate3DTest::reachEndCallBack()
{
    _sprite->stopActionByTag(100);
    auto inverse = (MoveTo*)_moveAction->reverse();
    inverse->retain();
    _moveAction->release();
    _moveAction = inverse;
    auto rot = RotateBy::create(1.f, Vec3(0.f, 180.f, 0.f));
    auto seq = Sequence::create(rot, _moveAction, CallFunc::create(CC_CALLBACK_0(Animate3DTest::reachEndCallBack, this)), nullptr);
    seq->setTag(100);
    _sprite->runAction(seq);
}

void Animate3DTest::renewCallBack()
{
    _sprite->stopActionByTag(101);
    _state = State::HURT_TO_SWIMMING;
}

void Animate3DTest::unproject(const Mat4& viewProjection, const Size* viewport, Vec3* src, Vec3* dst)
{
    assert(dst);
    
    assert(viewport->width != 0.0f && viewport->height != 0.0f);
    Vec4 screen(src->x / viewport->width, ((viewport->height - src->y)) / viewport->height, src->z, 1.0f);
    
    screen.x = screen.x * 2.0f - 1.0f;
    screen.y = screen.y * 2.0f - 1.0f;
    screen.z = screen.z * 2.0f - 1.0f;
    
    viewProjection.getInversed().transformVector(screen, &screen);
    
    if (screen.w != 0.0f)
    {
        screen.x /= screen.w;
        screen.y /= screen.w;
        screen.z /= screen.w;
    }
    
    dst->set(screen.x, screen.y, screen.z);
}

void Animate3DTest::calculateRayByLocationInView(Ray* ray, const Vec2& location)
{
    auto dir = Director::getInstance();
    auto view = dir->getWinSize();
    Mat4 mat = dir->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    mat = dir->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    
    Vec3 src = Vec3(location.x, location.y, -1);
    Vec3 nearPoint;
    unproject(mat, &view, &src, &nearPoint);
    
    src = Vec3(location.x, location.y, 1);
    Vec3 farPoint;
    unproject(mat, &view, &src, &farPoint);
    
    Vec3 direction;
    Vec3::subtract(farPoint, nearPoint, &direction);
    direction.normalize();
    
    ray->_origin = nearPoint;
    ray->_direction = direction;
}

void Animate3DTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    for (auto touch: touches)
    {
        auto location = touch->getLocationInView();
        
        if (_sprite)
        {
            Ray ray;
            calculateRayByLocationInView(&ray,location);
            if(ray.intersects(_obb))
            {
                _hasPick = !_hasPick;
                //hurt the tortoise
                if (_state == State::SWIMMING)
                {
                    _sprite->runAction(_hurt);
                    auto delay = DelayTime::create(_hurt->getDuration() - 0.1f);
                    auto seq = Sequence::create(delay, CallFunc::create(CC_CALLBACK_0(Animate3DTest::renewCallBack, this)), nullptr);
                    seq->setTag(101);
                    _sprite->runAction(seq);
                    _state = State::SWIMMING_TO_HURT;
                }
                return;
            }
           
        }
    }
}


AttachmentTest::AttachmentTest()
: _hasWeapon(false)
, _sprite(nullptr)
{
    auto s = Director::getInstance()->getWinSize();
    addNewSpriteWithCoords( Vec2(s.width/2, s.height/2) );
    
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = CC_CALLBACK_2(AttachmentTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
std::string AttachmentTest::title() const
{
    return "Testing Sprite3D Attachment";
}
std::string AttachmentTest::subtitle() const
{
    return "touch to switch weapon";
}

void AttachmentTest::addNewSpriteWithCoords(Vec2 p)
{
    std::string fileName = "Sprite3DTest/orc.c3b";
    auto sprite = Sprite3D::create(fileName);
    sprite->setScale(5);
    sprite->setRotation3D(Vec3(0,180,0));
    addChild(sprite);
    sprite->setPosition( Vec2( p.x, p.y) );
    
    //test attach
    auto sp = Sprite3D::create("Sprite3DTest/axe.c3b");
    sprite->getAttachNode("Bip001 R Hand")->addChild(sp);
    
    auto animation = Animation3D::create(fileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation);
        
        sprite->runAction(RepeatForever::create(animate));
    }
    _sprite = sprite;
    _hasWeapon = true;
}

void AttachmentTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    if (_hasWeapon)
    {
        _sprite->removeAllAttachNode();
    }
    else
    {
        auto sp = Sprite3D::create("Sprite3DTest/axe.c3b");
        _sprite->getAttachNode("Bip001 R Hand")->addChild(sp);
    }
    _hasWeapon = !_hasWeapon;
}
Sprite3DWithSubMeshTest::Sprite3DWithSubMeshTest()
{
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = CC_CALLBACK_2(Sprite3DWithSubMeshTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    auto s = Director::getInstance()->getWinSize();
    addNewSpriteWithCoords( Vec2(s.width/2, s.height/2) );
}
std::string Sprite3DWithSubMeshTest::title() const
{
    return "Testing Sprite3D With SubMesh";
}
std::string Sprite3DWithSubMeshTest::subtitle() const
{
    return "Tap screen to add more sprite3D";
}
void Sprite3DWithSubMeshTest::addNewSpriteWithCoords(Vec2 p)
{
    std::string fileName = "Sprite3DTest/lanternfish.c3b";
    auto sprite = Sprite3D::create(fileName);
    sprite->setScale(2.f);
    addChild(sprite);
    sprite->setPosition( Vec2( p.x, p.y) );
    auto animation = Animation3D::create(fileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation);
        sprite->runAction(RepeatForever::create(animate));
    }
}
void Sprite3DWithSubMeshTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    for (auto touch: touches)
    {
        auto location = touch->getLocation();
        addNewSpriteWithCoords( location );
    }
}
Sprite3DWithCollisonTest::Sprite3DWithCollisonTest()
: _sprite1(nullptr)
, _sprite2(nullptr)
, _moveActionGo1(nullptr)
, _moveActionGo2(nullptr)
, _obb1(nullptr)
, _obb2(nullptr)
, _drawAABB1(nullptr)
, _drawAABB2(nullptr)
, _hasCollider(false)
, _HP(100)
, _Dir(1)
{
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = CC_CALLBACK_2(Sprite3DWithCollisonTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    auto s = Director::getInstance()->getWinSize();
    addNewSpriteWithCoords( Vec2(s.width/2, s.height/2) );
    scheduleUpdate();
}
std::string Sprite3DWithCollisonTest::title() const
{
    return "Testing Collison";
}
std::string Sprite3DWithCollisonTest::subtitle() const
{
    return "Collison With BoxCollider";
}
void Sprite3DWithCollisonTest::addNewSpriteWithCoords(Vec2 p)
{
    std::string fileName1 = "Sprite3DTest/lanternfish.c3b";
    auto sprite1 = Sprite3D::create(fileName1);
    sprite1->setScale(1.f);
    addChild(sprite1);
    sprite1->setPosition( Vec2(p.x, p.y / 2) );
    _sprite1 = sprite1;
    auto animation1 = Animation3D::create(fileName1);
    if (animation1)
    {
        auto animate1 = Animate3D::create(animation1);
        sprite1->runAction(RepeatForever::create(animate1));
    }
    _moveActionGo1 = MoveTo::create(4.f, Vec2(p.x, p.y + 50));
    _moveActionGo1->retain();
    auto seq = Sequence::create(_moveActionGo1, CallFunc::create(CC_CALLBACK_0(Sprite3DWithCollisonTest::reachEndCallBack, this)), nullptr);
    seq->setTag(101);
    sprite1->runAction(seq);

    Vec3 extents1 = Vec3(25, 25, 25);
    AABB aabb1(-extents1, extents1);

    CC_SAFE_DELETE(_obb1);
    _obb1 = new OBB(aabb1);
    _drawAABB1 = DrawNode3D::create();
    addChild(_drawAABB1);

    std::string fileName2 = "Sprite3DTest/tortoise.c3b";
    auto sprite2 = Sprite3D::create(fileName2);
    sprite2->setScale(0.06f);
    sprite2->setRotation3D(Vec3(0,180,0));
    addChild(sprite2);
    sprite2->setPosition( Vec2( p.x / 3.5f, p.y) );
    _sprite2 = sprite2;
    auto animation2 = Animation3D::create(fileName2);
    if (animation2)
    {
        auto animate2 = Animate3D::create(animation2, 0.f, 1.933f);
        sprite2->runAction(RepeatForever::create(animate2));
    }

    Vec3 extents2 = Vec3(25, 25, 25);
    AABB aabb2(-extents2, extents2);
    CC_SAFE_DELETE(_obb2);
    _obb2 = new OBB(aabb2);
    _drawAABB2 = DrawNode3D::create();
    addChild(_drawAABB2);

    TTFConfig ttfHP("fonts/arial.ttf", 10);
    _labelHPPos = Label::createWithTTF(ttfHP,"tortoise HP:100");
    Vec2 tAnchor(0,0);
    _labelHPPos->setAnchorPoint(tAnchor);
    _labelHPPos->setPosition(10,260);
    addChild(_labelHPPos);
}
void Sprite3DWithCollisonTest::update(float dt)
{
    char szText[100];
    sprintf(szText,"tortoise HP:%d",_HP);
    std::string str = szText;
    _labelHPPos->setString(str);

    if (_obb1 && _drawAABB1)
    {
        _drawAABB1->clear();
        Mat4 mat1 = _sprite1->getNodeToWorldTransform();
        mat1.getRightVector(&_obb1->_xAxis);
        _obb1->_xAxis.normalize();
        mat1.getUpVector(&_obb1->_yAxis);
        _obb1->_yAxis.normalize();
        mat1.getForwardVector(&_obb1->_zAxis);
        _obb1->_zAxis.normalize();
        _obb1->_center = _sprite1->getPosition3D();
        Vec3 corners1[8] = {};
        _obb1->getCorners(corners1);
        _drawAABB1->drawCube(corners1, _hasCollider?Color4F(1,0,0,1):Color4F(0,1,0,1));
    }
    if (_obb2 && _drawAABB2)
    {
        _drawAABB2->clear();
        Mat4 mat2 = _sprite2->getNodeToWorldTransform();
        mat2.getRightVector(&_obb2->_xAxis);
        _obb2->_xAxis.normalize();
        mat2.getUpVector(&_obb2->_yAxis);
        _obb2->_yAxis.normalize();
        mat2.getForwardVector(&_obb2->_zAxis);
        _obb2->_zAxis.normalize();
        _obb2->_center = _sprite2->getPosition3D();
        Vec3 corners2[8] = {};
        _obb2->getCorners(corners2);
        _drawAABB2->drawCube(corners2, _hasCollider?Color4F(1,0,0,1):Color4F(0,1,0,1));
    }
    bool iscollider  = _obb1->intersects(*_obb2);
    if(iscollider)
    {
        if(!_hasCollider)
        {
            if(_HP >0)
                _HP -= 10;
            //_sprite2->stopActionByTag(100);
            _hasCollider = true;
        }
    }
    else
    {
        if(_hasCollider)
            _hasCollider = false;
    }
}

void Sprite3DWithCollisonTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    for (auto touch: touches)
    {
        auto location = touch->getLocation();
        if(_sprite2)
        {
            if(_HP > 0)
            {
                _moveActionGo2 = MoveTo::create(1.f, location);
                _moveActionGo2->retain();
               
                if((_Dir == 1 && _sprite2->getPositionX() < location.x) || (_Dir == 2 && _sprite2->getPositionX() > location.x))
                {
                      auto seq = Sequence::create(_moveActionGo2, nullptr);
                      seq->setTag(100);
                      _sprite2->runAction(seq);
                }
                else if(_Dir == 1 && _sprite2->getPositionX() > location.x)
                {
                    auto rot = RotateBy::create(0.5f, Vec3(0.f, 180.f, 0.f));
                    auto seq = Sequence::create(rot, _moveActionGo2, nullptr);
                    seq->setTag(100);
                    _sprite2->runAction(seq);
                    _Dir = 2;
                }
                else if(_Dir == 2 && _sprite2->getPositionX() < location.x)
                {
                    auto rot = RotateBy::create(0.5f, Vec3(0.f, 180.f, 0.f));
                    auto seq = Sequence::create(rot, _moveActionGo2, nullptr);
                    seq->setTag(100);
                    _sprite2->runAction(seq);
                    _Dir = 1;
                }
              
            }
            else
                return;
        }
    }
}

void Sprite3DWithCollisonTest::reachEndCallBack()
{
    _sprite1->stopActionByTag(101);
    auto inverse = (MoveTo*)_moveActionGo1->reverse();
    inverse->retain();
    _moveActionGo1->release();
    _moveActionGo1 = inverse;
    auto seq = Sequence::create(_moveActionGo1, CallFunc::create(CC_CALLBACK_0(Sprite3DWithCollisonTest::reachEndCallBack, this)), nullptr);
    seq->setTag(101);
    _sprite1->runAction(seq);
}

Sprite3DWithOBBPerfromanceTest::Sprite3DWithOBBPerfromanceTest()
{
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = CC_CALLBACK_2(Sprite3DWithOBBPerfromanceTest::onTouchesBegan, this);
    listener->onTouchesEnded = CC_CALLBACK_2(Sprite3DWithOBBPerfromanceTest::onTouchesEnded, this);
    listener->onTouchesMoved = CC_CALLBACK_2(Sprite3DWithOBBPerfromanceTest::onTouchesMoved, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    auto s = Director::getInstance()->getWinSize();
    initDrawBox();

    addNewSpriteWithCoords(Vec2(s.width/2, s.height/2));
    MenuItemFont::setFontName("fonts/arial.ttf");
    MenuItemFont::setFontSize(65);
    auto decrease = MenuItemFont::create(" - ", CC_CALLBACK_1(Sprite3DWithOBBPerfromanceTest::delOBBCallback, this));
    decrease->setColor(Color3B(0,200,20));
    auto increase = MenuItemFont::create(" + ", CC_CALLBACK_1(Sprite3DWithOBBPerfromanceTest::addOBBCallback, this));
    increase->setColor(Color3B(0,200,20));

    auto menu = Menu::create(decrease, increase, nullptr);
    menu->alignItemsHorizontally();
    menu->setPosition(Vec2(s.width/2, s.height-65));
    addChild(menu, 1);

    TTFConfig ttfCount("fonts/Marker Felt.ttf", 30);
    _labelCubeCount = Label::createWithTTF(ttfCount,"0 cubes");
    _labelCubeCount->setColor(Color3B(0,200,20));
    _labelCubeCount->setPosition(Vec2(s.width/2, s.height-90));
    addChild(_labelCubeCount);
    _hasCollider = false;
    scheduleUpdate();
}
std::string Sprite3DWithOBBPerfromanceTest::title() const
{
    return "OBB Collison Perfromance Test";
}
std::string Sprite3DWithOBBPerfromanceTest::subtitle() const
{
    return "";
}
void Sprite3DWithOBBPerfromanceTest::addNewOBBWithCoords(Vec2 p)
{
     Vec3 extents = Vec3(10, 10, 10);
     AABB aabb(-extents, extents);
     auto obb = OBB(aabb);
     obb._center = Vec3(p.x,p.y,0);
     _obb.push_back(obb);
}

void Sprite3DWithOBBPerfromanceTest::onTouchesBegan(const std::vector<Touch*>& touches, Event* event)
{
    for (auto touch: touches)
    {
        auto location = touch->getLocationInView();

        if(_obb.size() > 0)
        {
            _intersetList.clear();
            Ray ray;
            calculateRayByLocationInView(&ray,location);
            for(int i = 0; i < _obb.size(); i++)
            {
                if(ray.intersects(_obb[i]))
                {
                   _intersetList.insert(i);
                   return;
                }
            }
        }
    }
}

void Sprite3DWithOBBPerfromanceTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{

}

void Sprite3DWithOBBPerfromanceTest::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
    for (auto touch: touches)
    {
        auto location = touch->getLocation();

        for(int i = 0; i < _obb.size(); i++)
        {
            if(_intersetList.find(i) != _intersetList.end())
                _obb[i]._center = Vec3(location.x,location.y,0);
        }
    }
}

void Sprite3DWithOBBPerfromanceTest::update(float dt)
{
    char szText[16];
    sprintf(szText,"%u cubes",_obb.size());
    _labelCubeCount->setString(szText);

    if (_drawDebug)
    {
        _drawDebug->clear();
        
        Mat4 mat = _sprite->getNodeToWorldTransform();
        mat.getRightVector(&_obbt._xAxis);
        _obbt._xAxis.normalize();
        
        mat.getUpVector(&_obbt._yAxis);
        _obbt._yAxis.normalize();
        
        mat.getForwardVector(&_obbt._zAxis);
        _obbt._zAxis.normalize();
        
        _obbt._center = _sprite->getPosition3D();
        
        Vec3 corners[8] = {};
        _obbt.getCorners(corners);
        _drawDebug->drawCube(corners, Color4F(0,0,1,1));
    }
    if(_obb.size() > 0)
    {
        _drawOBB->clear();
        for(int i =0; i < _obb.size(); i++)
        {           
           Vec3 corners[8] = {};
           _obb[i].getCorners(corners);
           _drawOBB->drawCube(corners, _obbt.intersects(_obb[i])?Color4F(1,0,0,1):Color4F(0,1,0,1));
        }
    }
}

void Sprite3DWithOBBPerfromanceTest::initDrawBox()
{
    _drawOBB = DrawNode3D::create();
    addChild(_drawOBB);
}

void Sprite3DWithOBBPerfromanceTest::addNewSpriteWithCoords(Vec2 p)
{
    std::string fileName = "Sprite3DTest/tortoise.c3b";
    auto sprite = Sprite3D::create(fileName);
    sprite->setScale(0.1f);
    auto s = Director::getInstance()->getWinSize();
    sprite->setPosition(Vec2(s.width * 4.f / 5.f, s.height / 2.f));
    addChild(sprite);
    _sprite = sprite;
    auto animation = Animation3D::create(fileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation, 0.f, 1.933f);
        sprite->runAction(RepeatForever::create(animate));
    }
    
    _moveAction = MoveTo::create(4.f, Vec2(s.width / 5.f, s.height / 2.f));
    _moveAction->retain();
    auto seq = Sequence::create(_moveAction, CallFunc::create(CC_CALLBACK_0(Sprite3DWithOBBPerfromanceTest::reachEndCallBack, this)), nullptr);
    seq->setTag(100);
    sprite->runAction(seq);

    //Generate OBB by AABB
    Vec3 extents = Vec3(50, 25, 25);
    AABB aabb(-extents, extents);
    _obbt = OBB(aabb);
    
    _drawDebug = DrawNode3D::create();
    addChild(_drawDebug);
}

void Sprite3DWithOBBPerfromanceTest::reachEndCallBack()
{
    _sprite->stopActionByTag(100);
    auto inverse = (MoveTo*)_moveAction->reverse();
    inverse->retain();
    _moveAction->release();
    _moveAction = inverse;
    auto rot = RotateBy::create(1.0f, Vec3(0.f, 180.f, 0.f));
    auto seq = Sequence::create(rot, _moveAction, CallFunc::create(CC_CALLBACK_0(Sprite3DWithOBBPerfromanceTest::reachEndCallBack, this)), nullptr);
    seq->setTag(100);
    _sprite->runAction(seq);
}

void Sprite3DWithOBBPerfromanceTest::addOBBCallback(Ref* sender)
{
    addOBBWithCount(10);
}

void Sprite3DWithOBBPerfromanceTest::addOBBWithCount(float value)
{
    for(int i = 0; i < value; i++)
    {
        Vec2 randompos = Vec2(CCRANDOM_0_1() * Director::getInstance()->getWinSize().width,CCRANDOM_0_1() * Director::getInstance()->getWinSize().height);
        Vec3 extents = Vec3(10, 10, 10);
        AABB aabb(-extents, extents);
        auto obb = OBB(aabb);
        obb._center = Vec3(randompos.x,randompos.y,0);
        _obb.push_back(obb);
    }
}

void Sprite3DWithOBBPerfromanceTest::delOBBCallback(Ref* sender)
{
    delOBBWithCount(10);
}

void Sprite3DWithOBBPerfromanceTest::delOBBWithCount(float value)
{
    if(_obb.size() >= 10)
    {
        _obb.erase(_obb.begin(),_obb.begin() + value);
        _drawOBB->clear();
    }
    else
        return;
}
void Sprite3DWithOBBPerfromanceTest::unproject(const Mat4& viewProjection, const Size* viewport, Vec3* src, Vec3* dst)
{
    assert(dst);
    
    assert(viewport->width != 0.0f && viewport->height != 0.0f);
    Vec4 screen(src->x / viewport->width, ((viewport->height - src->y)) / viewport->height, src->z, 1.0f);
    
    screen.x = screen.x * 2.0f - 1.0f;
    screen.y = screen.y * 2.0f - 1.0f;
    screen.z = screen.z * 2.0f - 1.0f;
    
    viewProjection.getInversed().transformVector(screen, &screen);
    
    if (screen.w != 0.0f)
    {
        screen.x /= screen.w;
        screen.y /= screen.w;
        screen.z /= screen.w;
    }
    
    dst->set(screen.x, screen.y, screen.z);
}

void Sprite3DWithOBBPerfromanceTest::calculateRayByLocationInView(Ray* ray, const Vec2& location)
{
    auto dir = Director::getInstance();
    auto view = dir->getWinSize();
    Mat4 mat = dir->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    mat = dir->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    
    Vec3 src = Vec3(location.x, location.y, -1);
    Vec3 nearPoint;
    unproject(mat, &view, &src, &nearPoint);
    
    src = Vec3(location.x, location.y, 1);
    Vec3 farPoint;
    unproject(mat, &view, &src, &farPoint);
    
    Vec3 direction;
    Vec3::subtract(farPoint, nearPoint, &direction);
    direction.normalize();
    
    ray->_origin = nearPoint;
    ray->_direction = direction;
}

Sprite3DWithAABBPerfromanceTest::Sprite3DWithAABBPerfromanceTest()
{
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = CC_CALLBACK_2(Sprite3DWithAABBPerfromanceTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    auto s = Director::getInstance()->getWinSize();
    initDrawBox();

    MenuItemFont::setFontName("fonts/arial.ttf");
    MenuItemFont::setFontSize(65);
    auto decrease = MenuItemFont::create(" - ", CC_CALLBACK_1(Sprite3DWithAABBPerfromanceTest::delAABBCallback, this));
    decrease->setColor(Color3B(0,200,20));
    auto increase = MenuItemFont::create(" + ", CC_CALLBACK_1(Sprite3DWithAABBPerfromanceTest::addAABBCallback, this));
    increase->setColor(Color3B(0,200,20));

    auto menu = Menu::create(decrease, increase, nullptr);
    menu->alignItemsHorizontally();
    menu->setPosition(Vec2(s.width/2, s.height-65));
    addChild(menu, 1);

    TTFConfig ttfCount("fonts/Marker Felt.ttf", 30);
    _labelCubeCount = Label::createWithTTF(ttfCount,"0 cubes");
    _labelCubeCount->setColor(Color3B(0,200,20));
    _labelCubeCount->setPosition(Vec2(s.width/2, s.height-90));
    addChild(_labelCubeCount);
    scheduleUpdate();
}

std::string Sprite3DWithAABBPerfromanceTest::title() const
{
    return "AABB Collison Perfromance Test";
}

std::string Sprite3DWithAABBPerfromanceTest::subtitle() const
{
    return "";
}

void Sprite3DWithAABBPerfromanceTest::initDrawBox()
{
    _drawAABB = DrawNode3D::create();
    addChild(_drawAABB);
}

void Sprite3DWithAABBPerfromanceTest::addNewAABBWithCoords(Vec2 p)
{
     Vec3 extents = Vec3(10, 10, 10);
     AABB aabb(-extents, extents);
     aabb._min = Vec3(p.x - extents.x, p.y - extents.y, -extents.z);
     aabb._max = Vec3(p.x + extents.x, p.y + extents.y, extents.z);
     _aabb.push_back(aabb);
}
void Sprite3DWithAABBPerfromanceTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    for (auto touch: touches)
    {
        auto location = touch->getLocation();
    }
}

void Sprite3DWithAABBPerfromanceTest::update(float dt)
{
    char szText[16];
    sprintf(szText,"%u cubes",_aabb.size());
    _labelCubeCount->setString(szText);
    std::set<int> intersetList;

    if(_aabb.size() > 0)
    {
        _drawAABB->clear();
        for(int i = 0; i < _aabb.size(); i++)
        {
            auto aabb = _aabb[i];
            for(int j = 0; j < _aabb.size(); j++)
            {
                if(i != j)
                {
                    if(aabb.intersects(_aabb[j]))
                    {
                        intersetList.insert(i);
                        intersetList.insert(j);
                    }
                }                          
            }
        }

        for(int i = 0; i < _aabb.size(); i++)
        {
            Vec3 corners[8] = {};
            _aabb[i].getCorners(corners);
            if(intersetList.find(i) != intersetList.end())
            {
                _drawAABB->drawCube(corners, Color4F(1,0,0,1));
            }
            else
            {
                _drawAABB->drawCube(corners, Color4F(0,1,0,1));
            }
        }
    }
}

void Sprite3DWithAABBPerfromanceTest::addAABBCallback(Ref* sender)
{
    addAABBWithCount(10);
}

void Sprite3DWithAABBPerfromanceTest::addAABBWithCount(float value)
{
    for(int i = 0; i < value; i++)
    {
        Vec2 randompos = Vec2(CCRANDOM_0_1() * Director::getInstance()->getWinSize().width,CCRANDOM_0_1() * Director::getInstance()->getWinSize().height);
        Vec3 extents = Vec3(10, 10, 10);
        AABB aabb(-extents, extents);
        aabb._min = Vec3(randompos.x - extents.x, randompos.y - extents.y, -extents.z);
        aabb._max = Vec3(randompos.x + extents.x, randompos.y + extents.y, extents.z);
        _aabb.push_back(aabb);
    }
}

void Sprite3DWithAABBPerfromanceTest::delAABBCallback(Ref* sender)
{
    delAABBWithCount(10);
}

void Sprite3DWithAABBPerfromanceTest::delAABBWithCount(float value)
{
   if(_aabb.size() >= 10)
    {
        _aabb.erase(_aabb.begin(),_aabb.begin() + value);
        _drawAABB->clear();
    }
    else
        return;
}

BillBoardTest::BillBoardTest()
: _billborad(nullptr)
, _billboradAni(nullptr)
, _camera(nullptr)
{
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesMoved = CC_CALLBACK_2(BillBoardTest::onTouchesMoved, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    auto layer3D=Layer::create();
    addChild(layer3D,0);
    _layerBillBorad=layer3D;
    DrawNode3D* line =DrawNode3D::create();
    auto s = Director::getInstance()->getWinSize();
    if (_camera == nullptr)
    {
        _camera=Camera3D::createPerspective(60, (GLfloat)s.width/s.height, 1, 500);
        _camera->setCameraFlag(CameraFlag::CAMERA_USER1);
        Camera3D::addCamera(_camera);
    }
    _layerBillBorad->addChild(_camera);
    addNewBillBoradWithCoords(Vec3(20,0,0));
    addNewAniBillBoradWithCoords(Vec3(-20,0,0));
    _camera->setPosition3D(Vec3(0, 130, 130));
    _camera->lookAt(Vec3(0,0,0), Vec3(0,1,0));

    for( int j =-20; j<=20 ;j++)
    {
        line->drawLine(Vec3(-100, 0, 5*j),Vec3(100,0,5*j),Color4F(0,1,0,1));
    }
    //draw z
    for( int j =-20; j<=20 ;j++)
    {
        line->drawLine(Vec3(5*j, 0, -100),Vec3(5*j,0,100),Color4F(0,1,0,1));
    }
    //draw y
    //line->drawLine(Vec3(0, -50, 0),Vec3(0,0,0),Color4F(0,0.5,0,1));
    //line->drawLine(Vec3(0, 0, 0),Vec3(0,50,0),Color4F(0,1,0,1));
    _layerBillBorad->addChild(line,1);

    TTFConfig ttfConfig("fonts/arial.ttf", 20);
    auto label1 = Label::createWithTTF(ttfConfig,"rotate+");
    auto menuItem1 = MenuItemLabel::create(label1, CC_CALLBACK_1(BillBoardTest::rotateCameraCallback,this,10));
    auto label2 = Label::createWithTTF(ttfConfig,"rotate-");
    auto menuItem2 = MenuItemLabel::create(label2, CC_CALLBACK_1(BillBoardTest::rotateCameraCallback,this,-10));
    auto menu = Menu::create(menuItem1,menuItem2,NULL);
    menu->setPosition(Vec2::ZERO);
    menuItem1->setPosition( Vec2( s.width-50, VisibleRect::top().y-150) );
    menuItem2->setPosition( Vec2( s.width-50, VisibleRect::top().y-200) );
    addChild(menu, 0);
    _layerBillBorad->setCameraMask(2);
}
BillBoardTest::~BillBoardTest()
{
    if (_camera)
    {
        Camera3D::removeCamera(_camera);
        _camera = nullptr;
    }
}
std::string BillBoardTest::title() const
{
    return "Testing BillBoard";
}
std::string BillBoardTest::subtitle() const
{
    return "";
}
void BillBoardTest::addNewBillBoradWithCoords(Vec3 p)
{
    _billborad = BillBorad::create("Images/Icon.png");
    _billborad->setScale(0.5f);
    _layerBillBorad->addChild(_billborad,10);
    _billborad->setPosition(Vec2( p.x,p.y ));

    auto billborad1 = BillBorad::create("Images/Icon.png");
    billborad1->setScale(0.5f);
    _layerBillBorad->addChild(billborad1,10);
    billborad1->setPosition3D(Vec3(p.x,p.y,p.z - 130));

    auto billborad2 = BillBorad::create("Images/Icon.png");
    billborad2->setScale(0.5f);
    _layerBillBorad->addChild(billborad2,10);
    billborad2->setPosition3D(Vec3(p.x,p.y,p.z + 50));

    auto billborad3 = BillBorad::create("Images/Icon.png");
    billborad3->setScale(0.5f);
    _layerBillBorad->addChild(billborad3,10);
    billborad3->setPosition3D(Vec3(p.x,p.y,p.z + 90));

    auto billborad4 = BillBorad::create("Images/Icon.png");
    billborad4->setScale(0.5f);
    _layerBillBorad->addChild(billborad4,10);
    billborad4->setPosition3D(Vec3(p.x,p.y,p.z - 60));


}
void BillBoardTest::addNewAniBillBoradWithCoords(Vec3 p)
{

    auto billboradAni1 = BillBorad::create("Images/grossini.png");
    billboradAni1->setScale(0.5f);
    billboradAni1->setPosition3D(Vec3(p.x, p.y, p.z - 130));
    _layerBillBorad->addChild(billboradAni1,10);
    
    auto animation1 = Animation::create();
    for( int i=1;i<15;i++)
    {
        char szName1[100] = {0};
        sprintf(szName1, "Images/grossini_dance_%02d.png", i);
        animation1->addSpriteFrameWithFile(szName1);
    }
    // should last 2.8 seconds. And there are 14 frames.
    animation1->setDelayPerUnit(2.8f / 14.0f);
    animation1->setRestoreOriginalFrame(true);

    auto action1 = Animate::create(animation1);
    billboradAni1->runAction(RepeatForever::create(action1));

    auto billboradAni2 = BillBorad::create("Images/grossini.png");
    billboradAni2->setScale(0.5f);
    billboradAni2->setPosition3D(Vec3(p.x, p.y, p.z - 60));
    _layerBillBorad->addChild(billboradAni2,10);
    
    auto animation2 = Animation::create();
    for( int i=1;i<15;i++)
    {
        char szName2[100] = {0};
        sprintf(szName2, "Images/grossini_dance_%02d.png", i);
        animation2->addSpriteFrameWithFile(szName2);
    }
    // should last 2.8 seconds. And there are 14 frames.
    animation2->setDelayPerUnit(2.8f / 14.0f);
    animation2->setRestoreOriginalFrame(true);

    auto action2 = Animate::create(animation2);
    billboradAni2->runAction(RepeatForever::create(action2));

    _billboradAni = BillBorad::create("Images/grossini.png");
    _billboradAni->setScale(0.5f);
    _billboradAni->setPosition3D( p );
    _layerBillBorad->addChild(_billboradAni,10);
    
    auto animation = Animation::create();
    for( int i=1;i<15;i++)
    {
        char szName[100] = {0};
        sprintf(szName, "Images/grossini_dance_%02d.png", i);
        animation->addSpriteFrameWithFile(szName);
    }
    // should last 2.8 seconds. And there are 14 frames.
    animation->setDelayPerUnit(2.8f / 14.0f);
    animation->setRestoreOriginalFrame(true);

    auto action = Animate::create(animation);
    _billboradAni->runAction(RepeatForever::create(action));

    auto billboradAni3 = BillBorad::create("Images/grossini.png");
    billboradAni3->setScale(0.5f);
    billboradAni3->setPosition3D(Vec3(p.x, p.y, p.z + 50));
    _layerBillBorad->addChild(billboradAni3,10);
    
    auto animation3 = Animation::create();
    for( int i=1;i<15;i++)
    {
        char szName3[100] = {0};
        sprintf(szName3, "Images/grossini_dance_%02d.png", i);
        animation3->addSpriteFrameWithFile(szName3);
    }
    // should last 2.8 seconds. And there are 14 frames.
    animation3->setDelayPerUnit(2.8f / 14.0f);
    animation3->setRestoreOriginalFrame(true);

    auto action3 = Animate::create(animation3);
    billboradAni3->runAction(RepeatForever::create(action3));

    auto billboradAni4 = BillBorad::create("Images/grossini.png");
    billboradAni4->setScale(0.5f);
    billboradAni4->setPosition3D(Vec3(p.x, p.y, p.z + 90));
    _layerBillBorad->addChild(billboradAni4,10);
    
    auto animation4 = Animation::create();
    for( int i=1;i<15;i++)
    {
        char szName4[100] = {0};
        sprintf(szName4, "Images/grossini_dance_%02d.png", i);
        animation4->addSpriteFrameWithFile(szName4);
    }
    // should last 2.8 seconds. And there are 14 frames.
    animation4->setDelayPerUnit(2.8f / 14.0f);
    animation4->setRestoreOriginalFrame(true);

    auto action4 = Animate::create(animation4);
    billboradAni4->runAction(RepeatForever::create(action4));
}
void BillBoardTest::update(float dt)
{

}
void BillBoardTest::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
    if(touches.size()==1)
    {
        auto touch = touches[0];
        auto location = touch->getLocation();
        auto PreviousLocation = touch->getPreviousLocation();
        Point newPos = PreviousLocation - location;

        Vec3 cameraDir;
        Vec3 cameraRightDir;
        _camera->getNodeToWorldTransform().getForwardVector(&cameraDir);
        cameraDir.normalize();
        cameraDir.y=0;
        _camera->getNodeToWorldTransform().getRightVector(&cameraRightDir);
        cameraRightDir.normalize();
        cameraRightDir.y=0;
        Vec3 cameraPos=  _camera->getPosition3D();
        cameraPos+=cameraDir*newPos.y*0.1;
        cameraPos+=cameraRightDir*newPos.x*0.1;
        _camera->setPosition3D(cameraPos);      
    }
}
void BillBoardTest::rotateCameraCallback(Ref* sender,float value)
{
    Vec3  rotation3D= _camera->getRotation3D();
    rotation3D.y+= value;
    _camera->setRotation3D(rotation3D);
}