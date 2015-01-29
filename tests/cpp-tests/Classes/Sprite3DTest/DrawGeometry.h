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

#ifndef __DRAW_GEOMETRY_H__
#define __DRAW_GEOMETRY_H__

#include "cocos2d.h"

NS_CC_BEGIN

class DrawGeometry: public Node
{
public:
    static DrawGeometry* create();
    
    void drawSlice(Vec3* points, const Color4F &color); 

    void drawCube(Vec3* vertices, const Color4F &color);

    void drawSphere(const Vec3 &origin, const float radius, const float M, const float N, const Color4F &color);

    void drawCylinder(const float radius, const float length, const int blocks, const float sectorBlocks, const Color4F &color);

    void setTexture(const std::string &filename );
    
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
    DrawGeometry();
    virtual ~DrawGeometry();
    virtual bool init();

protected:
    struct V3F_C4B
    {
        Vec3     vertices;
        Color4B  colors;
        Tex2F    texcoord;
    };
    void ensureCapacity(int count);

    GLuint      _vao;
    GLuint      _vbo;

    int         _bufferCapacity;
    GLsizei     _bufferCount;
    V3F_C4B*    _buffer;

    BlendFunc   _blendFunc;
    CustomCommand _customCommand;

    bool        _dirty;

    Texture2D*  _texture;
    bool        _enableTexture;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(DrawGeometry);
};

NS_CC_END

#endif // __DRAW_GEOMETRY_H__
