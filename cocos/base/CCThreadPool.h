/****************************************************************************
origin from https://github.com/progschj/ThreadPool
 ****************************************************************************/
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <cstddef>

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include "platform/CCPlatformMacros.h"
#include "base/ccMacros.h"

NS_CC_BEGIN

namespace memory_sequential_consistent {
    template<typename Element, size_t Size>
    class CircularFifo{
    public:
        enum { Capacity = Size+1 };
        
        CircularFifo() : _tail(0), _head(0){}
        virtual ~CircularFifo() {}
        
        bool push(const Element& item); // pushByMOve?
        bool pop(Element& item);
        
        bool wasEmpty() const;
        bool wasFull() const;
        bool isLockFree() const;
        
    private:
        size_t increment(size_t idx) const;
        
        std::atomic <size_t>  _tail;  // tail(input) index
        Element    _array[Capacity];
        std::atomic<size_t>   _head; // head(output) index
    };
    
    
    // Here with memory_order_seq_cst for every operation. This is overkill but easy to reason about
    //
    // Push on tail. TailHead is only changed by producer and can be safely loaded using memory_order_relexed
    //         head is updated by consumer and must be loaded using at least memory_order_acquire
    template<typename Element, size_t Size>
    bool CircularFifo<Element, Size>::push(const Element& item)
    {
        const auto current_tail = _tail.load();
        const auto next_tail = increment(current_tail);
        if(next_tail != _head.load())
        {
            _array[current_tail] = item;
            _tail.store(next_tail);
            return true;
        }
        
        return false;  // full queue
    }
    
    
    // Pop by Consumer can only update the head
    template<typename Element, size_t Size>
    bool CircularFifo<Element, Size>::pop(Element& item)
    {
        const auto current_head = _head.load();
        if(current_head == _tail.load())
            return false;   // empty queue
        
        item = _array[current_head];
        _head.store(increment(current_head));
        return true;
    }
    
    // snapshot with acceptance of that this comparison function is not atomic
    // (*) Used by clients or test, since pop() avoid double load overhead by not
    // using wasEmpty()
    template<typename Element, size_t Size>
    bool CircularFifo<Element, Size>::wasEmpty() const
    {
        return (_head.load() == _tail.load());
    }
    
    // snapshot with acceptance that this comparison is not atomic
    // (*) Used by clients or test, since push() avoid double load overhead by not
    // using wasFull()
    template<typename Element, size_t Size>
    bool CircularFifo<Element, Size>::wasFull() const
    {
        const auto next_tail = increment(_tail.load());
        return (next_tail == _head.load());
    }
    
    
    template<typename Element, size_t Size>
    bool CircularFifo<Element, Size>::isLockFree() const
    {
        return (_tail.is_lock_free() && _head.is_lock_free());
    }
    
    template<typename Element, size_t Size>
    size_t CircularFifo<Element, Size>::increment(size_t idx) const
    {
        return (idx + 1) % Capacity;
    }
}

class ThreadPool {
public:
    ThreadPool();
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>;
    ~ThreadPool();
    
    // start thread pool, should call start in order to use the thread pool
    void start();
    // stop thread pool
    void stop();
    
private:
    // need to keep track of threads so we can join them, we use one worker thread for now
    std::vector< std::thread > _workers;
    
    bool _stop;
    
    memory_sequential_consistent::CircularFifo<std::function<void()>, 256 > _taskQueue;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool()
:   _stop(false)
{
    //start();
}

inline void ThreadPool::start()
{
    if (!_stop)
        return;
    
    size_t threads = 1;
    for(size_t i = 0;i<threads;++i)
        _workers.emplace_back(
                              [this]
                              {
                                  for(;;)
                                  {
                                      std::function<void()> task;
                                      while(_taskQueue.pop(task) == false && !_stop)
                                      {
                                          std::this_thread::yield();
                                      }
                                      
                                      if (_stop)
                                          return;
                                      
                                      task();
                                      
                                  }
                              }
                              );
}

inline void ThreadPool::stop()
{
    if (!_stop)
    {
        _stop = true;
        
        for(size_t i = 0;i<_workers.size();++i)
            _workers[i].join();
    }
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type>
{
    typedef typename std::result_of<F(Args...)>::type return_type;
    
    // don't allow enqueueing after stopping the pool
    CCASSERT(!_stop, "enqueue on stopped ThreadPool");
    
    auto task = std::make_shared< std::packaged_task<return_type()> >(
                                                                      std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                                                                      );
    
    std::future<return_type> res = task->get_future();
    {
        while(_taskQueue.push([task](){ (*task)(); }) == false)
        {
            std::this_thread::yield();
        }
    }

    return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    stop();
}

NS_CC_END

#endif
