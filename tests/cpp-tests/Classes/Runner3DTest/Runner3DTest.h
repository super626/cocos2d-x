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

#ifndef _RUNNER3D_TEST_H_
#define _RUNNER3D_TEST_H_

#include "../testBasic.h"
#include "../BaseTest.h"
#include <string>
#include <list>

class Runner3DTestDemo : public BaseTest
{
public:
    CREATE_FUNC(Runner3DTestDemo);
    Runner3DTestDemo();
    virtual ~Runner3DTestDemo();

    virtual std::string title() const;

    virtual void restartCallback(Ref* sender);
    virtual void nextCallback(Ref* sender);
    virtual void backCallback(Ref* sender);
    virtual void onEnter() override;
    virtual void onExit() override;

    virtual void update(float dt) override;

    void onKeyPressedEvent(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleasedEvent(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    bool onTouchBeganEvent(cocos2d::Touch *touch, cocos2d::Event  *event);
    void onTouchMovedEvent(cocos2d::Touch *touch, cocos2d::Event  *event);
    void onTouchEndedEvent(cocos2d::Touch *touch, cocos2d::Event  *event);

private:

    void initDemo();
    void initEvent();
    void initCamera(cocos2d::Node *parent);
    void init3DScene(cocos2d::Node *parent);

    cocos2d::Sprite3D* CreateCoin(const cocos2d::Vec3 &pos);
    bool checkCoinOutOfBound(cocos2d::Node *coin);
    bool checkCollectedCoin(cocos2d::Node *coin);
    void updateCamera();
    void updateCoins(float dt);
    void updateScore();
    bool vec3equals(const cocos2d::Vec3 &lvec3, const cocos2d::Vec3 &rvec3);

private:

    cocos2d::Layer *_layer3D;
    cocos2d::Camera *_camera;
    cocos2d::Sprite3D *_sprite;
    cocos2d::Label *_scoreLabel;
    cocos2d::Action *_spriteAnim;
    cocos2d::EventKeyboard::KeyCode _keyCode;
    float _moveSpeed;
    float _coinRotSpeed;
    float _modelOffsetRot;

    cocos2d::Layer *_coinLayer;
    unsigned int _collectedCoinNum;
    cocos2d::Vec3 _newIconPos;
    cocos2d::Vec3 _touchPos;
    bool _isRunningAction;
};

class Runner3DTestScene : public TestScene
{
public:
    virtual void runThisTest();
};

#endif
