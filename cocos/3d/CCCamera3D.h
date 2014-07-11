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
#ifndef _CCCAMERA3D_H__
#define _CCCAMERA3D_H__
#include "2d/CCNode.h"
NS_CC_BEGIN
// Camera3D dirty bits
#define CAMERA_DIRTY_VIEW 1
#define CAMERA_DIRTY_PROJ 2
#define CAMERA_DIRTY_ALL (CAMERA_DIRTY_VIEW | CAMERA_DIRTY_PROJ)
/**
 * Defines a camera which acts as a view of a scene to be rendered.
 */
class Camera3D :public Node
{
public:

	/**
     * create camera
     */
	static Camera3D*	create();
	    /**
     * The type of camera.
     */

    enum Type
    {
        PERSPECTIVE = 1,
        ORTHOGRAPHIC = 2
    };
	public:
    /**
     * Creates a perspective camera.
     *
     * @param fieldOfView The field of view for the perspective camera (normally in the range of 40-60 degrees).
     * @param aspectRatio The aspect ratio of the camera (normally the width of the viewport divided by the height of the viewport).
     * @param nearPlane The near plane distance.
     * @param farPlane The far plane distance.
	 * @param strId camera id
     */
    static Camera3D*    createPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);
	 /**
     * Creates an orthographic camera.
     *
     * @param zoomX The zoom factor along the X-axis of the orthographic projection (the width of the ortho projection).
     * @param zoomY The zoom factor along the Y-axis of the orthographic projection (the height of the ortho projection).
     * @param aspectRatio The aspect ratio of the orthographic projection.
     * @param nearPlane The near plane distance.
     * @param farPlane The far plane distance.
	 * @param strId Camera id
     */
    static Camera3D*  createOrthographic(float zoomX, float zoomY, float aspectRatio, float nearPlane, float farPlane);
	  /**
     * Gets the type of camera.
     *
     * @return The camera type.
     */
    Camera3D::Type getCameraType() const;
private:
	Camera3D(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);
    Camera3D(float zoomX, float zoomY, float aspectRatio, float nearPlane, float farPlane);
	Camera3D();
	~Camera3D();
public:
	/* sets the Eye value of the Camera */
    void setEye(const Vec3 &eye);
    void setEye(float x, float y, float z);
    /* returns the Eye value of the Camera */
    const Vec3& getEye() const { return _eye; }
    /* sets the Center value of the Camera */
    void setCenter(const Vec3 &center);
    /* returns the Center value of the Camera */
    const Vec3& getCenter() const { return _center; }
    /* sets the Up value of the Camera */
    void setUp(const Vec3 &up);
    /* Returns the Up value of the Camera */
    const Vec3& getUp() const { return _up; }
	/**
     * Gets the camera's projection matrix.
     *
     * @return The camera projection matrix.
     */
	 Mat4& getProjectionMatrix();
		/**
     * Gets the camera's view matrix.
     *
     * @return The camera view matrix.
     */
     Mat4& getViewMatrix();
	/**
     * translate camera
     */
	void translateX(float tx);
    void translateY(float ty);
    void translateZ(float tz);
	/**
     * rotate camera
     */
	void rotateX(float angle);
	void rotateY(float angle);
	void rotateZ(float angle);
	void applyProjection();
private:
	Vec3 _center;
    Vec3 _eye;
    Vec3 _up;
	Mat4 _view;
	Mat4 _projection;
	Camera3D::Type _type;
    float _fieldOfView;
    float _zoom[2];
    float _aspectRatio;
    float _nearPlane;
    float _farPlane;
	int  _dirtyBits;
};
NS_CC_END
#endif// __CCCAMERA3D_H_