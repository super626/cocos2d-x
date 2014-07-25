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
#include "3d/CCCamera3D.h"
#include "base/CCDirector.h"
#include "3d/CCRay.h"
NS_CC_BEGIN
    Camera3D* Camera3D::_activeCamera = nullptr;
Camera3D* Camera3D::create()
{
    auto ret = new Camera3D();
    if (ret)
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
Camera3D* Camera3D::createPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
    auto ret = new Camera3D(fieldOfView, aspectRatio, nearPlane, farPlane);
    if (ret)
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
Camera3D*  Camera3D::getActiveCamera()
{
    return _activeCamera;
}
Camera3D* Camera3D::createOrthographic(float zoomX, float zoomY, float aspectRatio, float nearPlane, float farPlane)
{
    auto ret = new Camera3D(zoomX, zoomY, aspectRatio, nearPlane, farPlane);
    if (ret)
    {  
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
Camera3D::Type Camera3D::getCameraType() const
{
    return _type;
}
Camera3D::Camera3D(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
    :_type(PERSPECTIVE), _fieldOfView(fieldOfView), _aspectRatio(aspectRatio), _nearPlane(nearPlane), _farPlane(farPlane)
{

}

Camera3D::Camera3D(float zoomX, float zoomY, float aspectRatio, float nearPlane, float farPlane)
    :_type(ORTHOGRAPHIC), _aspectRatio(aspectRatio), _nearPlane(nearPlane), _farPlane(farPlane)
{
    // Orthographic camera.
    _zoom[0] = zoomX;
    _zoom[1] = zoomY;
    _transform= Mat4::IDENTITY;
}
Camera3D::Camera3D()
{
}
Camera3D::~Camera3D()
{
}

void Camera3D::setPosition3D(const Vec3& position)
{
    Node::setPosition3D(position);

    // Mat4::createLookAt(getPosition3D(),_lookAtPos,_up, &_view);
    _transformUpdated = _transformDirty = _inverseDirty = true;
}
void Camera3D::setRotation3D(const Vec3& rotation)
{
    Node::setRotation3D(rotation);
    _transformUpdated = _transformDirty = _inverseDirty = true;	
}
//set active camera 
bool Camera3D::setActiveCamera()
{
    _activeCamera=this;
    return false;
}
void Camera3D::lookAt(const Vec3& position, const Vec3& up, const Vec3& lookAtPos)
{
    _lookAtPos=lookAtPos;
    _up=up;
    Vec3 upv = up;
    upv.normalize();
    Vec3 zaxis;
    Vec3::subtract(position, lookAtPos, &zaxis);
    zaxis.normalize();

    Vec3 xaxis;
    Vec3::cross(upv, zaxis, &xaxis);
    xaxis.normalize();

    Vec3 yaxis;
    Vec3::cross(zaxis, xaxis, &yaxis);
    yaxis.normalize();

    _rotation.m[0] = xaxis.x;
    _rotation.m[1] = xaxis.y;
    _rotation.m[2] = xaxis.z;
    _rotation.m[3] = 0;

    _rotation.m[4] = yaxis.x;
    _rotation.m[5] = yaxis.y;
    _rotation.m[6] = yaxis.z;
    _rotation.m[7] = 0;

    _rotation.m[8] = zaxis.x;
    _rotation.m[9] = zaxis.y;
    _rotation.m[10] = zaxis.z;
    _rotation.m[11] = 0;
    Node::setPosition3D(position);
    //Mat4::createLookAt(position,lookAtPos,up, &_view);
    _transformDirty=true;
}
Mat4& Camera3D::getProjectionMatrix() 
{
    if (_type == PERSPECTIVE)
    {
        Mat4::createPerspective(_fieldOfView, _aspectRatio, _nearPlane, _farPlane, &_projection);
    }
    else
    {
        Mat4::createOrthographic(_zoom[0], _zoom[1], _nearPlane, _farPlane, &_projection);
    }
    return _projection;
}
Mat4& Camera3D::getViewMatrix()
{

    _view=getNodeToWorldTransform().getInversed();	
    return _view;
}
void Camera3D::applyProjection()
{
    getProjectionMatrix();
    getViewMatrix();
    Director* director = Director::getInstance();
    director->loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
#if CC_TARGET_PLATFORM == CC_PLATFORM_WP8
    //if needed, we need to add a rotation for Landscape orientations on Windows Phone 8 since it is always in Portrait Mode
    GLView* view = getOpenGLView();
    if(getOpenGLView() != nullptr)
    {
        multiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, getOpenGLView()->getOrientationMatrix());
    }
#endif
    // issue #1334
    //Mat4::createPerspective(60, (GLfloat)size.width/size.height, 10, 1000, &matrixPerspective);
    director->multiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, _projection);
    director->multiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, _view);
    director->loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    GL::setProjectionMatrixDirty();
}
/* returns the Eye value of the Camera */
Vec3& Camera3D::getEyePos() 
{
    Mat4 mat=getNodeToWorldTransform();
    _realEyePos= Vec3(mat.m[12],mat.m[13],mat.m[14]);
    return 	  _realEyePos;
}
Vec3& Camera3D::getLookPos() 
{
    return _lookAtPos;
}
const Mat4& Camera3D::getNodeToParentTransform() const
{
    if (_transformDirty)
    {
        Mat4::createTranslation(getPosition3D(), &_transform);
        _transform.rotate(_rotation);
        _transformDirty = false;
    }
    return _transform;
}

void Camera3D::rotate(const Vec3& axis,float angle)
{
    Vec3  cameraPos=getPosition3D();
    Vec3  cameradir=_lookAtPos-cameraPos;
    float length=cameradir.length();
    cameradir.normalize();
    Mat4 rotMat;
    Mat4::createRotation(axis,CC_DEGREES_TO_RADIANS(angle),&rotMat);
    rotMat.transformVector(&cameradir);
    _lookAtPos=cameraPos+ cameradir*length;
    lookAt(cameraPos,_up,_lookAtPos);
}
void Camera3D::rotateAlong(const Vec3& point,const Vec3& axis, float angle)
{
    Vec3  cameraPos=getPosition3D()-point;
    Vec3  cameradir=cameraPos-_lookAtPos;
    float length=cameradir.length();
    cameradir.normalize();
    Mat4 rotMat;
    Mat4::createRotation(axis,CC_DEGREES_TO_RADIANS(angle),&rotMat);
    rotMat.transformVector(&cameradir);
    cameraPos=_lookAtPos+ cameradir*length+point;
    lookAt(cameraPos,_up,_lookAtPos);
}
void Camera3D::scale(float scale)
{
    Vec3  cameraPos=getPosition3D();
    Vec3  cameradir=_lookAtPos-cameraPos;
    cameradir.normalize();
    cameraPos+=cameradir*scale;
    lookAt(cameraPos,_up,_lookAtPos);
}
void Camera3D::translate(const Vec3& vector)
{

    Vec3  cameraPos=getPosition3D();
    Vec3  cameradir=_lookAtPos-cameraPos;
    Vec3  rightdir;
    Vec3::cross(Vec3(0,1,0),cameradir,&rightdir);

    float length=cameradir.length();
    cameradir.normalize();
    cameraPos.x+=cameradir.x*vector.z;
    cameraPos.z+=cameradir.z*vector.z;
    rightdir.normalize();
    cameraPos+=rightdir*vector.x;
    _lookAtPos=cameraPos+	cameradir*length;
    lookAt(cameraPos,Vec3(0, 1, 0), _lookAtPos);
}
void Camera3D::unproject(const Mat4& viewProjection, const Size* viewport, Vec3* src, Vec3* dst)
{
    assert(dst);
    assert(viewport->width != 0.0f && viewport->height != 0.0f);
    Vec4 screen(src->x / viewport->width, ((viewport->height - src->y)) / viewport->height, src->z, 1.0f);
    screen.x = screen.x * 2.0f - 1.0f;
    screen.y = screen.y * 2.0f - 1.0f;
    screen.z = screen.z * 2.0f - 1.0f;

    viewProjection.getInversed().transformVector(screen, &screen);  
    if (screen.w != 0.0f)
    {
        screen.x /= screen.w;
        screen.y /= screen.w;
        screen.z /= screen.w;
    }

    dst->set(screen.x, screen.y, screen.z);
}
void Camera3D::calculateRayByLocationInView(Ray* ray, const Vec2& location)
{
    auto dir = Director::getInstance();
    auto view = dir->getWinSize();
    Mat4 mat = _projection*_view;
    Vec3 src = Vec3(location.x, location.y, -1);
    Vec3 nearPoint;
    unproject(mat, &view, &src, &nearPoint);

    src = Vec3(location.x, location.y, 1);
    Vec3 farPoint;
    unproject(mat, &view, &src, &farPoint);

    Vec3 direction;
    Vec3::subtract(farPoint, nearPoint, &direction);
    direction.normalize();
    ray->_origin = nearPoint;
    ray->_direction = direction;
}
NS_CC_END