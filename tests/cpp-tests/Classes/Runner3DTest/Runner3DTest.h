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

private:

	void initDemo();
	void initCamera(cocos2d::Node *parent);
	void init3DScene(cocos2d::Node *parent);

private:

	cocos2d::Layer *_layer3D;
	cocos2d::Camera *_camera;
	cocos2d::Sprite3D *_sprite;
};

class Runner3DTestScene : public TestScene
{
public:
	virtual void runThisTest();
};

#endif
