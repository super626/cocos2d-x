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

#ifndef _SPRITE3D_TEST_H_
#define _SPRITE3D_TEST_H_

#include "../testBasic.h"
#include "../BaseTest.h"
#include "3d/CCOBB.h"
#include "3d/CCBillBoard.h"
#include <string>
#include <vector>

namespace cocos2d {
    class Animate3D;
    class Sprite3D;
    class Delay;
    class DrawNode3D;
    class OBB;
    class Ray;
    class AABB;
    class BillBorad;
}

class Sprite3DTestDemo : public BaseTest
{
public:
    Sprite3DTestDemo(void);
    virtual ~Sprite3DTestDemo(void);
    
    void restartCallback(Ref* sender);
    void nextCallback(Ref* sender);
    void backCallback(Ref* sender);
    
    // overrides
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void onEnter() override;

protected:
    std::string    _title;
};

class Sprite3DBasicTest : public Sprite3DTestDemo
{
public:
    CREATE_FUNC(Sprite3DBasicTest);
    Sprite3DBasicTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    
    void addNewSpriteWithCoords(Vec2 p);
    void onTouchesEnded(const std::vector<Touch*>& touches, Event* event);
};

class Sprite3DHitTest : public Sprite3DTestDemo
{
public:
    CREATE_FUNC(Sprite3DHitTest);
    Sprite3DHitTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;    
};

class EffectSprite3D;

class Effect3D : public Ref
{
public:
    virtual void draw(const Mat4 &transform) = 0;
    virtual void setTarget(EffectSprite3D *sprite) = 0;
protected:
    Effect3D() : _glProgramState(nullptr) {}
    virtual ~Effect3D()
    {
        CC_SAFE_RELEASE(_glProgramState);
    }
protected:
    GLProgramState* _glProgramState;
};

class Effect3DOutline: public Effect3D
{
public:
    static Effect3DOutline* create();
    
    void setOutlineColor(const Vec3& color);
    
    void setOutlineWidth(float width);
    
    virtual void draw(const Mat4 &transform) override;
    virtual void setTarget(EffectSprite3D *sprite) override;
protected:
    
    Effect3DOutline();
    virtual ~Effect3DOutline();
    
    bool init();
    
    Vec3 _outlineColor;
    float _outlineWidth;
    //weak reference
    EffectSprite3D* _sprite;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    EventListenerCustom* _backToForegroundListener;
#endif
    
protected:
    static const std::string _vertShaderFile;
    static const std::string _fragShaderFile;
    static const std::string _keyInGLProgramCache;
    static GLProgram* getOrCreateProgram();
};

class EffectSprite3D : public Sprite3D
{
public:
    static EffectSprite3D* createFromObjFileAndTexture(const std::string& objFilePath, const std::string& textureFilePath);
    void setEffect3D(Effect3D* effect);
    void addEffect(Effect3DOutline* effect, ssize_t order);
    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
protected:
    EffectSprite3D();
    virtual ~EffectSprite3D();
    
    std::vector<std::tuple<ssize_t,Effect3D*,CustomCommand>> _effects;
    Effect3D* _defaultEffect;
    CustomCommand _command;
};

class Sprite3DEffectTest : public Sprite3DTestDemo
{
public:
    CREATE_FUNC(Sprite3DEffectTest);
    Sprite3DEffectTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    
    void addNewSpriteWithCoords(Vec2 p);
    
    void onTouchesEnded(const std::vector<Touch*>& touches, Event* event);
};

class Sprite3DWithSkinTest : public Sprite3DTestDemo
{
public:
    CREATE_FUNC(Sprite3DWithSkinTest);
    Sprite3DWithSkinTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    
    void addNewSpriteWithCoords(Vec2 p);
    
    void onTouchesEnded(const std::vector<Touch*>& touches, Event* event);
};

class Animate3DTest : public Sprite3DTestDemo
{
public:
    CREATE_FUNC(Animate3DTest);
    Animate3DTest();
    ~Animate3DTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    
    void onTouchesEnded(const std::vector<Touch*>& touches, Event* event);
    
    virtual void update(float dt) override;
    
protected:
    void unproject(const Mat4& viewProjection, const Size* viewport, Vec3* src, Vec3* dst);
    void calculateRayByLocationInView(Ray* ray, const Vec2& location);
    
protected:
    void addSprite3D();
    
    enum class State
    {
        SWIMMING,
        SWIMMING_TO_HURT,
        HURT,
        HURT_TO_SWIMMING,
    };
    
    void reachEndCallBack();
    
    void renewCallBack();
    
    cocos2d::Sprite3D* _sprite;
    
    cocos2d::Animate3D* _swim;
    cocos2d::Animate3D* _hurt;
    float _transTime;
    float _elapseTransTime;
    
    State   _state;
    
    MoveTo* _moveAction;
    
    DrawNode3D* _drawDebug;
    OBB _obb;
    bool _hasPick;
};


class AttachmentTest : public Sprite3DTestDemo
{
public:
    CREATE_FUNC(AttachmentTest);
    AttachmentTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    
    void onTouchesEnded(const std::vector<Touch*>& touches, Event* event);
    
    void addNewSpriteWithCoords(Vec2 p);
    
protected:
    bool _hasWeapon;
    cocos2d::Sprite3D* _sprite;
};

class Sprite3DTestScene : public TestScene
{
public:
    virtual void runThisTest();
};

class Sprite3DWithSubMeshTest : public Sprite3DTestDemo
{
public:
    CREATE_FUNC(Sprite3DWithSubMeshTest);
    Sprite3DWithSubMeshTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void addNewSpriteWithCoords(Vec2 p);
    void onTouchesEnded(const std::vector<Touch*>& touches, Event* event);
};

class Sprite3DWithCollisonTest : public Sprite3DTestDemo
{
public:
    CREATE_FUNC(Sprite3DWithCollisonTest);
    Sprite3DWithCollisonTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void update(float dt) override;
    void addNewSpriteWithCoords(Vec2 p);
    void onTouchesEnded(const std::vector<Touch*>& touches, Event* event);
protected:
    void reachEndCallBack();

    cocos2d::Sprite3D*          _sprite1;
    cocos2d::Sprite3D*          _sprite2;
    MoveTo*                     _moveActionGo1;
    MoveTo*                     _moveActionGo2;
    DrawNode3D*                 _drawAABB1;
    DrawNode3D*                 _drawAABB2;
    OBB*                        _obb1;
    OBB*                        _obb2;
    bool                        _hasCollider;
    Label*                      _labelHPPos;
    int                         _HP;
    int                         _Dir;
};

class Sprite3DWithOBBPerfromanceTest : public Sprite3DTestDemo
{
public:
    CREATE_FUNC(Sprite3DWithOBBPerfromanceTest);
    Sprite3DWithOBBPerfromanceTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void update(float dt) override;
    void addNewOBBWithCoords(Vec2 p);
    void addNewSpriteWithCoords(Vec2 p);
    void onTouchesBegan(const std::vector<Touch*>& touches, Event* event);
    void onTouchesEnded(const std::vector<Touch*>& touches, Event* event);
    void onTouchesMoved(const std::vector<Touch*>& touches, Event* event);
    void addOBBCallback(Ref* sender);
    void delOBBCallback(Ref* sender);
    void addOBBWithCount(float value);
    void delOBBWithCount(float value);
protected:
    cocos2d::Sprite3D*        _sprite;
    std::vector<OBB>          _obb;
    DrawNode3D*               _drawOBB;
    Label*                    _labelCubeCount;
    MoveTo*                   _moveAction;
    OBB                       _obbt;
    DrawNode3D*               _drawDebug;
    bool                      _hasCollider;
    std::set<int>             _intersetList;
    void initDrawBox();
    void reachEndCallBack();

    void unproject(const Mat4& viewProjection, const Size* viewport, Vec3* src, Vec3* dst);
    void calculateRayByLocationInView(Ray* ray, const Vec2& location);
};

class Sprite3DWithAABBPerfromanceTest : public Sprite3DTestDemo
{
public:
    CREATE_FUNC(Sprite3DWithAABBPerfromanceTest);
    Sprite3DWithAABBPerfromanceTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void update(float dt) override;
    void addNewAABBWithCoords(Vec2 p);
    void onTouchesEnded(const std::vector<Touch*>& touches, Event* event);
    void addAABBCallback(Ref* sender);
    void delAABBCallback(Ref* sender);
    void addAABBWithCount(float value);
    void delAABBWithCount(float value);
protected:
    std::vector<AABB>          _aabb;
    DrawNode3D*               _drawAABB;
    Label*                    _labelCubeCount;

    void initDrawBox();
};

class BillBoardTest : public Sprite3DTestDemo
{
public:
    virtual ~BillBoardTest(void);
    CREATE_FUNC(BillBoardTest);
    BillBoardTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void update(float dt) override;
    void addNewBillBoradWithCoords(Vec3 p);
    void addNewAniBillBoradWithCoords(Vec3 p);
    void rotateCameraCallback(Ref* sender,float value);
    void onTouchesMoved(const std::vector<Touch*>& touches, Event* event);

protected:
    BillBorad*        _billborad;
    BillBorad*        _billboradAni;
    Camera3D*         _camera;
    Layer*            _layerBillBorad;

};


#endif
