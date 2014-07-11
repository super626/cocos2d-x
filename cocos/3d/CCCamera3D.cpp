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
	:_dirtyBits(CAMERA_DIRTY_ALL),_type(PERSPECTIVE), _fieldOfView(fieldOfView), _aspectRatio(aspectRatio), _nearPlane(nearPlane), _farPlane(farPlane)
{

}

Camera3D::Camera3D(float zoomX, float zoomY, float aspectRatio, float nearPlane, float farPlane)
	:_dirtyBits(CAMERA_DIRTY_ALL),_type(ORTHOGRAPHIC), _aspectRatio(aspectRatio), _nearPlane(nearPlane), _farPlane(farPlane)
{
	// Orthographic camera.
	_zoom[0] = zoomX;
	_zoom[1] = zoomY;
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
	_dirtyBits |= CAMERA_DIRTY_VIEW;
}
//set active camera 
bool Camera3D::setActiveCamera()
{
	_activeCamera=this;
    return false;
}
void Camera3D::setCenter(const Vec3& center)
{
	_center = center;
}
void Camera3D::setUp(const Vec3& up)
{
	_up = up;
}
void Camera3D::translateX(float tx)
{
	_position.x+=tx;
	_center.x+=tx;
	_dirtyBits |= CAMERA_DIRTY_VIEW;
}
void Camera3D::translateY(float ty)
{
	_position.y+=ty;
	_center.y+=ty;
	_dirtyBits |= CAMERA_DIRTY_VIEW;
}
void Camera3D::translateZ(float tz)
{
	_positionZ+=tz;
	_center.z+=tz;
	_dirtyBits |= CAMERA_DIRTY_VIEW;
}
Mat4& Camera3D::getProjectionMatrix() 
{
	if (_dirtyBits & CAMERA_DIRTY_PROJ)
	{
		if (_type == PERSPECTIVE)
		{
			Mat4::createPerspective(_fieldOfView, _aspectRatio, _nearPlane, _farPlane, &_projection);
		}
		else
		{
			Mat4::createOrthographic(_zoom[0], _zoom[1], _nearPlane, _farPlane, &_projection);
		}
		_dirtyBits &= ~CAMERA_DIRTY_PROJ;
	}
	return _projection;
}
Mat4& Camera3D::getViewMatrix()
{
	if (_dirtyBits & CAMERA_DIRTY_VIEW)
	{
		_view= Mat4::IDENTITY;
		Mat4::createLookAt(getPosition3D(), _center, _up, &_view);
		_dirtyBits &= ~CAMERA_DIRTY_VIEW;
	}
	return _view;
}
void Camera3D::rotateX(float angle)
{
	Vec3 	eye=this->getPosition3D();
	Mat4	tRotateMat;
	tRotateMat.rotateX(CC_DEGREES_TO_RADIANS(angle));
	Vec3    tLookAtDir = _center - eye ;
	Vec3    tNormalDir = tLookAtDir.getNormalized();
	float   fDistance  = tLookAtDir.length();
	tRotateMat.transformVector(&tNormalDir);
	_center = eye + tNormalDir*fDistance;
	_dirtyBits |= CAMERA_DIRTY_VIEW;
}
void Camera3D::rotateY(float angle)
{
	Vec3 	eye=this->getPosition3D();
	Mat4	tRotateMat;
	tRotateMat.rotateY(CC_DEGREES_TO_RADIANS(angle));
	Vec3    tLookAtDir = _center - eye ;
	Vec3    tNormalDir = tLookAtDir.getNormalized();
	float   fDistance  = tLookAtDir.length();
	tRotateMat.transformVector(&tNormalDir);
	_center = eye + tNormalDir*fDistance;
	_dirtyBits |= CAMERA_DIRTY_VIEW;
}
void Camera3D::rotateZ(float angle)
{
	Vec3 	eye=this->getPosition3D();
	Mat4	tRotateMat;
	tRotateMat.rotateZ(CC_DEGREES_TO_RADIANS(angle));
	Vec3    tLookAtDir = _center - eye ;
	Vec3    tNormalDir = tLookAtDir.getNormalized();
	float   fDistance  = tLookAtDir.length();
	tRotateMat.transformVector(&tNormalDir);
	_center = eye + tNormalDir*fDistance;
	_dirtyBits |= CAMERA_DIRTY_VIEW;
}
void Camera3D::applyProjection()
{
	getProjectionMatrix();
	getViewMatrix();
	Director* director = Director::getInstance();
	//Size size = director->getWinSize();
	//director->setViewport();
	director->loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
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
NS_CC_END