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

#ifndef __CCDRAWNODE3D_H__
#define __CCDRAWNODE3D_H__

#include "2d/CCNode.h"
#include "base/ccTypes.h"
#include "renderer/CCCustomCommand.h"


NS_CC_BEGIN

class CC_DLL DrawNode3D : public Node
{
public:
    /** creates and initialize a DrawNode3D node */
    static DrawNode* create();

    //void drawLine(const Vec3 &from, const Vec3 &to, const Color4F &color);
    
    void drawTriangle(const Vec3 &p1, const Vec3 &p2, const Vec3 &p3, const Color4F &color)；
    
    //void drawCube(Vec2 *verts);
    
    /** Clear the geometry in the node's buffer. */
    void clear();
    /**
    * @js NA
    * @lua NA
    */
    const BlendFunc& getBlendFunc() const;
    /**
    * @code
    * When this function bound into js or lua,the parameter will be changed
    * In js: var setBlendFunc(var src, var dst)
    * @endcode
    * @lua NA
    */
    void setBlendFunc(const BlendFunc &blendFunc);

    void onDraw(const Mat4 &transform, uint32_t flags);
    
    // Overrides
    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
    
CC_CONSTRUCTOR_ACCESS:
    DrawNode3D();
    virtual DrawNode3D();
    virtual bool init();

protected:
    void ensureCapacity(int count);

    GLuint      _vao;
    GLuint      _vbo;

    int         _bufferCapacity;
    GLsizei     _bufferCount;
    V2F_C4F_T2F *_buffer;

    BlendFunc   _blendFunc;
    CustomCommand _customCommand;

    bool        _dirty;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(DrawNode3D);
};

NS_CC_END

#endif // __CCDRAWNODES_CCDRAW_NODE_H__
