/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 
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

#ifndef _CAMERA3D_TEST_H_
#define _CAMERA3D_TEST_H_

#include "../testBasic.h"
#include "../BaseTest.h"
#include <string>

namespace cocos2d {
    class Sprite3D;
	class Camera3D;
    class Delay;
}
class Layer3D;
class Camera3DTestDemo : public BaseTest
{
public:
	CREATE_FUNC(Camera3DTestDemo);
    Camera3DTestDemo(void);
    virtual ~Camera3DTestDemo(void);
    
    void restartCallback(Ref* sender);
    void nextCallback(Ref* sender);
    void backCallback(Ref* sender);
    
    // overrides
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void onEnter() override;
	void addNewSpriteWithCoords(Vec3 p,std::string fileName,bool playAnimation=false,bool bindCamera=false);
	//void addNewParticleSystemWithCoords(Vec3 p);
    bool onTouchBegan(Touch* touch, Event* event); 
    void onTouchMoved(Touch* touch, Event* event); 
    void onTouchesEnded(Touch* touch, Event* event);
    void translateCameraXCallback(Ref* sender,float value);
    void translateCameraYCallback(Ref* sender,float value);
    void translateCameraZCallback(Ref* sender,float value);
protected:
    std::string    _title;
    Layer3D*      _layer3D;
	Sprite3D*     _sprite3D;
    Point    _mosPos;
    Point    _mosPosf;
};
class Camera3DTestScene : public TestScene
{
public:
    virtual void runThisTest();
};
class Layer3D : public cocos2d::Layer
{
public:
    virtual bool init();  
    virtual void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags);
    void onBeginDraw();
    void onEndDraw();
    CREATE_FUNC(Layer3D);
    GroupCommand _groupCommand;
    CustomCommand _BeginCommand;
    CustomCommand _EndCommand;
    Director::Projection _directorProjection;
};
#endif
