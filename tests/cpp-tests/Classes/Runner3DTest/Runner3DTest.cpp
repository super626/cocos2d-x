#include "Runner3DTest.h"

#define MAX_COIN_NUM 10
#define MAX_COIN_SPACE 20
static int sceneIdx = -1;

static std::function<Layer*()> createFunctions[] =
{
    CL(Runner3DTestDemo),
};
#define MAX_LAYER    (sizeof(createFunctions) / sizeof(createFunctions[0]))

static Layer* nextRunner3DTestAction()
{
    sceneIdx++;
    sceneIdx = sceneIdx % MAX_LAYER;
    auto layer = (createFunctions[sceneIdx])();
    return layer;
}

static Layer* backRunner3DTestAction()
{
    sceneIdx--;
    int total = MAX_LAYER;
    if( sceneIdx < 0 )
        sceneIdx += total;

    auto layer = (createFunctions[sceneIdx])();
    return layer;
}

static Layer* restartRunner3DTestAction()
{
    auto layer = (createFunctions[sceneIdx])();
    return layer;
}

Runner3DTestDemo::Runner3DTestDemo()
    : _layer3D(nullptr)
    , _camera(nullptr)
	, _sprite(nullptr)
	, _spriteAnim(nullptr)
	, _keyCode(cocos2d::EventKeyboard::KeyCode::KEY_NONE)
	, _moveSpeed(50.0f)
	, _coinRotSpeed(50.0f)
	, _modelOffsetRot(0.0f)
{
	scheduleUpdate();
}


Runner3DTestDemo::~Runner3DTestDemo()
{
}

std::string Runner3DTestDemo::title() const
{
    return "Runner3D Demo";
}

void Runner3DTestDemo::restartCallback( Ref* sender )
{
    auto s = new Runner3DTestScene();
    s->addChild(restartRunner3DTestAction());

    Director::getInstance()->replaceScene(s);
    s->release();
}

void Runner3DTestDemo::nextCallback( Ref* sender )
{
    auto s = new Runner3DTestScene();
    s->addChild(nextRunner3DTestAction());

    Director::getInstance()->replaceScene(s);
    s->release();
}

void Runner3DTestDemo::backCallback( Ref* sender )
{
    auto s = new Runner3DTestScene();
    s->addChild(backRunner3DTestAction());

    Director::getInstance()->replaceScene(s);
    s->release();
}

void Runner3DTestDemo::onEnter()
{
    BaseTest::onEnter();
    initDemo();
}

void Runner3DTestDemo::onExit()
{
	BaseTest::onExit();

	_coinList.clear();
	_spriteAnim->release();
	_sprite->release();

}

void Runner3DTestDemo::initDemo()
{
    _layer3D = cocos2d::Layer::create();
    addChild(_layer3D);

    initCamera(_layer3D);
    init3DScene(_layer3D);
	initEvent();

    //important
    _layer3D->setCameraMask(2);
    //
}

void Runner3DTestDemo::initEvent()
{
	auto keyListener = cocos2d::EventListenerKeyboard::create();
	keyListener->onKeyPressed = CC_CALLBACK_2(Runner3DTestDemo::onKeyPressed, this);
	keyListener->onKeyReleased = CC_CALLBACK_2(Runner3DTestDemo::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
}

void Runner3DTestDemo::initCamera(cocos2d::Node *parent)
{
    auto s = Director::getInstance()->getWinSize();
    _camera = cocos2d::Camera::createPerspective(60.0f, s.width / s.height, 1.0f, 10000.0f);
    _camera->setCameraFlag(cocos2d::CameraFlag::USER1);
    _camera->setPosition3D(cocos2d::Vec3(0.0f, 50.0f, 100.0f));
    _camera->lookAt(cocos2d::Vec3(0.0f, 0.0f, 0.0f), Vec3(0,1,0));
    parent->addChild(_camera);
}

void Runner3DTestDemo::init3DScene(cocos2d::Node *parent)
{
    _sprite = Sprite3D::create("Sprite3DTest/girl.c3b");
	_sprite->retain();
    _sprite->setScale(0.2f);
	_modelOffsetRot = 45.0f;
	auto animation = Animation3D::create("Sprite3DTest/girl.c3b","Take 001");
	auto animate = Animate3D::create(animation);
	//animate->setSpeed(_moveSpeed);
	_spriteAnim = RepeatForever::create(animate);
	_spriteAnim->retain();
    parent->addChild(_sprite);

	auto coinLayer = Layer::create();
	for (unsigned int i = 0; i < MAX_COIN_NUM; ++i)
	{
		auto coinsp = Sprite3D::create("Sprite3DTest/coin.obj");
		coinsp->setPositionX(MAX_COIN_SPACE * i);
		coinsp->setPositionY(20.0f);
		_coinList.push_back(coinsp);
		coinLayer->addChild(coinsp);
	}
	parent->addChild(coinLayer);
}

void Runner3DTestDemo::onKeyPressed( cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event )
{
	if (_keyCode == cocos2d::EventKeyboard::KeyCode::KEY_NONE)
	{
		_keyCode = keyCode;
		if (_sprite)
			_sprite->runAction(_spriteAnim);
	}
}

void Runner3DTestDemo::onKeyReleased( cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event )
{
	_keyCode = cocos2d::EventKeyboard::KeyCode::KEY_NONE;
	if (_sprite)
		_sprite->stopAction(_spriteAnim);
}

void Runner3DTestDemo::update( float dt )
{
	bool needUpdateCam = false;
	if (_sprite)
	{
		switch (_keyCode)
		{
		case cocos2d::EventKeyboard::KeyCode::KEY_W:
			{
				_sprite->setPositionZ(_sprite->getPositionZ() - _moveSpeed * dt);
				_sprite->setRotation3D(Vec3(0.0f, 180.0f + _modelOffsetRot, 0.0f));
				needUpdateCam = true;
			}
			break;

		case cocos2d::EventKeyboard::KeyCode::KEY_S:
			{
				_sprite->setPositionZ(_sprite->getPositionZ() + _moveSpeed * dt);
				_sprite->setRotation3D(Vec3(0.0f, 0.0f + _modelOffsetRot, 0.0f));
				needUpdateCam = true;
			}
			break;

		case cocos2d::EventKeyboard::KeyCode::KEY_A:
			{
				_sprite->setPositionX(_sprite->getPositionX() - _moveSpeed * dt);
				_sprite->setRotation3D(Vec3(0.0f, -90.0f + _modelOffsetRot, 0.0f));
				needUpdateCam = true;
			}
			break;

		case cocos2d::EventKeyboard::KeyCode::KEY_D:
			{
				_sprite->setPositionX(_sprite->getPositionX() + _moveSpeed * dt);
				_sprite->setRotation3D(Vec3(0.0f, 90.0f + _modelOffsetRot, 0.0f));
				needUpdateCam = true;
			}
			break;

		default:
			break;
		}
	}

	for (CoinList::iterator iter = _coinList.begin(); iter != _coinList.end(); ++iter)
	{
		auto sprite = *iter;
		//if (sprite->isVisible())
		{
			cocos2d::Vec3 rot = sprite->getRotation3D();
			rot.y += dt * _coinRotSpeed;
			if (360.0f < rot.y) 
				rot.y -= 360.0f;

			sprite->setRotation3D(rot);
		}
	}

	if (_camera && needUpdateCam)
	{
		_camera->lookAt(_sprite->getPosition3D(), cocos2d::Vec3(0.0f, 1.0f, 0.0f));
		_camera->setPosition3D(_sprite->getPosition3D() + cocos2d::Vec3(0.0f, 50.0f, 100.0f));
	}
}

void Runner3DTestScene::runThisTest()
{
    auto layer = nextRunner3DTestAction();
    addChild(layer);
    Director::getInstance()->replaceScene(this);
}
