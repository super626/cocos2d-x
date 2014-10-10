/****************************************************************************
origin from https://github.com/progschj/ThreadPool
 ****************************************************************************/
#include "base/CCThreadPool.h"

NS_CC_BEGIN

ThreadPool* ThreadPool::getInstance()
{
    static ThreadPool s_ThreadPool;
    return &s_ThreadPool;
}

NS_CC_END
