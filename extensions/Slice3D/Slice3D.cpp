#include "Slice3D.h"
#include <vector>
USING_NS_CC;


#define PI_OVER_2 (3.1415927 /2.0)
#define DEGREE(x) ((180.0/3.1415927)* x )
using namespace std;
static const char * vertex_shader = "\
attribute vec4 a_position;\
attribute vec2 a_texCoord;\
\n#ifdef GL_ES\n\
varying mediump vec2 v_texCoord;\
\n#else\n\
varying vec2 v_texCoord;\
\n#endif\n\
void main()\
{\
    gl_Position = CC_MVPMatrix * a_position;\
    v_texCoord = a_texCoord;\
}\
"; 
static const char * fragment_shader ="\n#ifdef GL_ES\n\
precision lowp float;\
\n#endif\n\
uniform vec3 u_color;\
varying vec2 v_texCoord;\
void main()\
{\
    gl_FragColor =  texture2D(CC_Texture0, v_texCoord)*vec4(u_color,1.0);\
}";

Slice3D * Slice3D::create(const char * img_file,cocos2d::Size size )
{
     Slice3D * obj = new (std::nothrow) Slice3D();
     if (obj && obj->initWithFile(img_file))
     {
         obj->initAttributeList(size);
         obj->Init();
         obj->autorelease();
         return obj;
     }
     CC_SAFE_DELETE(obj);
    return obj;
}


Slice3D::Slice3D()
{
   // _mesh = Mesh::create();
}

void Slice3D::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    updateAttribute();
    float zorder;
    Mat4 ModelViewTransform;
    if (_billboardMode ==cocos2d::Slice3D::BillBoardMode::BILLBOARD_NONE)
    {
    auto viewMatrix = Camera::getVisitingCamera()->getViewMatrix();
    zorder = recaculateZorder(viewMatrix,transform);
    ModelViewTransform = transform;
    }
    else
    {
    calculateBillBoardMatrix(transform,_billboardMode);
    zorder = recaculateZorder(_camWorldMat.getInversed(),_billboardTransform);
    ModelViewTransform = _billboardTransform;
    }
    _customCommand.init(zorder);
    _customCommand.func = CC_CALLBACK_0(Slice3D::onDraw, this, ModelViewTransform, flags);
    _customCommand.setTransparent(true);
    renderer->addCommand(&_customCommand);
}

void Slice3D::onDraw(const Mat4 &transform, uint32_t flags)
{
    auto glProgram = getGLProgram();
    glProgram->use();
    glProgram->setUniformsForBuiltins(transform);
    updateColor();
    updateCullFace();
    auto blendfunc =  this->getBlendFunc();
    glBlendFunc(blendfunc.src,blendfunc.dst);
    glBlendEquation(GL_FUNC_ADD);
    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,this->getTexture()->getName());
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    if(_isAttributeDirty)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(sliceVertexData)* 4, &_attributeList[0], GL_STREAM_DRAW);
        _isAttributeDirty = false;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_TEX_COORD);
    unsigned int offset = 0;
    //position
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(sliceVertexData), (GLvoid *)offset);
    offset +=sizeof(Vec3);
    //texcoord
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD,2,GL_FLOAT,GL_FALSE,sizeof(sliceVertexData),(GLvoid *)offset);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void cocos2d::Slice3D::Init()
{
    auto shader = GLProgram::createWithByteArrays(vertex_shader,fragment_shader);
    auto state = GLProgramState::create(shader);
    setGLProgramState(state);

    GLushort indices []={0,1,2,2,3,0}; //6 indices
    glGenBuffers(2,vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sliceVertexData)* 4, &_attributeList[0], GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
    CHECK_GL_ERROR_DEBUG();
    glBindBuffer(GL_ARRAY_BUFFER,0);
    this->setContentSize(_size);
    _cullFaceMode = CullFaceMode::CULL_BACK;
    _billboardMode = BillBoardMode::BILLBOARD_NONE;
    _billboardModeDirty = false;
}

void cocos2d::Slice3D::initAttributeList(Size size)
{
    _attributeList.push_back(sliceVertexData(Vec3(-size.width/2,size.height/2,0),Tex2F(0,0))); // 0 TOP_LEFT
    _attributeList.push_back(sliceVertexData(Vec3(-size.width/2,-size.height/2,0),Tex2F(0,1))); // 1 BOTTOM_LEFT
    _attributeList.push_back(sliceVertexData(Vec3(size.width/2,-size.height/2,0),Tex2F(1,1))); // 2 BOTTOM_RIGHT
    _attributeList.push_back(sliceVertexData(Vec3(size.width/2,size.height/2,0),Tex2F(1,0))); // 3 TOP_RIGHT
    _isAttributeDirty=false;
    _size = size;
}

void cocos2d::Slice3D::updateAttribute()
{
    if(_isUseSpriteUv)
    {
        _isAttributeDirty|= _attributeList[static_cast<int>(TexCoord::TOP_LEFT)].compareAndUpdate(_quad.tl);
        _isAttributeDirty|= _attributeList[static_cast<int>(TexCoord::BOTTOM_LEFT)].compareAndUpdate(_quad.bl);
        _isAttributeDirty|= _attributeList[static_cast<int>(TexCoord::BOTTOM_RIGHT)].compareAndUpdate(_quad.br);
        _isAttributeDirty|= _attributeList[static_cast<int>(TexCoord::TOP_RIGHT)].compareAndUpdate(_quad.tr);
    }
}

void cocos2d::Slice3D::setTextureRect(const Rect& rect, bool rotated, const Size& untrimmedSize)
{
    //we need override it ,cause the slice3D's contentsize is different from the sprite.
    Sprite::setTextureRect(rect,rotated,untrimmedSize);
    setContentSize(_size);
    _isUseSpriteUv = true;
}

void cocos2d::Slice3D::setBillBoardMode(BillBoardMode mode)
{
    _billboardMode = mode;
    _billboardModeDirty =true;
}

void cocos2d::Slice3D::setCullFaceMode(CullFaceMode mode)
{
    _cullFaceMode = mode;
}

void cocos2d::Slice3D::calculateBillBoardMatrix(const Mat4 &transform,BillBoardMode mode)
{
    auto camera = Camera::getVisitingCamera();
    const Mat4& camWorldMat = camera->getNodeToWorldTransform();
    if (memcmp(_camWorldMat.m, camWorldMat.m, sizeof(float) * 16) != 0 || memcmp(_mvTransform.m, transform.m, sizeof(float) * 16) != 0 || _billboardModeDirty)
    {
        Vec3 anchorPoint(_anchorPointInPoints.x , _anchorPointInPoints.y , 0.0f);
        Mat4 localToWorld = transform;
        localToWorld.translate(anchorPoint);
        Vec3 camDir;
        switch (mode)
        {
        case BillBoardMode::BILLBOARD_VIEW_POINT_ORIENTED:
            camDir = Vec3(localToWorld.m[12] - camWorldMat.m[12], localToWorld.m[13] - camWorldMat.m[13], localToWorld.m[14] - camWorldMat.m[14]);
            break;
        case BillBoardMode::BILLBOARD_VIEW_PLANE_ORIENTED:
            camWorldMat.transformVector(Vec3(0.0f, 0.0f, -1.0f), &camDir);
            break;
        default:
            CCASSERT(false, "invalid billboard mode");
            break;
        }
        _billboardModeDirty = false;
        if (camDir.length() < MATH_TOLERANCE)
        {
            camDir.set(camWorldMat.m[8], camWorldMat.m[9], camWorldMat.m[10]);
        }
        camDir.normalize();
        Quaternion rotationQuaternion;
        this->getNodeToWorldTransform().getRotation(&rotationQuaternion);
        // fetch the rotation angle of z
        float rotationZ = atan2(2*(rotationQuaternion.w*rotationQuaternion.z + rotationQuaternion.x*rotationQuaternion.y),
            (1 - 2* (rotationQuaternion.y*rotationQuaternion.y + rotationQuaternion.z *rotationQuaternion.z)));
        Mat4 rotationMatrix;
        rotationMatrix.setIdentity();
        rotationMatrix.rotateZ(rotationZ);
        Vec3 upAxis = Vec3(rotationMatrix.m[4],rotationMatrix.m[5],rotationMatrix.m[6]);
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
        _mvTransform = transform;
        _camWorldMat = camWorldMat;
    }
}

float cocos2d::Slice3D::recaculateZorder(const Mat4 &viewMatrix, const Mat4 &modelTransform)
{
    float zorder ;
    zorder = -1*(viewMatrix.m[2] * modelTransform.m[12] + viewMatrix.m[6] * modelTransform.m[13] + viewMatrix.m[10] * modelTransform.m[14] + viewMatrix.m[14]);
    return zorder;
}

void cocos2d::Slice3D::lookTo(const Vec3 &direction,const Vec3 &up)
{
    Mat4 a;
    Vec3 d = direction;
    Vec3 u =up;
    d.z *=-1;
    u.z *=-1;
    Mat4::createLookAt(Vec3::ZERO,d,u,&a);
    a = a.getInversed();
    Quaternion q;
    a.getRotation(&q);

    
    Vec3 result;
    float test = q.y*q.z + q.x*q.w;
    if (test > 0.4999f)
    {
        result.z = 2.0f * atan2(q.y, q.w);
        result.y = PI_OVER_2;
        result.x = 0.0f;
    }
    else
    if (test < -0.4999f)
    {
        result.z = 2.0f * atan2(q.y, q.w);
        result.y = -PI_OVER_2;
        result.x = 0.0f;
    }
    else
    {
        float sqx = q.x * q.x;
        float sqy = q.y * q.y;
        float sqz = q.z * q.z;
        result.z = atan2(2.0f * q.z * q.w - 2.0f * q.y * q.x, 1.0f - 2.0f * sqz - 2.0f * sqx);
        result.y = asin(2.0f * test);
        result.x = atan2(2.0f * q.y * q.w - 2.0f * q.z * q.x, 1.0f - 2.0f * sqy - 2.0f * sqx);
    }
    this->setRotation3D(Vec3(DEGREE(-1*result.y),DEGREE(-1*result.x),DEGREE(-1*result.z)));
}

void cocos2d::Slice3D::setSize(const Size &size)
{
    _size = size;
    _attributeList[static_cast<int>(TexCoord::TOP_LEFT)].position = Vec3(-size.width/2,size.height/2,0);
    _attributeList[static_cast<int>(TexCoord::BOTTOM_LEFT)].position = Vec3(-size.width/2,-size.height/2,0);
    _attributeList[static_cast<int>(TexCoord::BOTTOM_RIGHT)].position = Vec3(size.width/2,-size.height/2,0);
    _attributeList[static_cast<int>(TexCoord::TOP_RIGHT)].position = Vec3(size.width/2,size.height/2,0);
    _isAttributeDirty = true;
}

const Size &cocos2d::Slice3D::getSize()
{
    return _size;
}

const cocos2d::Tex2F & cocos2d::Slice3D::getTexCoord(TexCoord index)
{
    return _attributeList[(int)index].texcoord;
}

void cocos2d::Slice3D::setTexCoord(TexCoord index,const Tex2F &newcoord)
{
    _attributeList[(int)index].texcoord = newcoord;
    _isAttributeDirty = true;
    _isUseSpriteUv = false;
}

void cocos2d::Slice3D::updateColor()
{
    auto glProgram = getGLProgram();
    auto color = this->getColor();
    auto  location = glGetUniformLocation(glProgram->getProgram(),"u_color");
    glUniform3f(location,color.r*1.0f/255,color.g*1.0f/255,color.b*1.0f/255);
}

void cocos2d::Slice3D::updateCullFace()
{
    glEnable(GL_CULL_FACE);
    switch (_cullFaceMode)
    {
    case cocos2d::Slice3D::CullFaceMode::CULL_BACK:
        glCullFace(GL_BACK);
        break;
    case cocos2d::Slice3D::CullFaceMode::CULL_FRONT:
        glCullFace(GL_FRONT);
        break;
    case cocos2d::Slice3D::CullFaceMode::CULL_NONE:
        glDisable(GL_CULL_FACE);
        break;
    default:
        break;
    }
}

bool cocos2d::Slice3D::sliceVertexData::compareAndUpdate(V3F_C4B_T2F value)
{
    if(this->texcoord.u == value.texCoords.u  &&  texcoord.v == value.texCoords.v)
    {
        return false;
    }
    else
    {
        texcoord = value.texCoords;
        return true;
    }
}
