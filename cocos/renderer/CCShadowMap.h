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

#ifndef __CC_SHADOWMAP_H__
#define __CC_SHADOWMAP_H__

#include "base/ccMacros.h"
#include "base/CCRef.h"
#include "math/CCMath.h"
#include "renderer/CCFrameBuffer.h"

NS_CC_BEGIN

/**
 * @addtogroup _renderer
 * @{
 */

class BaseLight;

/**
 * Shadow map
 */
class CC_DLL ShadowMap : public Ref
{

public:
    
    static ShadowMap* create(int width, int height);
    
    /**bind shadow map before visit 3d queue*/
    void bind();
    /**unbind shadow map after visit 3d queue*/
    void unBind();
    
    /**
     * Constructor.
     *
     */
    ShadowMap();
    
    ~ShadowMap();
    
    bool init(int width, int height);
    
    GLuint getTextureName() const;
    
    unsigned int getShadowMapWidth() const;
    
    unsigned int getShadowMapHeight() const;
    
    const Mat4& getLightViewProjectionMat();

protected:
    
    
    experimental::FrameBuffer* _frameBuffer;
    GLuint                     _shaodowmapTexture;
    GLint                      _viewport[4];//old viewport
    BaseLight*                 _shadowLight; //light cast shadow
    Mat4                       _projectionMat;
    Mat4                       _lightViewMat;
    Mat4                       _lightWorldMat;
    Mat4                       _lightViewProjectionMat;
};

// end of renderer group
/// @}

NS_CC_END

#endif
