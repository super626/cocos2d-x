/****************************************************************************
 Copyright (c) 2015 Chukong Technologies Inc.
 
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

#ifndef __PHYSICS_SPRITE_3D_H__
#define __PHYSICS_SPRITE_3D_H__

#include "3d/CCSprite3D.h"
#include "CCPhysics3DObject.h"
#include "CCPhysics3DComponent.h"
#include "extensions/ExtensionMacros.h"
#include "extensions/ExtensionExport.h"

#if (CC_ENABLE_BULLET_INTEGRATION)

NS_CC_EXT_BEGIN

class CC_EX_DLL PhysicsSprite3D : public cocos2d::Sprite3D
{
public:

    /** creates a PhysicsSprite3D*/
    static PhysicsSprite3D* create(const std::string &modelPath, Physics3DRigidBodyDes& rigidDes, const cocos2d::Vec3& translateInPhysics = cocos2d::Vec3::ZERO, const cocos2d::Quaternion& rotInPhsyics = cocos2d::Quaternion::ZERO);
    
    Physics3DObject* getPhysicsObj() const;
    
    void setSyncFlag(Physics3DComponent::PhysicsSyncFlag syncFlag);
    
    void syncToPhysics();
    
    void syncToNode();
    
CC_CONSTRUCTOR_ACCESS:
    PhysicsSprite3D();
    virtual ~PhysicsSprite3D();

protected:
    Physics3DComponent* _physicsComponent;
};

NS_CC_EXT_END

#endif // CC_ENABLE_BULLET_INTEGRATION

#endif // __PHYSICS_SPRITE_3D_H__
