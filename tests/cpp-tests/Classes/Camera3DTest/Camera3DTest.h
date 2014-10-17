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
#include "base/CCCamera.h"
namespace cocos2d {
    class Sprite3D;
    class Delay;
}

class DrawLine3D: public Node
{
public:
    /** creates and initialize a node */
    static DrawLine3D* create();
    
    /**
     * Draw 3D Line
     */
    void drawLine(const Vec3 &from, const Vec3 &to, const Color4F &color);
    
    void drawCube(Vec3* vertices, const Color4F &color);
    
    /** Clear the geometry in the node's buffer. */
    void clear()
    {
        _buffer.clear();
    }
    
    void onDraw(const Mat4 &transform, uint32_t flags);
    
    // Overrides
    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
    
CC_CONSTRUCTOR_ACCESS:
    DrawLine3D()
    {
        
    }
    virtual ~DrawLine3D()
    {
        
    }
    virtual bool init();
    
protected:
    struct V3F_C4B
    {
        Vec3     vertices;
        Color4B  colors;
    };
    
    std::vector<V3F_C4B> _buffer;
    
    CustomCommand _customCommand;
    
private:
    CC_DISALLOW_COPY_AND_ASSIGN(DrawLine3D);
};

enum State
{
    State_None = 0,
    State_Idle = 0x01,
    State_Move = 0x02,
    State_Rotate = 0x04,
    State_Speak = 0x08,
    State_MeleeAttack = 0x10,
    State_RemoteAttack = 0x20,
    State_Attack = 0x40,
};
enum class CameraType
{
    FreeCamera=0,
    FirstCamera=1,
    ThirdCamera=2,
};
class Camera3DTestDemo : public BaseTest
{
public:
    CREATE_FUNC(Camera3DTestDemo);
    Camera3DTestDemo(void);
    virtual ~Camera3DTestDemo(void);

    void restartCallback(Ref* sender);
    void nextCallback(Ref* sender);
    void backCallback(Ref* sender);
    virtual void onEnter() override;
    virtual void onExit() override;
    // overrides
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void addNewSpriteWithCoords(Vec3 p,std::string fileName,bool playAnimation=false,float scale=1.0f,bool bindCamera=false);
    void onTouchesBegan(const std::vector<Touch*>& touches, cocos2d::Event  *event);
    void onTouchesMoved(const std::vector<Touch*>& touches, cocos2d::Event  *event);
    void onTouchesEnded(const std::vector<Touch*>& touches, cocos2d::Event  *event);
    void scaleCameraCallback(Ref* sender,float value);
    void rotateCameraCallback(Ref* sender,float value);
    void SwitchViewCallback(Ref* sender,CameraType cameraType);
    void updateCamera(float fDelta);
    void move3D(float elapsedTime);
    void updateState(float elapsedTime);
    bool isState(unsigned int state,unsigned int bit) const;
    void reachEndCallBack();
protected:
    std::string    _title;
    Layer*         _layer3D;
    Sprite3D*      _sprite3D;
    Vec3           _targetPos;
    CameraType     _cameraType;
    MenuItem*      _incRot;
    MenuItem*      _decRot;
    unsigned int   _curState;
    Camera*      _camera;
    MoveTo* _moveAction;
};

class CameraClipDemo : public BaseTest
{
public:
    CREATE_FUNC(CameraClipDemo);
    CameraClipDemo(void);
    virtual ~CameraClipDemo(void);
    
    void restartCallback(Ref* sender);
    void nextCallback(Ref* sender);
    void backCallback(Ref* sender);
    virtual void onEnter() override;
    virtual void onExit() override;
    // overrides
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void addNewSpriteWithCoords(Vec3 p,std::string fileName,bool playAnimation=false,float scale=1.0f,bool bindCamera=false);
    void onTouchesBegan(const std::vector<Touch*>& touches, cocos2d::Event  *event);
    void onTouchesMoved(const std::vector<Touch*>& touches, cocos2d::Event  *event);
    void onTouchesEnded(const std::vector<Touch*>& touches, cocos2d::Event  *event);
    
    void SwitchViewCallback(Ref* sender,CameraType cameraType);
    void update(float fDelta);
    
    void calculateRay(Ray& ray, const Camera& camera, const Vec2& location);
    
    // draw Camera Frustum
    void drawCameraFrustum();
    
    void moveRole(float elapsedTime);
    void moveCamera(float elapsedTime);
    void updateState(float elapsedTime);
    bool isState(unsigned int state,unsigned int bit) const;
    void reachEndCallBack();
    
protected:
    std::string    _title;
    Layer*         _layer3D;
    Sprite3D*      _sprite3D;
    std::vector<Sprite3D*> objects;
    Vec3           _targetPos;
    CameraType     _cameraType;
    unsigned int   _curState;
    Camera*        _cameraFirst;
    Camera*        _cameraThird;
    MoveTo* _moveAction;
    DrawLine3D* _drawAABB;
    DrawLine3D* _drawFrustum;
    Plane _land;
};

class Camera3DTestScene : public TestScene
{
public:
    virtual void runThisTest();
};
#endif
