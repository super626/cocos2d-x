#ifndef SLICE_3D_H
#define SLICE_3D_H

#include "2d/CCNode.h"
#include "2d/CCSprite.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCTexture2D.h"
#include "renderer/CCTextureCache.h"
#include <vector>
#include "cocos2d.h"
NS_CC_BEGIN

class CC_DLL Slice3D :public Sprite
{
public:
    
    //set the BillBoardMode
    //default value is BILLBOARD_NONE
    enum class BillBoardMode
    {
        BILLBOARD_NONE,
        BILLBOARD_VIEW_POINT_ORIENTED,
        BILLBOARD_VIEW_PLANE_ORIENTED
    };

    //CullFaceMode
    //default value is CULL_BACK
    enum class CullFaceMode
    {
        CULL_BACK,
        CULL_FRONT,
        CULL_NONE
    };

    //set the texcoord of the slice
    //TOP_LEFT default value is (0,0)
    //BOTTTOM_LEFT default value is (0,1)
    //BOTTOM_RIGHT default value is (1,1)
    //TOP_RIGHT default value is (1,0) 
    enum class TexCoord
        {
            TOP_LEFT=0,
            BOTTOM_LEFT,
            BOTTOM_RIGHT,
            TOP_RIGHT,
        };
    struct sliceVertexData
    {
    private:
        sliceVertexData(Vec3 v1 ,Tex2F v2)
        {
            position = v1;
            texcoord = v2;
        };
        bool compareAndUpdate(V3F_C4B_T2F value);
        cocos2d::Vec3 position;
    public:
        cocos2d::Tex2F texcoord;

        friend Slice3D;
    };
    //create an slice
    //params:
    //img_file the texture file
    //size slice's size, default value is cocos2d::Size(2,2)
    //return: the instance of Slice3D
    static Slice3D * create(const char * img_file, cocos2d::Size size = Size(2,2));
    
    //set the BillBoardMode
    //the default value is BILLBOARD_NONE
    void setBillBoardMode(BillBoardMode mode);

    //set the CullFaceMode 
    //use to determine which face (front or back or none) should be culled,
    //when the specified face is culled,that face is not drawn anymore
    void setCullFaceMode(CullFaceMode mode);

    //set the slice look to some direction
    //params:
    //direction the direction that slice look to
    //up the up vector that slice used.
    //
    // 
    //NOTICE: you can not set such up vector which are parallel to the direction vector
    void lookTo(const Vec3 &direction,const Vec3 &up);

    //set the slice's size
    void setSize(const Size &size);

    //get the slice's size
    // 
    // 
    //NOTICE : this size is not the texture's size, it the slice real size in 3D scene
    const Size& getSize();

    //get the specified corner's texture coordinate
    //params:
    //index one of the {TOP_LEFT , BOTTOM_LEFT , BOTTOM_RIGHT , TOP_RIGHT } should be used
    const cocos2d::Tex2F & getTexCoord(TexCoord index);

    //get the specified corner's texture coordinate
    //params:
    //index one of the {TOP_LEFT , BOTTOM_LEFT , BOTTOM_RIGHT , TOP_RIGHT } should be used
    // 
    // 
    //NOTICE: if you use this method to manually set texcoords , you can't let the slice play common animation 
    //at the same time because the texcoord will be overwrite , the result is unexpected . also if you play
    //animation, the texcoords will be overwrite too.
    void setTexCoord(TexCoord index,const Tex2F &newcoord);

    // Overrides, internal use only
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
    // Overrides, internal use only
    virtual void setTextureRect(const Rect& rect, bool rotated, const Size& untrimmedSize);
private:
    void calculateBillBoardMatrix(const Mat4 &transform,BillBoardMode mode);
    Mat4 _mvTransform;
    Mat4 _camWorldMat;
    Mat4 _billboardTransform;
    float recaculateZorder(const Mat4 &viewMatrix, const Mat4 &modelTransform);
    void updateAttribute();
    void updateColor();
    void updateCullFace();
    void initAttributeList(cocos2d::Size size);
    std::vector<sliceVertexData> _attributeList;
    void Init();
    GLuint vbo[2];
    void onDraw(const Mat4 &transform, uint32_t flags);
    CustomCommand _customCommand;
    Slice3D();
    bool _isUseSpriteUv;
    bool _isAttributeDirty;
    BillBoardMode _billboardMode;
    bool _billboardModeDirty;
    CullFaceMode _cullFaceMode;
    Size _size;
};

NS_CC_END
#endif