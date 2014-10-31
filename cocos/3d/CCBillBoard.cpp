/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.

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

#include "3d/CCBillBoard.h"
#include "2d/CCSpriteFrameCache.h"
#include "base/CCDirector.h"
#include "2d/CCCamera.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCGLProgramCache.h"

NS_CC_BEGIN

BillBoard::BillBoard()
: _zDepthInView(0.0f)
, _mode(Mode::VIEW_POINT_ORIENTED)
, _modeDirty(false)
{
    Node::setAnchorPoint(Vec2(0.5f,0.5f));
}

BillBoard::~BillBoard()
{
}



BillBoard* BillBoard::create(Mode mode)
{
    BillBoard *billborad = new (std::nothrow) BillBoard();
    if (billborad && billborad->init())
    {
        billborad->_mode = mode;
        billborad->autorelease();
        return billborad;
    }
    CC_SAFE_DELETE(billborad);
    return nullptr;
}


void BillBoard::setMode( Mode mode )
{
    _mode = mode;
    _modeDirty = true;
}

BillBoard::Mode BillBoard::getMode() const
{
    return _mode;
}

void BillBoard::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    // quick return if not visible. children won't be drawn.
    if (!_visible)
    {
        return;
    }

    uint32_t flags = processParentFlags(parentTransform, parentFlags);

    // IMPORTANT:
    // To ease the migration to v3.0, we still support the Mat4 stack,
    // but it is deprecated and your code should not rely on it
    Director* director = Director::getInstance();
    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);

    bool visibleByCamera = isVisitableByVisitingCamera();

    _modelViewTransform = billBoardTransform(_modelViewTransform);


    int i = 0;

    if(!_children.empty())
    {
        sortAllChildren();
        // draw children zOrder < 0
        for( ; i < _children.size(); i++ )
        {
            auto node = _children.at(i);

            if ( node && node->getLocalZOrder() < 0 )
                node->visit(renderer, _modelViewTransform, flags);
            else
                break;
        }
        // self draw
        if (visibleByCamera)
            this->draw(renderer, _modelViewTransform, flags);

        for(auto it=_children.cbegin()+i; it != _children.cend(); ++it)
            (*it)->visit(renderer, _modelViewTransform, flags);
    }
    else if (visibleByCamera)
    {
        this->draw(renderer, _modelViewTransform, flags);
    }

    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

    // FIX ME: Why need to set _orderOfArrival to 0??
    // Please refer to https://github.com/cocos2d/cocos2d-x/pull/6920
    // reset for next frame
    // _orderOfArrival = 0;
}

cocos2d::Mat4 BillBoard::billBoardTransform(const Mat4 &transform)
{
    auto camera = Camera::getVisitingCamera();

    const Mat4& camWorldMat = camera->getNodeToWorldTransform();
    // only calculate when the matrices changed
    if (memcmp(_camWorldMat.m, camWorldMat.m, sizeof(float) * 16) != 0 || memcmp(_mvTransform.m, transform.m, sizeof(float) * 16) != 0 || _modeDirty)
    {
        Vec3 anchorPoint(_anchorPointInPoints.x , _anchorPointInPoints.y , 0.0f);
        Mat4 localToWorld = transform;
        localToWorld.translate(anchorPoint);

        Vec3 camDir;
        switch (_mode)
        {
        case Mode::VIEW_POINT_ORIENTED:
            camDir = Vec3(localToWorld.m[12] - camWorldMat.m[12], localToWorld.m[13] - camWorldMat.m[13], localToWorld.m[14] - camWorldMat.m[14]);
            break;
        case Mode::VIEW_PLANE_ORIENTED:
            camWorldMat.transformVector(Vec3(0.0f, 0.0f, -1.0f), &camDir);
            break;
        default:
            CCASSERT(false, "invalid billboard mode");
            break;
        }
        _modeDirty = false;

        if (camDir.length() < MATH_TOLERANCE)
        {
            camDir.set(camWorldMat.m[8], camWorldMat.m[9], camWorldMat.m[10]);
        }
        camDir.normalize();

        static Vec3 upAxis(0.0f, 1.0f, 0.0f);
        Vec3 x, y;
        camWorldMat.transformVector(upAxis, &y);
        Vec3::cross(camDir, y, &x);
        x.normalize();
        Vec3::cross(x, camDir, &y);
        y.normalize();

        float xlen = sqrtf(localToWorld.m[0] * localToWorld.m[0] + localToWorld.m[1] * localToWorld.m[1] + localToWorld.m[2] * localToWorld.m[2]);
        float ylen = sqrtf(localToWorld.m[4] * localToWorld.m[4] + localToWorld.m[5] * localToWorld.m[5] + localToWorld.m[6] * localToWorld.m[6]);
        float zlen = sqrtf(localToWorld.m[8] * localToWorld.m[8] + localToWorld.m[9] * localToWorld.m[9] + localToWorld.m[10] * localToWorld.m[10]);

        _billboardTransform.m[0] = x.x * xlen; _billboardTransform.m[1] = x.y * xlen; _billboardTransform.m[2] = x.z * xlen;
        _billboardTransform.m[4] = y.x * ylen; _billboardTransform.m[5] = y.y * ylen; _billboardTransform.m[6] = y.z * ylen;
        _billboardTransform.m[8] = -camDir.x * zlen; _billboardTransform.m[9] = -camDir.y * zlen; _billboardTransform.m[10] = -camDir.z * zlen;
        _billboardTransform.m[12] = localToWorld.m[12]; _billboardTransform.m[13] = localToWorld.m[13]; _billboardTransform.m[14] = localToWorld.m[14];

        _billboardTransform.translate(-anchorPoint);

        const Mat4 &viewMat = camWorldMat.getInversed();
        _zDepthInView = -(viewMat.m[2] * _billboardTransform.m[12] + viewMat.m[6] * _billboardTransform.m[13] + viewMat.m[10] * _billboardTransform.m[14] + viewMat.m[14]);
        _mvTransform = transform;
        _camWorldMat = camWorldMat;
    }
    return _billboardTransform;
}

NS_CC_END
