#include "Runner3DTest.h"

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
{
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

void Runner3DTestDemo::initDemo()
{
	_layer3D = cocos2d::Layer::create();
	addChild(_layer3D);
	initCamera(_layer3D);
	init3DScene(_layer3D);

	//important
	_layer3D->setCameraMask(2);
	//
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
	_sprite->setScale(0.2f);

	parent->addChild(_sprite);
}

void Runner3DTestScene::runThisTest()
{
	auto layer = nextRunner3DTestAction();
	addChild(layer);
	Director::getInstance()->replaceScene(this);
}
