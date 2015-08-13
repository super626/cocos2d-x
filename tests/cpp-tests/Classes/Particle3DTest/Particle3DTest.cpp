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

#include "Particle3DTest.h"
#include "Particle3D/CCParticleSystem3D.h"
#include "Particle3D/PU/CCPUParticleSystem3D.h"
#include "Particle3D/PU/CCPURender.h"

USING_NS_CC;

#define PARTICLE_SYSTEM_TAG 0x0001

Particle3DTests::Particle3DTests()
{
    ADD_TEST_CASE(Particle3DExplosionSystemDemo);
    ADD_TEST_CASE(Particle3DLineStreakDemo);
    ADD_TEST_CASE(Particle3DBlackHoleDemo);
    ADD_TEST_CASE(Particle3DHypnoDemo);
    ADD_TEST_CASE(Particle3DAdvancedLodSystemDemo);
    ADD_TEST_CASE(Particle3DTimeShiftDemo);
    ADD_TEST_CASE(Particle3DUVAnimDemo);
    ADD_TEST_CASE(Particle3DFirePlaceDemo);
    ADD_TEST_CASE(Particle3DElectricBeamSystemDemo);
    ADD_TEST_CASE(Particle3DFlareShieldDemo);
    ADD_TEST_CASE(Particle3DLightningBoltDemo);
    ADD_TEST_CASE(Particle3DCanOfWormsDemo);
    ADD_TEST_CASE(Particle3DRibbonTrailDemo);
    ADD_TEST_CASE(Particle3DWeaponTrailDemo);
    ADD_TEST_CASE(Particle3DWithSprite3DDemo);
    ADD_TEST_CASE(Particle3DCustomDemo);
}

std::string Particle3DTestDemo::title() const 
{
    return "Particle3D Test";
}

bool Particle3DTestDemo::init()
{
    if (!TestCase::init()) return false;

    FileUtils::getInstance()->addSearchPath("Particle3D/materials");
    FileUtils::getInstance()->addSearchPath("Particle3D/scripts");
    FileUtils::getInstance()->addSearchPath("Sprite3DTest");
    //FileUtils::getInstance()->addSearchPath("Particle3D/textures");

    Size size = Director::getInstance()->getWinSize();
    _camera = Camera::createPerspective(30.0f, size.width / size.height, 1.0f, 1000.0f);
    _camera->setPosition3D(Vec3(0.0f, 0.0f, 100.0f));
    _camera->lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
    _camera->setCameraFlag(CameraFlag::USER1);
    this->addChild(_camera);

    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = CC_CALLBACK_2(Particle3DTestDemo::onTouchesBegan, this);
    listener->onTouchesMoved = CC_CALLBACK_2(Particle3DTestDemo::onTouchesMoved, this);
    listener->onTouchesEnded = CC_CALLBACK_2(Particle3DTestDemo::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


    TTFConfig config("fonts/tahoma.ttf",10);
    _particleLab = Label::createWithTTF(config,"Particle Count: 0",TextHAlignment::LEFT);
    _particleLab->retain();
    _particleLab->setPosition(Vec2(0.0f, size.height / 6.0f));
    _particleLab->setAnchorPoint(Vec2(0.0f, 0.0f));
    this->addChild(_particleLab);
    
    scheduleUpdate();
    return true;
}

void Particle3DTestDemo::onTouchesBegan(const std::vector<Touch*>& touches, cocos2d::Event  *event)
{
    
}

void Particle3DTestDemo::onTouchesMoved(const std::vector<Touch*>& touches, cocos2d::Event  *event)
{
    if (touches.size())
    {
        auto touch = touches[0];
        auto delta = touch->getDelta();
        
        _angle -= CC_DEGREES_TO_RADIANS(delta.x);
        _camera->setPosition3D(Vec3(100.0f * sinf(_angle), 0.0f, 100.0f * cosf(_angle)));
        _camera->lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
    }
}

void Particle3DTestDemo::onTouchesEnded(const std::vector<Touch*>& touches, cocos2d::Event  *event)
{
    
}

Particle3DTestDemo::Particle3DTestDemo( void )
: _angle(0.0f)
{

}

void Particle3DTestDemo::update( float delta )
{
    for (auto ps : this->getChildren()){
        if (ps->getTag() == PARTICLE_SYSTEM_TAG){
            unsigned int count = 0;
            auto children = ps->getChildren();
            for (auto iter : children){
                ParticleSystem3D *child = dynamic_cast<ParticleSystem3D *>(iter);
                if (child){
                    count += child->getAliveParticleCount();
                }
            }

            char str[128];
            sprintf(str, "Particle Count: %d", count);
            _particleLab->setString(str);
        }
    }
}

Particle3DTestDemo::~Particle3DTestDemo( void )
{
    _particleLab->release();
}

std::string Particle3DAdvancedLodSystemDemo::subtitle() const 
{
    return "AdvancedSystem";
}

bool Particle3DAdvancedLodSystemDemo::init()
{
    if (!Particle3DTestDemo::init()) 
        return false;

    auto rootps = PUParticleSystem3D::create("advancedLodSystem.pu");
    rootps->setCameraMask((unsigned short)CameraFlag::USER1);

    auto scale = ScaleBy::create(1.0f, 2.0f, 2.0f, 2.0f);
    auto rotate = RotateBy::create(1.0f, Vec3(0.0f, 0.0f, 100.0f));
    rootps->runAction(RepeatForever::create(Sequence::create(rotate, nullptr)));
    rootps->runAction(RepeatForever::create(Sequence::create(scale, scale->reverse(), nullptr)));
    rootps->startParticleSystem();


    this->addChild(rootps, 0, PARTICLE_SYSTEM_TAG);

    return true;
}

std::string Particle3DBlackHoleDemo::subtitle() const 
{
    return "BlackHole";
}

bool Particle3DBlackHoleDemo::init()
{
    if (!Particle3DTestDemo::init()) 
        return false;

    auto rootps = PUParticleSystem3D::create("blackHole.pu", "pu_mediapack_01.material");
    rootps->setCameraMask((unsigned short)CameraFlag::USER1);
    rootps->setPosition(-25.0f, 0.0f);
    auto moveby = MoveBy::create(2.0f, Vec2(50.0f, 0.0f));
    auto moveby1 = MoveBy::create(2.0f, Vec2(-50.0f, 0.0f));
//    auto scale = ScaleBy::create(1.0f, 2.0f, 2.0f, 2.0f);
//    auto rotate = RotateBy::create(1.0f, Vec3(100.0f, 100.0f, 100.0f));
    rootps->runAction(RepeatForever::create(Sequence::create(moveby, moveby1, nullptr)));
    //rootps->runAction(RepeatForever::create(Sequence::create(scale, scale->reverse(), nullptr)));
    //rootps->runAction(RepeatForever::create(Sequence::create(rotate, nullptr)));
    rootps->startParticleSystem();

    this->addChild(rootps, 0, PARTICLE_SYSTEM_TAG);

    return true;
}

std::string Particle3DHypnoDemo::subtitle() const 
{
    return "Hypno";
}

bool Particle3DHypnoDemo::init()
{
    if (!Particle3DTestDemo::init()) 
        return false;

    auto rootps = PUParticleSystem3D::create("hypno.pu", "pu_mediapack_01.material");
    rootps->setCameraMask((unsigned short)CameraFlag::USER1);
//    auto scale = ScaleBy::create(1.0f, 2.0f, 2.0f, 2.0f);
//    auto rotate = RotateBy::create(1.0f, Vec3(0.0, 100.0f, 0.0f));
    //rootps->runAction(RepeatForever::create(Sequence::create(scale, scale->reverse(), nullptr)));
    //rootps->runAction(RepeatForever::create(Sequence::create(rotate, nullptr)));
    rootps->startParticleSystem();

    this->addChild(rootps, 0, PARTICLE_SYSTEM_TAG);

    return true;
}

std::string Particle3DTimeShiftDemo::subtitle() const 
{
    return "TimeShift";
}

bool Particle3DTimeShiftDemo::init()
{
    if (!Particle3DTestDemo::init()) 
        return false;

    auto rootps = PUParticleSystem3D::create("timeShift.pu", "pu_mediapack_01.material");
    rootps->setScale(2.0f);
    rootps->setCameraMask((unsigned short)CameraFlag::USER1);
    rootps->startParticleSystem();

    this->addChild(rootps, 0, PARTICLE_SYSTEM_TAG);

    return true;
}

std::string Particle3DUVAnimDemo::subtitle() const 
{
    return "UVAnim";
}

bool Particle3DUVAnimDemo::init()
{
    if (!Particle3DTestDemo::init()) 
        return false;

    auto rootps = PUParticleSystem3D::create("UVAnimation.pu", "pu_mediapack_01.material");
    rootps->setCameraMask((unsigned short)CameraFlag::USER1);
    rootps->startParticleSystem();

    this->addChild(rootps, 0, PARTICLE_SYSTEM_TAG);

    return true;
}

std::string Particle3DFirePlaceDemo::subtitle() const 
{
    return "Fire";
}

bool Particle3DFirePlaceDemo::init()
{
    if (!Particle3DTestDemo::init()) 
        return false;

    auto rootps = PUParticleSystem3D::create("mp_torch.pu", "pu_mediapack_01.material");
    rootps->setCameraMask((unsigned short)CameraFlag::USER1);
    rootps->setScale(5.0f);
    rootps->startParticleSystem();

    this->addChild(rootps, 0, PARTICLE_SYSTEM_TAG);

    return true;
}

std::string Particle3DLineStreakDemo::subtitle() const 
{
    return "LineStreak";
}

bool Particle3DLineStreakDemo::init()
{
    if (!Particle3DTestDemo::init()) 
        return false;

    auto rootps = PUParticleSystem3D::create("lineStreak.pu", "pu_mediapack_01.material");
    rootps->setCameraMask((unsigned short)CameraFlag::USER1);
    rootps->setScale(5.0f);
    //rootps->runAction(RepeatForever::create(Sequence::create(rotate, nullptr)));
    rootps->startParticleSystem();
    this->addChild(rootps, 0, PARTICLE_SYSTEM_TAG);


    //auto sprite = Sprite::create("pump_streak_04.png");
    //sprite->setCameraMask((unsigned short)CameraFlag::USER1);
    //sprite->setScale(0.5f);
    //this->addChild(sprite);

    return true;
}

std::string Particle3DElectricBeamSystemDemo::subtitle() const 
{
    return "ElectricBeamSystem";
}

bool Particle3DElectricBeamSystemDemo::init()
{
    if (!Particle3DTestDemo::init()) 
        return false;


    auto rootps = PUParticleSystem3D::create("electricBeamSystem.pu");
    rootps->setCameraMask((unsigned short)CameraFlag::USER1);
    rootps->startParticleSystem();
    this->addChild(rootps, 0, PARTICLE_SYSTEM_TAG);

    return true;
}

std::string Particle3DFlareShieldDemo::subtitle() const 
{
    return "flareShield";
}

bool Particle3DFlareShieldDemo::init()
{
    if (!Particle3DTestDemo::init()) 
        return false;


    auto rootps = PUParticleSystem3D::create("flareShield.pu");
    rootps->setCameraMask((unsigned short)CameraFlag::USER1);
    //rootps->setScale(0.25f);
    rootps->startParticleSystem();
    this->addChild(rootps, 0, PARTICLE_SYSTEM_TAG);

    return true;
}

std::string Particle3DLightningBoltDemo::subtitle() const 
{
    return "LightningBolt";
}

bool Particle3DLightningBoltDemo::init()
{
    if (!Particle3DTestDemo::init()) 
        return false;


    auto rootps = PUParticleSystem3D::create("lightningBolt.pu");
    rootps->setCameraMask((unsigned short)CameraFlag::USER1);
    //rootps->setScale(0.25f);
    rootps->startParticleSystem();
    this->addChild(rootps, 0, PARTICLE_SYSTEM_TAG);

    return true;
}

std::string Particle3DExplosionSystemDemo::subtitle() const 
{
    return "ExplosionSystem";
}

bool Particle3DExplosionSystemDemo::init()
{
    if (!Particle3DTestDemo::init()) 
        return false;


    auto rootps = PUParticleSystem3D::create("explosionSystem.pu");
    rootps->setCameraMask((unsigned short)CameraFlag::USER1);
    //rootps->setScale(5.0f);
    //rootps->setPosition(-25.0f, 0.0f);
    //auto moveby = MoveBy::create(2.0f, Vec2(50.0f, 0.0f));
    //auto moveby1 = MoveBy::create(2.0f, Vec2(-50.0f, 0.0f));
    //rootps->runAction(RepeatForever::create(Sequence::create(moveby, moveby1, nullptr)));
    rootps->startParticleSystem();
    this->addChild(rootps, 0, PARTICLE_SYSTEM_TAG);

    return true;
}

std::string Particle3DCanOfWormsDemo::subtitle() const 
{
    return "CanOfWorms";
}

bool Particle3DCanOfWormsDemo::init()
{
    if (!Particle3DTestDemo::init()) 
        return false;

    auto rootps = PUParticleSystem3D::create("canOfWorms.pu");
    rootps->setCameraMask((unsigned short)CameraFlag::USER1);
    rootps->startParticleSystem();
    this->addChild(rootps, 0, PARTICLE_SYSTEM_TAG);

    return true;
}

std::string Particle3DRibbonTrailDemo::subtitle() const 
{
    return "RibbonTrailTest";
}

bool Particle3DRibbonTrailDemo::init()
{
    if (!Particle3DTestDemo::init()) 
        return false;

    auto rootps = PUParticleSystem3D::create("ribbonTrailTest.pu");
    rootps->setCameraMask((unsigned short)CameraFlag::USER1);
    rootps->startParticleSystem();
    this->addChild(rootps, 0, PARTICLE_SYSTEM_TAG);

    return true;
}

std::string Particle3DWeaponTrailDemo::subtitle() const 
{
    return "WeaponTrail";
}

bool Particle3DWeaponTrailDemo::init()
{
    if (!Particle3DTestDemo::init()) 
        return false;

    auto rootps = PUParticleSystem3D::create("weaponTrail.pu");
    rootps->setCameraMask((unsigned short)CameraFlag::USER1);
    rootps->startParticleSystem();
    this->addChild(rootps, 0, PARTICLE_SYSTEM_TAG);

    return true;
}

std::string Particle3DWithSprite3DDemo::subtitle() const 
{
    return "Particle3DWithSprite3D";
}

bool Particle3DWithSprite3DDemo::init()
{
    if (!Particle3DTestDemo::init()) 
        return false;

    std::string c3bfileName = "Sprite3DTest/orc.c3b";
    auto sprite = Sprite3D::create(c3bfileName);
    this->addChild(sprite);
    sprite->setPosition3D(Vec3(-20.0f, 0.0f, 0.0f));
    sprite->setRotation3D(Vec3(0, 180, 0));
    sprite->setCameraMask((unsigned short)CameraFlag::USER1);
    //sprite->setOpacity(100);
    auto animation = Animation3D::create(c3bfileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation);
        sprite->runAction(RepeatForever::create(animate));
    }

    auto billboard = BillBoard::create("Images/Icon.png");
    billboard->setPosition3D(Vec3(20.0f, 0.0f, 0.0f));
    billboard->setScale(0.2f);
    billboard->setCameraMask((unsigned short)CameraFlag::USER1);
    this->addChild(billboard);


    auto rootps = PUParticleSystem3D::create("lineStreak.pu");
    rootps->setCameraMask((unsigned short)CameraFlag::USER1);
    rootps->startParticleSystem();
    this->addChild(rootps, 0, PARTICLE_SYSTEM_TAG);

    return true;
}

class SurfaceRender : public PUParticle3DCustomRender
{
public:
    SurfaceRender(){}
    virtual ~SurfaceRender(){}

    virtual void initRenderParticles(){
        auto vv = _paramValues["surface_dir_local_vector"].asValueVector();
        _surfaceDir = Vec3(vv[0].asFloat(), vv[1].asFloat(), vv[2].asFloat());
        _surfaceDir.normalize();

        vv = _paramValues["surface_up_local_vector"].asValueVector();
        _surfaceUp = Vec3(vv[0].asFloat(), vv[1].asFloat(), vv[2].asFloat());
        _surfaceUp.normalize();
        _stateBlock->setCullFace(false);
    }

    virtual void preRenderParticles(ParticleSystem3D* particleSystem){
        _currentIndex = 0;
        _currentVertexIndex = 0;

        auto pusys = static_cast<PUParticleSystem3D *>(particleSystem);
        Quaternion rot = pusys->getDerivedOrientation();
        _surfaceWorldDir = rot * _surfaceDir;
        _surfaceWorldUp = rot * _surfaceUp;
    }

    virtual void renderParticle(ParticleSystem3D* particleSystem, const PUParticle3D *particle){
        Vec3 up = _surfaceWorldUp;
        up.normalize();
        Vec3 right;
        Vec3::cross(up, _surfaceWorldDir, &right);

        Vec3 halfwidth = particle->width * 0.5f * right;
        Vec3 halfheight = particle->height * 0.5f * up;
        Vec3 position = particle->position;

        float costheta = cosf(-particle->zRotation);
        float sintheta = sinf(-particle->zRotation);
        Vec2 texOffset = 0.5f * (particle->lb_uv + particle->rt_uv);
        Vec2 val;
        val.set((particle->lb_uv.x - texOffset.x), (particle->lb_uv.y - texOffset.y));
        val.set(val.x * costheta - val.y * sintheta, val.x * sintheta + val.y * costheta);
        fillVertex(_currentVertexIndex, (position + (-halfwidth - halfheight)), particle->color, val + texOffset);

        val.set(particle->rt_uv.x - texOffset.x, particle->lb_uv.y - texOffset.y);
        val.set(val.x * costheta - val.y * sintheta, val.x * sintheta + val.y * costheta);
        fillVertex(_currentVertexIndex + 1, (position + (halfwidth - halfheight)), particle->color, val + texOffset);

        val.set(particle->lb_uv.x - texOffset.x, particle->rt_uv.y - texOffset.y);
        val.set(val.x * costheta - val.y * sintheta, val.x * sintheta + val.y * costheta);
        fillVertex(_currentVertexIndex + 2, (position + (-halfwidth + halfheight)), particle->color, val + texOffset);

        val.set(particle->rt_uv.x - texOffset.x, particle->rt_uv.y - texOffset.y);
        val.set(val.x * costheta - val.y * sintheta, val.x * sintheta + val.y * costheta);
        fillVertex(_currentVertexIndex + 3, (position + (halfwidth + halfheight)), particle->color, val + texOffset);

        fillTriangle(_currentIndex, _currentVertexIndex, _currentVertexIndex + 1, _currentVertexIndex + 3);
        fillTriangle(_currentIndex + 3, _currentVertexIndex, _currentVertexIndex + 3, _currentVertexIndex + 2);

        _currentIndex += 6;
        _currentVertexIndex += 4;
    }

    virtual void postRenderParticles(ParticleSystem3D* particleSystem){
        return;
    }

private:

    void fillVertex(unsigned short index, const Vec3 &pos, const Vec4 &color, const Vec2 &uv){
        _vertices[index].position = pos;
        _vertices[index].color = color;
        _vertices[index].uv = uv;
    }
    void fillTriangle(unsigned short index, unsigned short v0, unsigned short v1, unsigned short v2){
        _indices[index] = v0;
        _indices[index + 1] = v1;
        _indices[index + 2] = v2;
    }

private:

    Vec3 _surfaceDir;
    Vec3 _surfaceUp;
    Vec3 _surfaceWorldDir;
    Vec3 _surfaceWorldUp;
    unsigned int _currentIndex;
    unsigned int _currentVertexIndex;
};

std::string Particle3DCustomDemo::subtitle() const
{
    return "Particle3DCustomRenderDemo";
}

bool Particle3DCustomDemo::init()
{
    if (!Particle3DTestDemo::init())
        return false;

    PUParticle3DCustomRender::registerCustomRender("Surface", []()->PUParticle3DCustomRender*{
        return new SurfaceRender;
    });

    this->runAction(RepeatForever::create(Sequence::create(CallFunc::create([=]{
        auto rootps = PUParticleSystem3D::create("custom.pu");
        rootps->setCameraMask((unsigned short)CameraFlag::USER1);
        rootps->startParticleSystem();
        Quaternion rot(Vec3::UNIT_Z, cocos2d::random(-M_PI_4, M_PI_4));
        rootps->setRotationQuat(rot);
        Vec3 endPos(0.0f, 100.0f, 0.0f);
        endPos = rot * endPos;
        rootps->runAction(Sequence::create(MoveBy::create(5.0f, endPos), CallFunc::create([=]{
            rootps->removeFromParent(); 
        }), nullptr));
        this->addChild(rootps, 0, PARTICLE_SYSTEM_TAG);
    }), DelayTime::create(1.0f), nullptr)));

    return true;
}
