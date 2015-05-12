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

#ifndef __CCNAV_MESH_OBSTACLE_COMPONENT_H__
#define __CCNAV_MESH_OBSTACLE_COMPONENT_H__

#include "base/ccConfig.h"
#include "2d/CCComponent.h"


NS_CC_BEGIN

/**
* @addtogroup 3d
* @{
*/
class NavMeshObstacle;
class CC_DLL NavMeshObstacleComponent : public Component
{
public:

	static NavMeshObstacleComponent* create(NavMeshObstacle *obstacle);


	virtual void onEnter();
	virtual void onExit();

CC_CONSTRUCTOR_ACCESS:
	NavMeshObstacleComponent();
	virtual ~NavMeshObstacleComponent();

	bool init(NavMeshObstacle *obstacle);

protected:

	NavMeshObstacle *_obstacle;
};

/** @} */

NS_CC_END

#endif // __CCNAV_MESH_AGENT_COMPONENT_H__
