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

#include "Camera3DTest.h"
#include "3d/CCCamera3D.h"
//#include "3d/CCBillboardParticleEmitter3D.h"
#include <algorithm>
#include "../testResource.h"

enum
{
	IDC_NEXT = 100,
	IDC_BACK,
	IDC_RESTART
};

static int sceneIdx = -1;

static std::function<Layer*()> createFunctions[] =
{
	CL(Camera3DTestDemo)
};
#define MAX_LAYER    (sizeof(createFunctions) / sizeof(createFunctions[0]))

static Layer* nextSpriteTestAction()
{
	auto layer = (createFunctions[0])();
	return layer;
}

static Layer* backSpriteTestAction()
{
	sceneIdx--;
	int total = MAX_LAYER;
	if( sceneIdx < 0 )
		sceneIdx += total;

	auto layer = (createFunctions[0])();
	return layer;
}

static Layer* restartSpriteTestAction()
{
	auto layer = (createFunctions[0])();
	return layer;
}

//------------------------------------------------------------------
//
// SpriteTestDemo
//
//------------------------------------------------------------------

Camera3DTestDemo::Camera3DTestDemo(void)
	: BaseTest()
{

	_sprite3D=NULL;
	auto s = Director::getInstance()->getWinSize();
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(Camera3DTestDemo::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(Camera3DTestDemo::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(Camera3DTestDemo::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	auto layer3D=Layer3D::create();
	addChild(layer3D,0);
	_layer3D=layer3D;
	addNewSpriteWithCoords( Vec3(s.width/2, s.height/2,0),"Sprite3DTest/scene.c3t",false,false);
	addNewSpriteWithCoords( Vec3(s.width/2, s.height/2,0),"Sprite3DTest/orc.c3t",true,true);
	//addNewParticleSystemWithCoords(Vec3(s.width/2-30, s.height/2,0));
	//addNewParticleSystemWithCoords(Vec3(s.width/2+30, s.height/2,0));
	_mosPos=Point(0,0);
	_mosPosf=Point(0,0);
	TTFConfig ttfConfig("fonts/arial.ttf", 20);
	auto label1 = Label::createWithTTF(ttfConfig,"Move(-x)");
	auto menuItem1 = MenuItemLabel::create(label1, CC_CALLBACK_1(Camera3DTestDemo::translateCameraXCallback,this,-5));
	auto label2 = Label::createWithTTF(ttfConfig,"Move(x)");
	auto menuItem2 = MenuItemLabel::create(label2, CC_CALLBACK_1(Camera3DTestDemo::translateCameraXCallback,this,5));
	auto label3 = Label::createWithTTF(ttfConfig,"Move(-y)");
	auto menuItem3 = MenuItemLabel::create(label3, CC_CALLBACK_1(Camera3DTestDemo::translateCameraYCallback,this,-5));
	auto label4 = Label::createWithTTF(ttfConfig,"Move(y)");
	auto menuItem4 = MenuItemLabel::create(label4, CC_CALLBACK_1(Camera3DTestDemo::translateCameraYCallback,this,5));
	auto label5 = Label::createWithTTF(ttfConfig,"Move(-z)");
	auto menuItem5 = MenuItemLabel::create(label5, CC_CALLBACK_1(Camera3DTestDemo::translateCameraZCallback,this,-5));
	auto label6 = Label::createWithTTF(ttfConfig,"Move(z)");
	auto menuItem6 = MenuItemLabel::create(label6, CC_CALLBACK_1(Camera3DTestDemo::translateCameraZCallback,this,5));
	auto menu = Menu::create(menuItem1,menuItem2,menuItem3,menuItem4,menuItem5,menuItem6,NULL);
	menu->setPosition(Vec2::ZERO);
	menuItem1->setPosition( Vec2( VisibleRect::center().x-200, VisibleRect::center().y-40 ) );
	menuItem2->setPosition( Vec2( VisibleRect::center().x-200, VisibleRect::center().y-80) );
	menuItem3->setPosition( Vec2( VisibleRect::center().x, VisibleRect::center().y-40) );
	menuItem4->setPosition( Vec2( VisibleRect::center().x, VisibleRect::center().y-80) );
	menuItem5->setPosition( Vec2( VisibleRect::center().x+200, VisibleRect::center().y-40) );
	menuItem6->setPosition( Vec2( VisibleRect::center().x+200, VisibleRect::center().y-80) );
	addChild(menu, 0);
}
Camera3DTestDemo::~Camera3DTestDemo(void)
{
}

std::string Camera3DTestDemo::title() const
{
	return "Testing Camera3D";
}

std::string Camera3DTestDemo::subtitle() const
{
	return "";
}
void Camera3DTestDemo::translateCameraXCallback(Ref* sender,float value)
{
		if(_sprite3D)
		{
			Vec3  pos=_sprite3D->getPosition3D();
			pos.x+=value;
			_sprite3D->setPosition3D(pos);
		}
		else
		{
			Vec3  pos=Camera3D::getActiveCamera()->getPosition3D();
			pos.x+=value;
			Camera3D::getActiveCamera()->setPosition3D(pos);
		}
			
}
void Camera3DTestDemo::translateCameraYCallback(Ref* sender,float value)
{
		if(_sprite3D)
		{
			Vec3  pos=_sprite3D->getPosition3D();
	        pos.y+=value;
		    _sprite3D->setPosition3D(pos);
		}
		else
		{
			Vec3  pos=Camera3D::getActiveCamera()->getPosition3D();
	        pos.y+=value;
		    Camera3D::getActiveCamera()->setPosition3D(pos);
		}
			
}
void Camera3DTestDemo::translateCameraZCallback(Ref* sender,float value)
{
	if(_sprite3D)
	{
		Vec3  pos=_sprite3D->getPosition3D();
		pos.z+=value;
		_sprite3D->setPositionZ(pos.z);
	}
	else
	{
		Vec3  pos=Camera3D::getActiveCamera()->getPosition3D();
		pos.z+=value;
		Camera3D::getActiveCamera()->setPositionZ(pos.z);
	}    	
}
void Camera3DTestDemo::onEnter()
{
	BaseTest::onEnter();
}

void Camera3DTestDemo::restartCallback(Ref* sender)
{
	auto s = new Camera3DTestScene();
	s->addChild(restartSpriteTestAction());

	Director::getInstance()->replaceScene(s);
	s->release();
}

void Camera3DTestDemo::nextCallback(Ref* sender)
{
	auto s = new Camera3DTestScene();
	s->addChild( nextSpriteTestAction() );
	Director::getInstance()->replaceScene(s);
	s->release();
}

void Camera3DTestDemo::backCallback(Ref* sender)
{
	auto s = new Camera3DTestScene();
	s->addChild( backSpriteTestAction() );
	Director::getInstance()->replaceScene(s);
	s->release();
}
//void Camera3DTestDemo::addNewParticleSystemWithCoords(Vec3 p)
//{
//	auto particleSystem3D = BillboardParticleEmitter3D::create();
//	particleSystem3D->setPosition(Vec2(p.x,p.y));
//	particleSystem3D->setPositionZ(p.z);
//	particleSystem3D->setTexture(s_fire);
//	particleSystem3D->setTotalParticles(250);
//	particleSystem3D->setScale(0.1);
//	particleSystem3D->setDefaultProperty();
//	particleSystem3D->start();
//	_layer3D->addChild(particleSystem3D,0);
//}
void Camera3DTestDemo::addNewSpriteWithCoords(Vec3 p,std::string fileName,bool playAnimation,bool bindCamera)
{
	
	auto sprite = Sprite3D::create(fileName);
	sprite->setScale(1);
	_layer3D->addChild(sprite);
	sprite->setPosition( Vec2( p.x, p.y) );
    sprite->setPositionZ(p.z);
	if(playAnimation)
	{
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
	if(bindCamera)
	{
		_sprite3D=sprite;
		auto s = Director::getInstance()->getWinSize();
		auto camera=Camera3D::createPerspective(60, (GLfloat)s.width/s.height, 10, 1000);
		if(_sprite3D)
		{
			camera->lookAt(Vec3(0,0,50),Vec3(0, 1, 0),Vec3(0,0,0));
			_sprite3D->addChild(camera);
		}
		else
		{
			camera->lookAt(Vec3(240,160,50),Vec3(0, 1, 0),Vec3(240,160,0));
			_layer3D->addChild(camera);
		}
		camera->setActiveCamera();
		
		
	}
	else
	{
		sprite->setScale(3);
		sprite->setRotation3D(Vec3(0,0,0));
	}
}
bool Camera3DTestDemo::onTouchBegan(Touch* touch, Event* event)
{
	if(touch)
	{
		auto location = touch->getLocation();
		_mosPosf=_mosPos;
		_mosPos=location;  
	}
	return true;
}
Vec3  rotation(0,0,0);
void Camera3DTestDemo::onTouchMoved(Touch* touch, Event* event)
{
	if(touch)
	{
		auto location = touch->getLocation();
		_mosPosf=_mosPos;
		_mosPos=location;
		float angleX=(_mosPos.x-_mosPosf.x)*0.4;
		float angleY=(_mosPos.y-_mosPosf.y)*0.4;
		if(_layer3D)
		{
			if(_sprite3D)
			{
				Vec3  rotation=_sprite3D->getRotation3D();
				rotation.y+=angleX;
				_sprite3D->setRotation3D(rotation);
			}
			else
			{
				Vec3  rotation=Camera3D::getActiveCamera()->getRotation3D();
				rotation.y+=angleX;
				Camera3D::getActiveCamera()->setRotation3D(rotation);
			}
		}
	}
}
void Camera3DTestDemo::onTouchesEnded(Touch* touch, Event* event)
{
	if(touch)
	{
		auto location = touch->getLocation();
		_mosPosf=_mosPos;
		_mosPos=location;  
	}
}
void Camera3DTestScene::runThisTest()
{
	auto layer = nextSpriteTestAction();
	addChild(layer);
	Director::getInstance()->replaceScene(this);
}
bool Layer3D::init()
{
	 auto s = Director::getInstance()->getWinSize();
	 auto camera=Camera3D::createPerspective(60, (GLfloat)s.width/s.height, 10, 1000);
	 camera->lookAt(Vec3(240,160,50),Vec3(0, 1, 0),Vec3(240,160,0));
	 camera->setActiveCamera();
     addChild(camera);
	return true;
}
void Layer3D::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
	if (!_visible)
	{
		return;
	}
	_groupCommand.init(_globalZOrder);
	renderer->addCommand(&_groupCommand);
	renderer->pushGroup(_groupCommand.getRenderQueueID());
	bool dirty = (parentFlags & FLAGS_TRANSFORM_DIRTY) || _transformUpdated;
	if(dirty)
		_modelViewTransform = this->transform(parentTransform);
	_transformUpdated = false;
	Director* director = Director::getInstance();
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);
	Camera3D::getActiveCamera()->applyProjection();
	_BeginCommand.init(_globalZOrder);
	_BeginCommand.func = CC_CALLBACK_0(Layer3D::onBeginDraw, this);
	renderer->addCommand(&_BeginCommand);
	int i = 0;
	if(!_children.empty())
	{
		sortAllChildren();
		// draw children zOrder < 0
		for( ; i < _children.size(); i++ )
		{
			auto node = _children.at(i);

			if ( node && node->getLocalZOrder() < 0 )
				node->visit(renderer, _modelViewTransform, dirty);
			else
				break;
		}
		// self draw
		this->draw(renderer, _modelViewTransform, dirty);

		for(auto it=_children.cbegin()+i; it != _children.cend(); ++it)
			(*it)->visit(renderer, _modelViewTransform, dirty);
	}
	else
	{
		this->draw(renderer, _modelViewTransform, dirty);
	}
	_orderOfArrival = 0;
	_EndCommand.init(_globalZOrder);
	_EndCommand.func = CC_CALLBACK_0(Layer3D::onEndDraw, this);
	renderer->addCommand(&_EndCommand);
	renderer->popGroup();
	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}
void Layer3D::onBeginDraw()
{
	Director *director = Director::getInstance();
	_directorProjection = director->getProjection();
}
void Layer3D::onEndDraw()
{
	Director *director = Director::getInstance();
	director->setProjection(_directorProjection);
}
