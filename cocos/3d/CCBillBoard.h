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

#ifndef __CCBILLBOARD_H__
#define __CCBILLBOARD_H__

#include "2d/CCSprite.h"

NS_CC_BEGIN

/**
 * Inherit from Sprite, achieve BillBorad.
 */
class CC_DLL BillBorad : public Sprite
{
public:

    /// @{
    /// @name Creators

    /**
     * Creates an empty BillBorad without texture. You can call setTexture method subsequently.
     *
     * @return An autoreleased BillBorad object.
     */
    static BillBorad* create();

    /**
     * Creates a BillBorad with an image filename.
     *
     * After creation, the rect of BillBorad will be the size of the image,
     * and the offset will be (0,0).
     *
     * @param   filename A path to image file, e.g., "scene1/monster.png"
     * @return  An autoreleased BillBorad object.
     */
    static BillBorad* create(const std::string& filename);

    /**
     * Creates a BillBorad with an image filename and a rect.
     *
     * @param   filename A path to image file, e.g., "scene1/monster.png"
     * @param   rect     A subrect of the image file
     * @return  An autoreleased BillBorad object
     */
    static BillBorad* create(const std::string& filename, const Rect& rect);

     /**
     * Creates a BillBorad with a Texture2D object.
     *
     * After creation, the rect will be the size of the texture, and the offset will be (0,0).
     *
     * @param   texture    A pointer to a Texture2D object.
     * @return  An autoreleased BillBorad object
     */
    static BillBorad* createWithTexture(Texture2D *texture);

    /**
     * Creates a BillBorad with a texture and a rect.
     *
     * After creation, the offset will be (0,0).
     *
     * @param   texture    A pointer to an existing Texture2D object.
     *                      You can use a Texture2D object for many sprites.
     * @param   rect        Only the contents inside the rect of this texture will be applied for this sprite.
     * @param   rotated     Whether or not the rect is rotated
     * @return  An autoreleased BillBorad object
     */
    static BillBorad* createWithTexture(Texture2D *texture, const Rect& rect, bool rotated=false);

    /**
     * Creates a BillBorad with an sprite frame.
     *
     * @param   spriteFrame    A sprite frame which involves a texture and a rect
     * @return  An autoreleased BillBorad object
     */
    static BillBorad* createWithSpriteFrame(SpriteFrame *spriteFrame);

    /**
     * Creates a BillBorad with an sprite frame name.
     *
     * A SpriteFrame will be fetched from the SpriteFrameCache by spriteFrameName param.
     * If the SpriteFrame doesn't exist it will raise an exception.
     *
     * @param   spriteFrameName A null terminated string which indicates the sprite frame name.
     * @return  An autoreleased BillBorad object
     */
    static BillBorad* createWithSpriteFrameName(const std::string& spriteFrameName);

    /** draw BillBorad object */
    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;


CC_CONSTRUCTOR_ACCESS:
    BillBorad();
    virtual ~BillBorad();

protected:
    Mat4      _BillBoradMat;

    void calculateBillBoradMatrix(Mat4 &dst);

private:
    CC_DISALLOW_COPY_AND_ASSIGN(BillBorad);

};



NS_CC_END

#endif // __CCBILLBOARD_H__