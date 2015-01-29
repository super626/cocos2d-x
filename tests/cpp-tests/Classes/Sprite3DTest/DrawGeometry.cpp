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

#include "DrawGeometry.h"

NS_CC_BEGIN


DrawGeometry::DrawGeometry()
: _vao(0)
, _vbo(0)
, _bufferCapacity(0)
, _bufferCount(0)
, _buffer(nullptr)
, _dirty(false)
,_enableTexture(false)
,_texture(nullptr)
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
}

DrawGeometry::~DrawGeometry()
{
    free(_buffer);
    _buffer = nullptr;
    
    glDeleteBuffers(1, &_vbo);
    _vbo = 0;
    
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        glDeleteVertexArrays(1, &_vao);
        GL::bindVAO(0);
        _vao = 0;
    }
}

DrawGeometry* DrawGeometry::create()
{
    DrawGeometry* ret = new (std::nothrow) DrawGeometry();
    if (ret && ret->init())
    {
        ret->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(ret);
    }
    
    return ret;
}

void DrawGeometry::ensureCapacity(int count)
{
    CCASSERT(count>=0, "capacity must be >= 0");
    
    if(_bufferCount + count > _bufferCapacity)
    {
		_bufferCapacity += MAX(_bufferCapacity, count);
		_buffer = (V3F_C4B*)realloc(_buffer, _bufferCapacity*sizeof(V3F_C4B));
	}
}

bool DrawGeometry::init()
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

    setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));
    
    ensureCapacity(512);
    
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        glGenVertexArrays(1, &_vao);
        GL::bindVAO(_vao);
    }
    
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B)* _bufferCapacity, _buffer, GL_STREAM_DRAW);
    
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B), (GLvoid *)offsetof(V3F_C4B, vertices));
    
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_COLOR);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B), (GLvoid *)offsetof(V3F_C4B, colors));

    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD,2,GL_FLOAT,GL_FALSE,sizeof(V3F_C4B),(GLvoid *)offsetof(V3F_C4B, texcoord));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(0);
    }
    
    CHECK_GL_ERROR_DEBUG();
    
    _dirty = true;
    
#if CC_ENABLE_CACHE_TEXTURE_DATA
    // Need to listen the event only when not use batchnode, because it will use VBO
    auto listener = EventListenerCustom::create(EVENT_COME_TO_FOREGROUND, [this](EventCustom* event){
    /** listen the event that coming to foreground on Android */
        this->init();
    });

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#endif
    
    return true;
}

void DrawGeometry::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    _customCommand.init(_globalZOrder);
    _customCommand.func = CC_CALLBACK_0(DrawGeometry::onDraw, this, transform, flags);
    renderer->addCommand(&_customCommand);
}

void DrawGeometry::onDraw(const Mat4 &transform, uint32_t flags)
{
    auto glProgram = getGLProgram();
    glProgram->use();
    glProgram->setUniformsForBuiltins(transform);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    if(_enableTexture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,_texture->getName());
    }

    GL::blendFunc(_blendFunc.src, _blendFunc.dst);

    if (_dirty)
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B)*_bufferCapacity, _buffer, GL_STREAM_DRAW);
        _dirty = false;
    }
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(_vao);
    }
    else
    {
        GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        // vertex
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B), (GLvoid *)offsetof(V3F_C4B, vertices));

        // color
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B), (GLvoid *)offsetof(V3F_C4B, colors));

        //texcoord
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD,2,GL_FLOAT,GL_FALSE,sizeof(V3F_C4B),(GLvoid *)offsetof(V3F_C4B, texcoord));
    }

    glDrawArrays(GL_TRIANGLES, 0, _bufferCount);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1,_bufferCount);
	glDisable(GL_DEPTH_TEST);
    CHECK_GL_ERROR_DEBUG();
}

void DrawGeometry::drawSlice(Vec3* points, const Color4F &color)
{
    unsigned int vertex_count = 6;
    ensureCapacity(vertex_count);
    
    Color4B col = Color4B(color);
    V3F_C4B a = {Vec3(points[0].x, points[0].y, points[0].z), col, Tex2F(0,0)};
    V3F_C4B b = {Vec3(points[1].x, points[1].y, points[1].z), col, Tex2F(1,0)};
    V3F_C4B c = {Vec3(points[2].x, points[2].y, points[2].z), col, Tex2F(1,1)};
    V3F_C4B d = {Vec3(points[3].x, points[3].y, points[3].z), col, Tex2F(0,1)};
    V3F_C4B e = {Vec3(points[0].x, points[0].y, points[0].z), col, Tex2F(0,0)};
    V3F_C4B f = {Vec3(points[2].x, points[2].y, points[2].z), col, Tex2F(1,1)};
    
    V3F_C4B *slice = (V3F_C4B *)(_buffer + _bufferCount);
    slice[0] = a;
    slice[1] = b;
    slice[2] = c;
    slice[3] = d;
    slice[4] = e;
    slice[5] = f;
    
    _bufferCount += vertex_count;
    _dirty = true;
}

void DrawGeometry::drawCube(Vec3* vertices, const Color4F &color)
{
    // front face
    Vec3 ponitsf[4] ={vertices[0],vertices[1],vertices[2],vertices[3]};
    drawSlice(ponitsf,color);
    
    // back face
    Vec3 ponitsb[4] ={vertices[7],vertices[6],vertices[5],vertices[4]};
    drawSlice(ponitsb,color);

    // left face
    Vec3 ponitsl[4] ={vertices[7],vertices[0],vertices[3],vertices[4]};
    drawSlice(ponitsl,color);

    // right face
    Vec3 ponitsr[4] ={vertices[1],vertices[6],vertices[5],vertices[2]};
    drawSlice(ponitsr,color);

    // top face
    Vec3 ponitst[4] ={vertices[7],vertices[6],vertices[1],vertices[0]};
    drawSlice(ponitst,color);

    // bottom face
    Vec3 ponitso[4] ={vertices[4],vertices[5],vertices[2],vertices[3]};
    drawSlice(ponitso,color);
    
}

void DrawGeometry::drawSphere(const Vec3 &origin, const float radius, const float M, const float N, const Color4F &color)
{
    float step_y = M_PI/M;
    float step_xz = 2*M_PI/N;

    Vec3 points[4];
    float angle_y = 0.0;
    float angle_xz = 0.0;
    int i=0, j=0;

    for(i=0; i<M; i++)
    {
        angle_y = i * step_y;
   
        for(j=0; j<N; j++)
        {
            angle_xz = j * step_xz;
            points[0].x = radius * sin(angle_y) * cos(angle_xz);
            points[0].z = radius * sin(angle_y) * sin(angle_xz);
            points[0].y = radius * cos(angle_y);
            points[1].x = radius * sin(angle_y + step_y) * cos(angle_xz);
            points[1].z = radius * sin(angle_y + step_y) * sin(angle_xz);
            points[1].y = radius * cos(angle_y + step_y);
            points[2].x = radius*sin(angle_y + step_y)*cos(angle_xz + step_xz);
            points[2].z = radius*sin(angle_y + step_y)*sin(angle_xz + step_xz);
            points[2].y = radius*cos(angle_y + step_y);
            points[3].x = radius * sin(angle_y) * cos(angle_xz + step_xz);
            points[3].z = radius * sin(angle_y) * sin(angle_xz + step_xz);
            points[3].y = radius * cos(angle_y);

            points[0] += origin;
            points[1] += origin;
            points[2] += origin;
            points[3] += origin;

            drawSlice(points,color);        
        }
    }

}

void DrawGeometry::drawCylinder(const float radius, const float length, const int blocks, const float sectorBlocks, const Color4F &color)
{
    Vec3 points[4];
    float len=length/blocks;
    float sectorAngle=360/sectorBlocks;

    for(int i=0;i<blocks;i++)
    {
        for(float angle=360;angle>0;angle=angle-sectorAngle)
        {
            //left top
            float x1=-(length/2-i*len);
            float y1=(float) (radius*sin(CC_DEGREES_TO_RADIANS(angle)));
            float z1=(float) (radius*cos(CC_DEGREES_TO_RADIANS(angle)));

            //right top
            float x2=x1+len;
            float y2=y1;
            float z2=z1;

            //right bottom
            float x3=x2;
            float y3=(float) (radius*sin(CC_DEGREES_TO_RADIANS(angle-sectorAngle)));
            float z3=(float) (radius*cos(CC_DEGREES_TO_RADIANS(angle-sectorAngle)));

            //left bottom
            float x4=x1;
            float y4=y3;
            float z4=z3;

            points[0] = Vec3(x1,y1,z1);
            points[1] = Vec3(x2,y2,z2);
            points[2] = Vec3(x3,y3,z3);
            points[3] = Vec3(x4,y4,z4);

            drawSlice(points,color); 

        }
    }
}

void DrawGeometry::setTexture(const std::string &filename)
{
    Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(filename);
    _texture = texture;
    _enableTexture = true;
}

void DrawGeometry::clear()
{
    _bufferCount = 0;
    _dirty = true;
}

const BlendFunc& DrawGeometry::getBlendFunc() const
{
    return _blendFunc;
}

void DrawGeometry::setBlendFunc(const BlendFunc &blendFunc)
{
    _blendFunc = blendFunc;
}

NS_CC_END
